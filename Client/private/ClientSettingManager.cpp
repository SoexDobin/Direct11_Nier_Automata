#include "pch.h"
#include "ClientSettingManager.h"

#include <fstream>
#include <SpdLogger.h>
#include "Client_Define.h"
#include "Game.h"
#include "TagRegistry.h"
#include "LayerRegistry.h"
#include "GameObject.h"
#include "Navigation.h"
#include "Engine_ID.h"
// zlib declares a global Byte type; isolate it from Engine::Byte in Client unity builds.
#define Byte MinizipByte
#include <minizip/unzip.h>
#include <minizip/iowin32.h>
#undef Byte
#include <pugixml.hpp>
#include <regex>
#include <unordered_set>

namespace
{
	constexpr size_t MaxXlsxEntrySize = 64ull * 1024ull * 1024ull;
	constexpr uint32 MaxSettingRows = 100'000;

	struct XlsxArchiveCloser
	{
		void operator()(void* archive) const
		{
			if (archive)
				unzClose(static_cast<unzFile>(archive));
		}
	};

	using XlsxArchive = unique_ptr<void, XlsxArchiveCloser>;

	string CleanExcelCell(string value)
	{
		return Helper::Trim(value);
	}

	string ToLowerAscii(string value)
	{
		transform(value.begin(), value.end(), value.begin(), [](unsigned char ch)
		{
			return static_cast<Char>(std::tolower(ch));
		});
		return value;
	}

	Bool TryParseExcelFloat(const string& value, Float defaultValue, Float& outValue)
	{
		const string cleaned = CleanExcelCell(value);
		if (cleaned.empty())
		{
			outValue = defaultValue;
			return true;
		}

		try
		{
			size_t parsedLength = 0;
			outValue = std::stof(cleaned, &parsedLength);
			return parsedLength == cleaned.size() && std::isfinite(outValue);
		}
		catch (...)
		{
			return false;
		}
	}

	Bool TryParseExcelUint32(const string& value, uint32& outValue)
	{
		const string cleaned = CleanExcelCell(value);
		if (cleaned.empty())
			return false;

		try
		{
			size_t parsedLength = 0;
			const unsigned long long parsedValue = std::stoull(cleaned, &parsedLength);
			if (parsedLength != cleaned.size() || parsedValue == 0 ||
				parsedValue > std::numeric_limits<uint32>::max())
			{
				return false;
			}
			outValue = static_cast<uint32>(parsedValue);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	Bool TryParseModelLevel(const string& value, LEVEL& outLevel)
	{
		const string normalized = ToLowerAscii(CleanExcelCell(value));
		if (normalized == "0" || normalized == "static")
			outLevel = LEVEL::STATIC;
		else if (normalized == "1" || normalized == "loading")
			outLevel = LEVEL::LOADING;
		else if (normalized == "2" || normalized == "title")
			outLevel = LEVEL::TITLE;
		else if (normalized == "3" || normalized == "gameplay")
			outLevel = LEVEL::GAMEPLAY;
		else
			return false;
		return true;
	}

	HRESULT ReadXlsxEntry(unzFile archive, const Char* entryPath, string& outData)
	{
		if (!archive || !entryPath || unzLocateFile(archive, entryPath, 1) != UNZ_OK)
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		unz_file_info64 entryInfo{};
		if (unzGetCurrentFileInfo64(archive, &entryInfo, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK ||
			entryInfo.uncompressed_size > MaxXlsxEntrySize)
		{
			return E_FAIL;
		}

		if (unzOpenCurrentFile(archive) != UNZ_OK)
			return E_FAIL;

		outData.assign(static_cast<size_t>(entryInfo.uncompressed_size), '\0');
		size_t totalRead = 0;
		while (totalRead < outData.size())
		{
			const unsigned chunkSize = static_cast<unsigned>(min<size_t>(64 * 1024, outData.size() - totalRead));
			const int readSize = unzReadCurrentFile(archive, outData.data() + totalRead, chunkSize);
			if (readSize <= 0)
			{
				unzCloseCurrentFile(archive);
				return E_FAIL;
			}
			totalRead += static_cast<size_t>(readSize);
		}

		return unzCloseCurrentFile(archive) == UNZ_OK ? S_OK : E_FAIL;
	}

	HRESULT LoadXlsxXml(unzFile archive, const Char* entryPath, pugi::xml_document& outDocument)
	{
		string xmlData;
		if (FAILED(ReadXlsxEntry(archive, entryPath, xmlData)))
			return E_FAIL;

		const pugi::xml_parse_result parseResult = outDocument.load_buffer(
			xmlData.data(), xmlData.size(), pugi::parse_default, pugi::encoding_auto);
		return parseResult ? S_OK : E_FAIL;
	}

	void AppendXlsxText(const pugi::xml_node& node, string& outText)
	{
		if (strcmp(node.name(), "t") == 0)
		{
			outText += node.text().as_string();
			return;
		}

		for (const pugi::xml_node& child : node.children())
			AppendXlsxText(child, outText);
	}

	HRESULT LoadSharedStrings(unzFile archive, vector<string>& outSharedStrings)
	{
		if (unzLocateFile(archive, "xl/sharedStrings.xml", 1) != UNZ_OK)
			return S_OK;

		pugi::xml_document sharedStringsDocument;
		if (FAILED(LoadXlsxXml(archive, "xl/sharedStrings.xml", sharedStringsDocument)))
			return E_FAIL;

		const pugi::xml_node root = sharedStringsDocument.document_element();
		for (const pugi::xml_node& item : root.children("si"))
		{
			string text;
			AppendXlsxText(item, text);
			outSharedStrings.push_back(std::move(text));
		}
		return S_OK;
	}

	HRESULT FindWorksheetPath(unzFile archive, const string& worksheetName, string& outWorksheetPath)
	{
		pugi::xml_document workbookDocument;
		pugi::xml_document relationshipDocument;
		if (FAILED(LoadXlsxXml(archive, "xl/workbook.xml", workbookDocument)) ||
			FAILED(LoadXlsxXml(archive, "xl/_rels/workbook.xml.rels", relationshipDocument)))
		{
			return E_FAIL;
		}

		string relationshipId;
		const pugi::xml_node workbookRoot = workbookDocument.document_element();
		for (const pugi::xml_node& sheet : workbookRoot.child("sheets").children("sheet"))
		{
			if (sheet.attribute("name").as_string() == worksheetName)
			{
				relationshipId = sheet.attribute("r:id").as_string();
				break;
			}
		}
		if (relationshipId.empty())
			return E_FAIL;

		string target;
		for (const pugi::xml_node& relationship : relationshipDocument.document_element().children("Relationship"))
		{
			if (relationship.attribute("Id").as_string() == relationshipId)
			{
				target = relationship.attribute("Target").as_string();
				break;
			}
		}
		if (target.empty())
			return E_FAIL;

		filesystem::path worksheetPath;
		if (target.front() == '/')
			worksheetPath = filesystem::path(target.substr(1));
		else
			worksheetPath = filesystem::path("xl") / filesystem::path(target);
		worksheetPath = worksheetPath.lexically_normal();
		if (worksheetPath.empty() || *worksheetPath.begin() == "..")
			return E_FAIL;

		outWorksheetPath = worksheetPath.generic_string();
		return S_OK;
	}

	Bool TryGetColumnIndex(const string& cellReference, size_t& outColumnIndex)
	{
		size_t column = 0;
		size_t letterCount = 0;
		for (const unsigned char ch : cellReference)
		{
			if (!std::isalpha(ch))
				break;
			const unsigned char upper = static_cast<unsigned char>(std::toupper(ch));
			column = column * 26 + static_cast<size_t>(upper - 'A' + 1);
			++letterCount;
		}
		if (letterCount == 0 || column == 0)
			return false;
		outColumnIndex = column - 1;
		return true;
	}

	Bool TryReadXlsxCell(const pugi::xml_node& cell, const vector<string>& sharedStrings, string& outValue)
	{
		const string cellType = cell.attribute("t").as_string();
		if (cellType == "e")
			return false;
		if (cellType == "inlineStr")
		{
			AppendXlsxText(cell.child("is"), outValue);
			return true;
		}

		const pugi::xml_node valueNode = cell.child("v");
		if (!valueNode)
			return true;
		const string rawValue = valueNode.text().as_string();
		if (cellType != "s")
		{
			outValue = rawValue;
			return true;
		}

		try
		{
			size_t parsedLength = 0;
			const size_t sharedStringIndex = std::stoull(rawValue, &parsedLength);
			if (parsedLength != rawValue.size() || sharedStringIndex >= sharedStrings.size())
				return false;
			outValue = sharedStrings[sharedStringIndex];
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	HRESULT ParseModelSettingsWorkbook(const filesystem::path& workbookPath,
		const filesystem::path& resourceRoot, nlohmann::json& outJson)
	{
		zlib_filefunc64_def fileFunctions{};
		fill_win32_filefunc64W(&fileFunctions);
		XlsxArchive archive(unzOpen2_64(workbookPath.c_str(), &fileFunctions));
		if (!archive)
		{
			LOG_ERROR(L"Failed to open ModelSettings workbook : {}", workbookPath.wstring());
			return E_FAIL;
		}

		vector<string> sharedStrings;
		string worksheetPath;
		if (FAILED(LoadSharedStrings(archive.get(), sharedStrings)) ||
			FAILED(FindWorksheetPath(archive.get(), "Models", worksheetPath)))
		{
			LOG_ERROR(L"Invalid ModelSettings workbook structure or missing Models worksheet : {}", workbookPath.wstring());
			return E_FAIL;
		}

		pugi::xml_document worksheetDocument;
		if (FAILED(LoadXlsxXml(archive.get(), worksheetPath.c_str(), worksheetDocument)))
		{
			LOG_ERROR(L"Failed to read Models worksheet : {}", workbookPath.wstring());
			return E_FAIL;
		}

		vector<pair<uint32, unordered_map<size_t, string>>> rows;
		uint32 fallbackRowNumber = 0;
		for (const pugi::xml_node& row : worksheetDocument.document_element().child("sheetData").children("row"))
		{
			if (rows.size() >= MaxSettingRows)
			{
				LOG_ERROR(L"ModelSettings.xlsx exceeds the maximum row count {}", MaxSettingRows);
				return E_FAIL;
			}

			const uint32 rowNumber = row.attribute("r").as_uint(++fallbackRowNumber);
			fallbackRowNumber = max(fallbackRowNumber, rowNumber);
			unordered_map<size_t, string> cells;
			for (const pugi::xml_node& cell : row.children("c"))
			{
				const string cellReference = cell.attribute("r").as_string();
				size_t columnIndex = 0;
				string value;
				if (!TryGetColumnIndex(cellReference, columnIndex) ||
					!TryReadXlsxCell(cell, sharedStrings, value) ||
					!cells.emplace(columnIndex, std::move(value)).second)
				{
					LOG_ERROR(L"Invalid or duplicate ModelSettings cell {}", Helper::To_wString(cellReference));
					return E_FAIL;
				}
			}
			rows.emplace_back(rowNumber, std::move(cells));
		}

		if (rows.empty())
		{
			LOG_ERROR(L"Models worksheet is empty : {}", workbookPath.wstring());
			return E_FAIL;
		}

		const array<string, 13> requiredHeaders{
			"Level", "Tag", "Path", "PosX", "PosY", "PosZ", "RotX", "RotY", "RotZ",
			"ScaleX", "ScaleY", "ScaleZ", "AnimationPresetGuid"
		};
		unordered_map<string, size_t> headerColumns;
		const auto& [headerRowNumber, headerCells] = rows.front();
		for (const auto& [columnIndex, headerValue] : headerCells)
		{
			const string normalizedHeader = ToLowerAscii(CleanExcelCell(headerValue));
			if (!normalizedHeader.empty() && !headerColumns.emplace(normalizedHeader, columnIndex).second)
			{
				LOG_ERROR(L"Duplicate header {} in Models row {}",
					Helper::To_wString(headerValue), headerRowNumber);
				return E_FAIL;
			}
		}
		for (const string& requiredHeader : requiredHeaders)
		{
			if (!headerColumns.contains(ToLowerAscii(requiredHeader)))
			{
				LOG_ERROR(L"Missing required Models header {}", Helper::To_wString(requiredHeader));
				return E_FAIL;
			}
		}

		auto getCell = [&headerColumns](const unordered_map<size_t, string>& cells, const Char* header)
		{
			const auto columnIt = headerColumns.find(ToLowerAscii(header));
			if (columnIt == headerColumns.end())
				return string{};
			const auto cellIt = cells.find(columnIt->second);
			return cellIt == cells.end() ? string{} : CleanExcelCell(cellIt->second);
		};

		outJson = nlohmann::json::object();
		outJson["ModelSettings"] = nlohmann::json::array();
		unordered_set<string> prototypeKeys;
		for (size_t rowIndex = 1; rowIndex < rows.size(); ++rowIndex)
		{
			const auto& [rowNumber, cells] = rows[rowIndex];
			Bool hasValue = false;
			for (const auto& [columnIndex, value] : cells)
			{
				if (!CleanExcelCell(value).empty())
				{
					hasValue = true;
					break;
				}
			}
			if (!hasValue)
				continue;

			const string level = getCell(cells, "Level");
			const string tag = getCell(cells, "Tag");
			const string path = getCell(cells, "Path");
			const string animationPresetGuid = getCell(cells, "AnimationPresetGuid");
			if (level.empty() || tag.empty() || path.empty())
			{
				LOG_ERROR(L"Missing required Level, Tag, or Path in Models row {}", rowNumber);
				return E_FAIL;
			}

			LEVEL parsedLevel{};
			if (!TryParseModelLevel(level, parsedLevel))
			{
				LOG_ERROR(L"Invalid Level in Models row {} : {}", rowNumber, Helper::To_wString(level));
				return E_FAIL;
			}

			const string prototypeKey = std::to_string(ETOI(parsedLevel)) + "\n" + tag;
			if (!prototypeKeys.insert(prototypeKey).second)
			{
				LOG_ERROR(L"Duplicate Model prototype Tag in Models row {} : {}", rowNumber, Helper::To_wString(tag));
				return E_FAIL;
			}

			const filesystem::path relativeModelPath =
				filesystem::path(Helper::To_wString(path)).lexically_normal();
			if (relativeModelPath.empty() || relativeModelPath.is_absolute() || relativeModelPath.has_root_name() ||
				*relativeModelPath.begin() == L".." ||
				ToLowerAscii(relativeModelPath.extension().string()) != ".model" ||
				!filesystem::is_regular_file(resourceRoot / relativeModelPath))
			{
				LOG_ERROR(L"Invalid or missing Model Path in Models row {} : {}",
					rowNumber, Helper::To_wString(path));
				return E_FAIL;
			}

			AssetGuid parsedPresetGuid{};
			if (!animationPresetGuid.empty() && !Try_Parse_AssetGuid(animationPresetGuid, parsedPresetGuid))
			{
				LOG_ERROR(L"Invalid AnimationPresetGuid in Models row {} : {}",
					rowNumber, Helper::To_wString(animationPresetGuid));
				return E_FAIL;
			}

			Float px{}, py{}, pz{}, rx{}, ry{}, rz{}, sx{}, sy{}, sz{};
			struct NumericField
			{
				const Char* header;
				Float defaultValue;
				Float* output;
			};
			const array<NumericField, 9> numericFields{
				NumericField{"PosX", 0.f, &px}, NumericField{"PosY", 0.f, &py}, NumericField{"PosZ", 0.f, &pz},
				NumericField{"RotX", 0.f, &rx}, NumericField{"RotY", 0.f, &ry}, NumericField{"RotZ", 0.f, &rz},
				NumericField{"ScaleX", 1.f, &sx}, NumericField{"ScaleY", 1.f, &sy}, NumericField{"ScaleZ", 1.f, &sz}
			};
			for (const NumericField& field : numericFields)
			{
				if (!TryParseExcelFloat(getCell(cells, field.header), field.defaultValue, *field.output))
				{
					LOG_ERROR(L"Invalid numeric value in Models row {}, column {}",
						rowNumber, Helper::To_wString(field.header));
					return E_FAIL;
				}
			}

			outJson["ModelSettings"].push_back({
				{"level", level},
				{"tag", tag},
				{"path", path},
				{"position", {{"x", px}, {"y", py}, {"z", pz}}},
				{"rotation", {{"x", rx}, {"y", ry}, {"z", rz}}},
				{"scale", {{"x", sx}, {"y", sy}, {"z", sz}}},
				{"animationPresetGuid", animationPresetGuid}
			});
		}

		return S_OK;
	}

	HRESULT ParseTextureSettingsWorkbook(const filesystem::path& workbookPath, nlohmann::json& outJson)
	{
		zlib_filefunc64_def fileFunctions{};
		fill_win32_filefunc64W(&fileFunctions);
		XlsxArchive archive(unzOpen2_64(workbookPath.c_str(), &fileFunctions));
		if (!archive)
		{
			LOG_ERROR(L"Failed to open TextureSettings workbook : {}", workbookPath.wstring());
			return E_FAIL;
		}

		vector<string> sharedStrings;
		string worksheetPath;
		if (FAILED(LoadSharedStrings(archive.get(), sharedStrings)) ||
			FAILED(FindWorksheetPath(archive.get(), "Textures", worksheetPath)))
		{
			LOG_ERROR(L"Invalid TextureSettings workbook structure or missing Textures worksheet : {}",
				workbookPath.wstring());
			return E_FAIL;
		}

		pugi::xml_document worksheetDocument;
		if (FAILED(LoadXlsxXml(archive.get(), worksheetPath.c_str(), worksheetDocument)))
		{
			LOG_ERROR(L"Failed to read Textures worksheet : {}", workbookPath.wstring());
			return E_FAIL;
		}

		vector<pair<uint32, unordered_map<size_t, string>>> rows;
		uint32 fallbackRowNumber = 0;
		for (const pugi::xml_node& row : worksheetDocument.document_element().child("sheetData").children("row"))
		{
			if (rows.size() >= MaxSettingRows)
			{
				LOG_ERROR(L"TextureSettings.xlsx exceeds the maximum row count {}", MaxSettingRows);
				return E_FAIL;
			}

			const uint32 rowNumber = row.attribute("r").as_uint(++fallbackRowNumber);
			fallbackRowNumber = max(fallbackRowNumber, rowNumber);
			unordered_map<size_t, string> cells;
			for (const pugi::xml_node& cell : row.children("c"))
			{
				const string cellReference = cell.attribute("r").as_string();
				size_t columnIndex = 0;
				string value;
				if (!TryGetColumnIndex(cellReference, columnIndex) ||
					!TryReadXlsxCell(cell, sharedStrings, value) ||
					!cells.emplace(columnIndex, std::move(value)).second)
				{
					LOG_ERROR(L"Invalid or duplicate TextureSettings cell {}", Helper::To_wString(cellReference));
					return E_FAIL;
				}
			}
			rows.emplace_back(rowNumber, std::move(cells));
		}

		if (rows.empty())
		{
			LOG_ERROR(L"Textures worksheet is empty : {}", workbookPath.wstring());
			return E_FAIL;
		}

		const array<string, 4> requiredHeaders{ "Level", "Tag", "Path", "Count" };
		unordered_map<string, size_t> headerColumns;
		const auto& [headerRowNumber, headerCells] = rows.front();
		for (const auto& [columnIndex, headerValue] : headerCells)
		{
			const string normalizedHeader = ToLowerAscii(CleanExcelCell(headerValue));
			if (!normalizedHeader.empty() && !headerColumns.emplace(normalizedHeader, columnIndex).second)
			{
				LOG_ERROR(L"Duplicate header {} in Textures row {}",
					Helper::To_wString(headerValue), headerRowNumber);
				return E_FAIL;
			}
		}
		for (const string& requiredHeader : requiredHeaders)
		{
			if (!headerColumns.contains(ToLowerAscii(requiredHeader)))
			{
				LOG_ERROR(L"Missing required Textures header {}", Helper::To_wString(requiredHeader));
				return E_FAIL;
			}
		}

		auto getCell = [&headerColumns](const unordered_map<size_t, string>& cells, const Char* header)
		{
			const auto columnIt = headerColumns.find(ToLowerAscii(header));
			if (columnIt == headerColumns.end())
				return string{};
			const auto cellIt = cells.find(columnIt->second);
			return cellIt == cells.end() ? string{} : CleanExcelCell(cellIt->second);
		};

		outJson = nlohmann::json::object();
		outJson["TextureSettings"] = nlohmann::json::array();
		unordered_set<string> prototypeKeys;
		for (size_t rowIndex = 1; rowIndex < rows.size(); ++rowIndex)
		{
			const auto& [rowNumber, cells] = rows[rowIndex];
			Bool hasValue = false;
			for (const auto& [columnIndex, value] : cells)
			{
				if (!CleanExcelCell(value).empty())
				{
					hasValue = true;
					break;
				}
			}
			if (!hasValue)
				continue;

			const string level = getCell(cells, "Level");
			const string tag = getCell(cells, "Tag");
			const string path = getCell(cells, "Path");
			const string countText = getCell(cells, "Count");
			if (level.empty() || tag.empty() || path.empty() || countText.empty())
			{
				LOG_ERROR(L"Missing required Level, Tag, Path, or Count in Textures row {}", rowNumber);
				return E_FAIL;
			}

			LEVEL parsedLevel{};
			uint32 count = 0;
			if (!TryParseModelLevel(level, parsedLevel))
			{
				LOG_ERROR(L"Invalid Level in Textures row {} : {}", rowNumber, Helper::To_wString(level));
				return E_FAIL;
			}
			if (!TryParseExcelUint32(countText, count))
			{
				LOG_ERROR(L"Invalid Count in Textures row {} : {}", rowNumber, Helper::To_wString(countText));
				return E_FAIL;
			}

			const filesystem::path relativeTexturePath =
				filesystem::path(Helper::To_wString(path)).lexically_normal();
			if (relativeTexturePath.empty() || relativeTexturePath.is_absolute() ||
				relativeTexturePath.has_root_name() || *relativeTexturePath.begin() == L"..")
			{
				LOG_ERROR(L"Invalid Texture Path in Textures row {} : {}", rowNumber, Helper::To_wString(path));
				return E_FAIL;
			}

			const string prototypeKey = std::to_string(ETOI(parsedLevel)) + "\n" + tag;
			if (!prototypeKeys.insert(prototypeKey).second)
			{
				LOG_ERROR(L"Duplicate Texture prototype Tag in Textures row {} : {}",
					rowNumber, Helper::To_wString(tag));
				return E_FAIL;
			}

			outJson["TextureSettings"].push_back({
				{"level", level},
				{"tag", tag},
				{"path", path},
				{"count", count}
			});
		}

		return S_OK;
	}

	HRESULT WriteJsonAtomically(const filesystem::path& outputPath, const nlohmann::json& document)
	{
		const filesystem::path temporaryPath = outputPath.wstring() + L".tmp";
		{
			ofstream output(temporaryPath, std::ios::binary | std::ios::trunc);
			if (!output.is_open())
				return E_FAIL;
			output << document.dump(4);
			output.flush();
			if (!output.good())
			{
				output.close();
				std::error_code removeError;
				filesystem::remove(temporaryPath, removeError);
				return E_FAIL;
			}
		}

		if (!MoveFileExW(temporaryPath.c_str(), outputPath.c_str(),
			MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
		{
			std::error_code removeError;
			filesystem::remove(temporaryPath, removeError);
			return HRESULT_FROM_WIN32(GetLastError());
		}
		return S_OK;
	}

	HRESULT FindAnimationPresetPaths(const wstring& projectSettingPath, const wstring& resourcePath,
		const string& assetGuidText, vector<wstring>& outAnimationPaths)
	{
		AssetGuid assetGuid{};
		if (!Try_Parse_AssetGuid(assetGuidText, assetGuid))
		{
			LOG_ERROR(L"Invalid AnimationPreset AssetGuid : {}", Helper::To_wString(assetGuidText));
			return E_INVALIDARG;
		}

		const filesystem::path presetDirectory = filesystem::path(projectSettingPath) / L"AnimationPreset";
		if (!filesystem::exists(presetDirectory))
		{
			LOG_ERROR(L"AnimationPreset directory not found : {}", presetDirectory.wstring());
			return E_FAIL;
		}

		for (const auto& entry : filesystem::directory_iterator(presetDirectory))
		{
			if (!entry.is_regular_file() || entry.path().extension() != L".json")
				continue;

			try
			{
				ifstream presetFile(entry.path());
				nlohmann::json preset;
				presetFile >> preset;
				if (preset.value("assetGuid", string{}) != assetGuidText)
					continue;
				if (preset.value("schemaVersion", 0) != 1 || !preset.contains("animations") ||
					!preset["animations"].is_array())
				{
					LOG_ERROR(L"Invalid AnimationPreset schema : {}", entry.path().wstring());
					return E_FAIL;
				}

				vector<wstring> resolvedPaths;
				for (const auto& animation : preset["animations"])
				{
					const string relativePathText = animation.is_string()
						? animation.get<string>()
						: animation.value("path", string{});
					const filesystem::path relativePath = filesystem::path(Helper::To_wString(relativePathText)).lexically_normal();
					if (relativePath.empty() || relativePath.is_absolute() ||
						(!relativePath.empty() && *relativePath.begin() == L"..") ||
						relativePath.extension() != L".anim")
					{
						LOG_ERROR(L"Invalid AnimationPreset clip path : {}", Helper::To_wString(relativePathText));
						return E_FAIL;
					}

					const filesystem::path fullPath = filesystem::path(resourcePath) / relativePath;
					if (!filesystem::exists(fullPath))
					{
						LOG_ERROR(L"Animation clip not found : {}", fullPath.wstring());
						return E_FAIL;
					}
					resolvedPaths.push_back(fullPath.wstring());
				}

				if (resolvedPaths.empty())
				{
					LOG_ERROR(L"AnimationPreset has no clips : {}", entry.path().wstring());
					return E_FAIL;
				}

				outAnimationPaths = std::move(resolvedPaths);
				return S_OK;
			}
			catch (const std::exception& exception)
			{
				LOG_ERROR(L"Failed to parse AnimationPreset {} : {}", entry.path().wstring(), Helper::To_wString(exception.what()));
				return E_FAIL;
			}
		}

		LOG_ERROR(L"AnimationPreset AssetGuid not found : {}", Helper::To_wString(assetGuidText));
		return E_FAIL;
	}
}

IMPLEMENT_SINGLETON(ClientSettingManager)

ENGINE_DESC ClientSettingManager::g_EngineDesc{};

HRESULT ClientSettingManager::Load_Textures_FromJson(LEVEL baseLevel) const
{
	wstring fullPath = m_ProjectSettingPath + L"TextureSettings.json";

	if (!filesystem::exists(fullPath))
	{
		nlohmann::json defaultJson;
		defaultJson["TextureSettings"] = nlohmann::json::array();
		
		std::ofstream outFile(fullPath);
		if (outFile.is_open()) {
			outFile << defaultJson.dump(4);
			outFile.close();
		}
		LOG_INFO(L"Created Default TextureSettings.json: {}", fullPath);
	}

	if (!filesystem::exists(fullPath))
	{
		LOG_WARN(L"Failed To Find Texture Settings : {}", fullPath);
		return S_OK;
	}

	ifstream file(fullPath);
	if (!file.is_open()) return E_FAIL;

	nlohmann::json json;
	file >> json;
	file.close();

	auto& settings = json["TextureSettings"];
	for (auto& item : settings)
	{

		const string levelStr = item["level"];
		LEVEL level{};
		if (!TryParseModelLevel(levelStr, level))
		{
			LOG_ERROR(L"Invalid Model level in ModelSettings.json : {}", Helper::To_wString(levelStr));
			return E_FAIL;
		}
		
        if (level != baseLevel)
			if (level != LEVEL::LOADING)
				continue;
		
		wstring tag = Helper::To_wString(item["tag"].get<string>());
		wstring relativePath = Helper::To_wString(item["path"].get<string>());
		uint32 count = 1;
		if (item["count"].is_string())
			count = std::stoul(item["count"].get<string>());
		else
			count = item.value("count", 1);

		wstring fullTexturePath = m_ResourcePath + relativePath;
		
		if (FAILED(GAME_INSTANCE->Load_Texture(ETOI(level), fullTexturePath.c_str(), count, tag)))
		{
			LOG_ERROR(L"Failed to Load Texture Prototype: {}", tag);
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT ClientSettingManager::Sync_TextureJson_FromExcel() const
{
	const filesystem::path workbookPath = filesystem::path(m_ProjectSettingPath) / L"TextureSettings.xlsx";
	const filesystem::path jsonPath = filesystem::path(m_ProjectSettingPath) / L"TextureSettings.json";
	if (!filesystem::exists(workbookPath))
	{
		if (filesystem::exists(jsonPath))
		{
			LOG_WARN(L"TextureSettings.xlsx is missing; using the existing TextureSettings.json cache");
			return S_FALSE;
		}
		LOG_ERROR(L"TextureSettings.xlsx and TextureSettings.json are both missing : {}", m_ProjectSettingPath);
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	nlohmann::json jsonRoot;
	if (FAILED(ParseTextureSettingsWorkbook(workbookPath, jsonRoot)))
	{
		LOG_ERROR(L"Failed to synchronize TextureSettings.xlsx : {}", workbookPath.wstring());
		return E_FAIL;
	}

	const HRESULT writeResult = WriteJsonAtomically(jsonPath, jsonRoot);
	if (FAILED(writeResult))
	{
		LOG_ERROR(L"Failed to replace TextureSettings.json from workbook : {}", jsonPath.wstring());
		return writeResult;
	}

	LOG_INFO(L"Synchronized {} Texture settings from {}",
		jsonRoot["TextureSettings"].size(), workbookPath.wstring());
	return S_OK;
}

HRESULT ClientSettingManager::Load_Model_FromJson(LEVEL baseLevel) const
{
	wstring fullPath = m_ProjectSettingPath + L"ModelSettings.json";

	if (!filesystem::exists(fullPath))
	{
		nlohmann::json defaultJson;
		defaultJson["ModelSettings"] = nlohmann::json::array();

		std::ofstream outFile(fullPath);
		if (outFile.is_open()) {
			outFile << defaultJson.dump(3);
			outFile.close();
		}
		LOG_INFO(L"Created Default ModelSettings.json: {}", fullPath);
	}

	if (!filesystem::exists(fullPath))
	{
		LOG_WARN(L"Failed To Find Model Settings : {}", fullPath);
		return S_OK;
	}

	ifstream file(fullPath);
	if (!file.is_open()) return E_FAIL;

	nlohmann::json json;
	file >> json;
	file.close();

	auto& settings = json["ModelSettings"];
	for (auto& item : settings)
	{
		string levelStr = item["level"];
		LEVEL level = LEVEL::STATIC;
		if (levelStr == "1" || levelStr == "Loading" || levelStr == "LOADING")
			level = LEVEL::LOADING;
		else if (levelStr == "2" || levelStr == "Title" || levelStr == "TITLE")
			level = LEVEL::TITLE;
		else if (levelStr == "3" || levelStr == "GamePlay" || levelStr == "GAMEPLAY")
			level = LEVEL::GAMEPLAY;
		
        if (level != baseLevel)
            continue;
		
		wstring tag = Helper::To_wString(item["tag"].get<string>());
		wstring relativePath = Helper::To_wString(item["path"].get<string>());
		wstring fullTexturePath = m_ResourcePath + relativePath;

		// JSON에서 트랜스포메이션 데이터 추출 및 기본값 처리
		Vector3 vPos = Vector3::Zero;
		if (item.contains("position"))
		{
			vPos.x = item["position"].value("x", 0.f);
			vPos.y = item["position"].value("y", 0.f);
			vPos.z = item["position"].value("z", 0.f);
		}

		Vector3 vRot = Vector3::Zero;
		if (item.contains("rotation"))
		{
			vRot.x = item["rotation"].value("x", 0.f);
			vRot.y = item["rotation"].value("y", 0.f);
			vRot.z = item["rotation"].value("z", 0.f);
		}

		Vector3 vScale = Vector3::One;
		if (item.contains("scale"))
		{
			vScale.x = item["scale"].value("x", 1.f);
			vScale.y = item["scale"].value("y", 1.f);
			vScale.z = item["scale"].value("z", 1.f);
		}

		// 월드 행렬 생성 (Scale * Rotation * Translation)
		// Rotation은 오일러(Degree) -> Radian 변환 후 YawPitchRoll로 생성
		Matrix matWorld = Matrix::CreateScale(vScale) * 
						  Matrix::CreateFromYawPitchRoll(XMConvertToRadians(vRot.y), XMConvertToRadians(vRot.x), XMConvertToRadians(vRot.z)) * 
						  Matrix::CreateTranslation(vPos);

		if (FAILED(GAME_INSTANCE->Load_Model(ETOI(level), fullTexturePath.c_str(), tag, matWorld)))
		{
			LOG_ERROR(L"Failed to Load Model Prototype: {}", tag);
			return E_FAIL;
		}

		const string animationPresetGuid = item.value("animationPresetGuid", string{});
		if (!animationPresetGuid.empty())
		{
			vector<wstring> animationPaths;
			if (FAILED(FindAnimationPresetPaths(m_ProjectSettingPath, m_ResourcePath,
				animationPresetGuid, animationPaths)) ||
				FAILED(GAME_INSTANCE->Load_ModelAnimations(ETOI(level), tag, animationPaths)))
			{
				LOG_ERROR(L"Failed to apply AnimationPreset {} to model {}",
					Helper::To_wString(animationPresetGuid), tag);
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT ClientSettingManager::Sync_ModelJson_FromExcel() const
{
	const filesystem::path workbookPath = filesystem::path(m_ProjectSettingPath) / L"ModelSettings.xlsx";
	const filesystem::path jsonPath = filesystem::path(m_ProjectSettingPath) / L"ModelSettings.json";
	if (!filesystem::exists(workbookPath))
	{
		if (filesystem::exists(jsonPath))
		{
			LOG_WARN(L"ModelSettings.xlsx is missing; using the existing ModelSettings.json cache");
			return S_FALSE;
		}
		LOG_ERROR(L"ModelSettings.xlsx and ModelSettings.json are both missing : {}", m_ProjectSettingPath);
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	nlohmann::json jsonRoot;
	if (FAILED(ParseModelSettingsWorkbook(workbookPath, filesystem::path(m_ResourcePath), jsonRoot)))
	{
		LOG_ERROR(L"Failed to synchronize ModelSettings.xlsx : {}", workbookPath.wstring());
		return E_FAIL;
	}

	for (const auto& item : jsonRoot["ModelSettings"])
	{
		const string animationPresetGuid = item.value("animationPresetGuid", string{});
		if (animationPresetGuid.empty())
			continue;

		vector<wstring> animationPaths;
		if (FAILED(FindAnimationPresetPaths(m_ProjectSettingPath, m_ResourcePath,
			animationPresetGuid, animationPaths)))
		{
			LOG_ERROR(L"Invalid AnimationPreset reference before ModelSettings.json replacement : {}",
				Helper::To_wString(animationPresetGuid));
			return E_FAIL;
		}
	}

	const HRESULT writeResult = WriteJsonAtomically(jsonPath, jsonRoot);
	if (FAILED(writeResult))
	{
		LOG_ERROR(L"Failed to replace ModelSettings.json from workbook : {}", jsonPath.wstring());
		return writeResult;
	}

	LOG_INFO(L"Synchronized {} Model settings from {}",
		jsonRoot["ModelSettings"].size(), workbookPath.wstring());
	return S_OK;
}

HRESULT ClientSettingManager::Load_EngineDesc(ENGINE_DESC& outDesc) const 
{
	std::wstring path = m_ProjectSettingPath + L"EngineDesc.json";

	if (!filesystem::exists(path)) {
		LOG_WARN(L"Failed To Find Path : {}",path);
		return E_FAIL;
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		return E_FAIL;
	}

	nlohmann::json json;
	file >> json;
	file.close();

	outDesc.levelCount = ETOI(LEVEL::LEVEL_END);
	outDesc.startLevel = json.value("startLevel", ETOI(LEVEL::TITLE));
	outDesc.viewportWidth = json.value("viewportWidth", 1920);
	outDesc.viewportHeight = json.value("viewportHeight", 1080);
	outDesc.windowTitle = Helper::To_wString(json.value("windowTitle", "NieRAutomata"));

	g_EngineDesc = outDesc;

	return S_OK;
}

HRESULT ClientSettingManager::Apply_LayerAndTagSettings() const
{
	auto layerRegistry = GAME_INSTANCE->Get_LayerRegister();
	auto tagRegistry = GAME_INSTANCE->Get_TagRegister();

	if (layerRegistry) {
	  layerRegistry->LoadFromFile(m_ProjectSettingPath + L"LayerSettings.json");
	}

	if (tagRegistry) {
	  tagRegistry->LoadFromFile(m_ProjectSettingPath + L"TagSettings.json");
	}

	return S_OK;
}

HRESULT ClientSettingManager::Ready_Client_Prototypes(LEVEL baseLevel) const
{
    return GAME_INSTANCE->Register_ReflectedPrototypes(ETOI(baseLevel));
}

HRESULT ClientSettingManager::Load_Shader() const
{
	if (!filesystem::exists(m_ShaderPath))
	{
		LOG_WARN(L"Failed To Find Shader Folder : {}", m_ShaderPath);
		return E_FAIL;
	}

	for (const auto& entry : filesystem::recursive_directory_iterator(m_ShaderPath))
	{
		if (entry.is_regular_file())
		{
			std::wstring filePath = entry.path().wstring();
			auto CaseInsensitiveCompare = [](tChar a, tChar b)
				{
					return ::towlower(a) == ::towlower(b);
				};

			if (entry.path().extension() == L".hlsl")
			{
				std::replace(filePath.begin(), filePath.end(), L'\\', L'/');
				std::wstring tagName = entry.path().stem().wstring() + entry.path().extension().wstring();


				std::wstring tex = L"vtxtex";
				std::wstring normTex = L"vtxnormtex";
				std::wstring staticMesh = L"vtxmesh";
				std::wstring animMesh = L"vtxanimmesh";
				std::wstring cube = L"vtxcube";
				std::wstring sphere = L"vtxsphere";

				std::wstring particlePoint = L"vtxparticlepoint";
				std::wstring particleRect = L"vtxparticlerect";
				
				auto itTex = std::search(
						tagName.begin(), tagName.end(), 
						tex.begin(), tex.end(), CaseInsensitiveCompare
					);
				auto itNormTex = std::search(
						tagName.begin(), tagName.end(), 
						normTex.begin(), normTex.end(), CaseInsensitiveCompare
					);
				auto itMeshTex = std::search(
						tagName.begin(), tagName.end(),
						staticMesh.begin(), staticMesh.end(), CaseInsensitiveCompare
					);
				auto itAnimTex = std::search(
						tagName.begin(), tagName.end(),
						animMesh.begin(), animMesh.end(), CaseInsensitiveCompare
					);
				auto itCubeTex = std::search(
						tagName.begin(), tagName.end(),
						cube.begin(), cube.end(), CaseInsensitiveCompare
					);
				auto itSphereTex = std::search(
					tagName.begin(), tagName.end(),
					sphere.begin(), sphere.end(), CaseInsensitiveCompare
				);
				auto itParticlePoint = std::search(
					tagName.begin(), tagName.end(),
					particlePoint.begin(), particlePoint.end(), CaseInsensitiveCompare
				);
				auto itParticleRect = std::search(
					tagName.begin(), tagName.end(),
					particleRect.begin(), particleRect.end(), CaseInsensitiveCompare
				);

				if (itAnimTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXANIMMESH::Elements, VTXANIMMESH::numElements, VTXANIMMESH::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXANIMMESH::Tag);
				}
				else if (itMeshTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXMESH::Elements, VTXMESH::numElements, VTXMESH::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXMESH::Tag);
				}
				else if (itNormTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXNORMTEX::Elements, VTXNORMTEX::numElements, VTXNORMTEX::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXNORMTEX::Tag);
				}
				else if (itTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXTEX::Elements, VTXTEX::numElements, VTXTEX::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXTEX::Tag);
				}
				else if (itCubeTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXCUBE::Elements, VTXCUBE::numElements, VTXCUBE::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXCUBE::Tag);
				}
				else if (itSphereTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXSPHERE::Elements, VTXSPHERE::numElements, VTXSPHERE::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXSPHERE::Tag);
				}
				else if (itParticlePoint != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXPARTICLE_POINT_DESC::Elements, VTXPARTICLE_POINT_DESC::numElements, VTXPARTICLE_POINT_DESC::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXPARTICLE_POINT_DESC::Tag);
				}
				else if (itParticleRect != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXPARTICLE_RECT_DESC::Elements, VTXPARTICLE_RECT_DESC::numElements, VTXPARTICLE_RECT_DESC::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXPARTICLE_RECT_DESC::Tag);
				}
				else
				{
					LOG_WARN(L"Shader File {} Does Not Follow Naming Convention", entry.path().filename().wstring());
				}
			}
		}
	}
	
	return S_OK;
}

HRESULT ClientSettingManager::Load_LevelData(LEVEL level) const
{
	wstring fullPath = m_ProjectSettingPath + L"Scene/LevelData_" + std::to_wstring(ETOI(level)) + L".json";

	if (filesystem::exists(fullPath))
	{
		return GAME_INSTANCE->DeSerializeLevel(fullPath);
	}

	return S_FALSE;
}

HRESULT ClientSettingManager::Load_Sound_FromJson() const
{
	wstring fullPath = m_ProjectSettingPath + L"SoundSettings.json";

	if (!filesystem::exists(fullPath))
	{
		nlohmann::json defaultJson;
		defaultJson["SoundSettings"] = nlohmann::json::array();

		std::ofstream outFile(fullPath);
		if (outFile.is_open())
		{
			outFile << defaultJson.dump(4);
			outFile.close();
		}
		LOG_INFO(L"Created Default SoundSettings.json: {}", fullPath);
	}

	if (!filesystem::exists(fullPath))
	{
		LOG_WARN(L"Failed To Find Sound Settings : {}", fullPath);
		return S_OK;
	}

	ifstream file(fullPath);
	if (!file.is_open()) return E_FAIL;

	nlohmann::json json;
	file >> json;
	file.close();

	auto& settings = json["SoundSettings"];
	for (auto& item : settings)
	{
		auto CleanString = [](string str)
		{
			string trimmed = Helper::Trim(str);
			if (trimmed.size() >= 2 && trimmed.front() == '\"' && trimmed.back() == '\"')
				trimmed = trimmed.substr(1, trimmed.size() - 2);
			return Helper::Trim(trimmed);
		};

		wstring tag = Helper::To_wString(CleanString(item["tag"].get<string>()));
		wstring relativePath = Helper::To_wString(CleanString(item["path"].get<string>()));
		wstring fullSoundPath = m_ResourcePath + relativePath;

		if (FAILED(GAME_INSTANCE->Load_Sound(tag, fullSoundPath)))
		{
			LOG_ERROR(L"Failed to Load Sound: {}", tag);
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT ClientSettingManager::Sync_SoundJson_FromCSV() const
{
	wstring csvPath = m_ResourcePath + L"SoundSettings.csv";

	if (!filesystem::exists(csvPath))
	{
		std::ofstream outFile(csvPath);
		if (outFile.is_open())
		{
			outFile << "Tag, Path" << std::endl;
			outFile << "Title_BGM, Sound/Title_BGM.wem" << std::endl;
			outFile.close();
		}
		LOG_INFO(L"Created Template SoundSettings.csv: {}", csvPath);
	}

	if (!filesystem::exists(csvPath)) return S_OK;

	ifstream csvFile(csvPath);
	nlohmann::json jsonRoot;
	string line;

	getline(csvFile, line);
	while (getline(csvFile, line))
	{
		if (line.empty()) continue;

		stringstream stream(line);
		string tag, path;
		getline(stream, tag, ',');
		getline(stream, path);

		auto CleanString = [](string str)
		{
			string trimmed = Helper::Trim(str);
			trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\"'), trimmed.end());
			trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\r'), trimmed.end());
			return Helper::Trim(trimmed);
		};

		jsonRoot["SoundSettings"].push_back({
			{"tag", CleanString(tag)},
			{"path", CleanString(path)},
		});
	}

	ofstream jsonFile(m_ProjectSettingPath + L"SoundSettings.json");
	jsonFile << jsonRoot.dump(4);

	return S_OK;
}

HRESULT ClientSettingManager::Load_Navigation_FromBinary() const
{
	wstring navDataDir = m_ProjectSettingPath + L"NavData/";
	if (!std::filesystem::exists(navDataDir))
	{
		LOG_WARN(L"NavData directory not found: {}", navDataDir);
		return S_OK;
	}

	for (const auto& entry : std::filesystem::directory_iterator(navDataDir))
	{
		if (entry.is_regular_file() && entry.path().extension() == L".nnav")
		{
			std::wstring filename = entry.path().stem().wstring();
			std::string filePathStr = Helper::To_String(entry.path().wstring());
			
			// Create Navigation component
			auto pNav = Engine::Navigation::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());
			if (!pNav) {
				LOG_ERROR(L"Failed to create Navigation component prototype for: {}", filename);
				continue;
			}

			if (FAILED(pNav->Load_FromBinary(filePathStr)))
			{
				LOG_ERROR(L"Failed to load Navigation binary: {}", filename);
				continue;
			}

			// Add to Engine PrototypeManager (Level STATIC)
			if (FAILED(GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC), pNav, filename)))
			{
				LOG_ERROR(L"Failed to register Navigation prototype: {}", filename);
			}
			else
			{
				LOG_INFO(L"Auto-Registered Navigation Prototype: {}", filename);
			}
		}
	}

	return S_OK;
}

Bool ClientSettingManager::AutoTransitionLevel(LEVEL curLevel, LEVEL nextLev)
{
	if (curLevel == LEVEL::LOADING)
		return ETOI(nextLev) != 1;

	return false;
}

