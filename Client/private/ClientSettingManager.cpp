#include "pch.h"
#include "ClientSettingManager.h"

#include <fstream>
#include <SpdLogger.h>
#include "Client_Define.h"
#include "Game.h"
#include "TagRegistry.h"
#include "LayerRegistry.h"
#include "Texture.h"
#include "Shader.h"
#include <regex>

IMPLEMENT_SINGLETON(ClientSettingManager)

ENGINE_DESC ClientSettingManager::g_EngineDesc{};

HRESULT ClientSettingManager::Load_Textures_FromJson() const
{
	wstring fullPath = m_ProjectSettingPath + L"TextureSettings.json";

	if (!filesystem::exists(fullPath))
	{
		nlohmann::json defaultJson;
		defaultJson["TextureSettings"] = nlohmann::json::array();
		// 초기 템플릿 예시 추가 가능
		std::ofstream outFile(fullPath);
		if (outFile.is_open()) {
			outFile << defaultJson.dump(4);
			outFile.close();
		}
		SpdLogger::Info(L"Created Default TextureSettings.json: " + fullPath);
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

		string levelStr = item["level"];
		LEVEL level = LEVEL::STATIC;
		if (levelStr == "1" || levelStr == "Loading" || levelStr == "LOADING")
			level = LEVEL::LOADING;
		else if (levelStr == "2" || levelStr == "Logo" || levelStr == "LOGO")
			level = LEVEL::LOGO;
		else if (levelStr == "3" || levelStr == "GamePlay" || levelStr == "GAMEPLAY")
			level = LEVEL::GAMEPLAY;
		
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

HRESULT ClientSettingManager::Sync_TextureJson_FromCSV() const
{
	wstring csvPath = m_ResourcePath + L"TextureSettings.csv";

	// CSV가 없을 경우 템플릿 생성
	if (!filesystem::exists(csvPath))
	{
		std::ofstream outFile(csvPath);
		if (outFile.is_open()) {
			// 헤더 및 템플릿 행 작성
			outFile << "Level, Tag, Path, Count" << std::endl;
			outFile << "STATIC, Prototype_Component_Texture_Default, Default/Default.dds, 1" << std::endl;
			outFile.close();
		}
		SpdLogger::Info(L"Created Template TextureSettings.csv: " + csvPath);
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
		string level, tag, path, count;
		getline(stream, level, ',');
		getline(stream, tag, ',');
		getline(stream, path, ',');
		getline(stream, count);

		jsonRoot["TextureSettings"].push_back({
			{"level", Helper::Trim(level)},
			{"tag", Helper::Trim(tag)},
			{"path", Helper::Trim(path)},
			{"count", stoi(Helper::Trim(count))},
			});
	}

	ofstream jsonFile(m_ProjectSettingPath + L"TextureSettings.json");
	jsonFile << jsonRoot.dump(4);

	return S_OK;
}

HRESULT ClientSettingManager::Load_EngineDesc(ENGINE_DESC& outDesc) const 
{
	std::wstring path = m_ProjectSettingPath + L"EngineDesc.json";

	if (!filesystem::exists(path)) {
		SpdLogger::Warn(L"Failed To Find Path " + path);
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
	outDesc.startLevel = json.value("startLevel", ETOI(LEVEL::LOGO));
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

HRESULT ClientSettingManager::Load_Texture(LEVEL level) const
{
	//if (!filesystem::exists(m_ResourcePath))
	//{
	//	SpdLogger::Warn(L"Failed To Find Resource Folder " + m_ResourcePath);
	//	return E_FAIL; 
	//}
	//
	//std::wregex levelRegEx(L"lev(\\d+)_.*", std::regex_constants::icase);
	//std::wregex fileRegEx(L"(.+?)(_*)(\\d+)\\.(dds|png|tga|jpg)", std::regex_constants::icase);
	//for (const auto& entry : filesystem::recursive_directory_iterator(m_ResourcePath))
	//{
	//	if (entry.is_regular_file()) // 파일일 경우에만 처리
	//	{
	//		std::wstring FilePath = entry.path().wstring();
	//		std::replace(FilePath.begin(), FilePath.end(), L'\\', L'/');
	//		
	//		uint32 levIndex = { 0 };
	//		std::wsmatch levelMatch;
	//		if (std::regex_search(FilePath, levelMatch, levelRegEx))
	//		{
	//			levIndex = std::stoul(levelMatch[1].str());
	//		}
	//		
	//		std::wsmatch fileMatch;
	//		std::wstring entryFileName = entry.path().filename().wstring();
	//		if (std::regex_match(entryFileName, fileMatch, fileRegEx))
	//		{
	//			std::wstring baseName = fileMatch[1].str();
	//			std::wstring separator = fileMatch[2].str();
	//			std::wstring extension = fileMatch[4].str();
	//
	//			// 상위 폴더 경로까지 합쳐서 L"../bin/resources/lev0_static/Textures/p10000_%d.dds" 포맷 만들기
	//			std::wstring parentPath = entry.path().parent_path().wstring();
	//			std::replace(parentPath.begin(), parentPath.end(), L'\\', L'/');
	//			std::wstring formatPath = parentPath + L"/" + baseName + separator + L"%d." + extension;
	//
	//
	//			std::wstring tagName = baseName;
	//			uint32 sequenceCount = 0;
	//			for (const auto& peerEntry : filesystem::directory_iterator(entry.path().parent_path()))
	//			{
	//				std::wsmatch peerMatch;
	//				std::wstring peerFileName = peerEntry.path().filename().wstring();
	//				if (std::regex_match(peerFileName, peerMatch, fileRegEx))
	//				{
	//					// 같은 시퀀스(BaseName 일치)라면 카운트 증가
	//					if (peerMatch[1].str() == baseName)
	//					{
	//						sequenceCount++;
	//					}
	//				}
	//			}
	//
	//			GAME_INSTANCE->Add_Prototype(levIndex,
	//				Texture::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),
	//					formatPath.c_str(), sequenceCount)
	//			);
	//		}
	//		else
	//		{
	//			std::wstring formatPath = FilePath;
	//			std::wstring tagName = entry.path().stem().wstring();
	//			GAME_INSTANCE->Add_Prototype(levIndex,
	//				Texture::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),
	//					formatPath.c_str(), 1)
	//			);
	//		}
	//	}
	//}

	return S_OK;
}

HRESULT ClientSettingManager::Load_Shader() const
{
	if (!filesystem::exists(m_ShaderPath))
	{
		SpdLogger::Warn(L"Failed To Find Shader Folder: " + m_ShaderPath);
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
				std::wstring meshTex = L"vtxmesh";
				
				auto itTex = 
					std::search(
						tagName.begin(), tagName.end(), 
						tex.begin(), tex.end(),
						CaseInsensitiveCompare
					);
				auto itNormTex =
					std::search(
						tagName.begin(), tagName.end(), 
						normTex.begin(), normTex.end(),
						CaseInsensitiveCompare
					);
				auto itMeshTex =
					std::search(
						tagName.begin(), tagName.end(),
						meshTex.begin(), meshTex.end(),
						CaseInsensitiveCompare
					);

				if (itTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXTEX::Elements, VTXTEX::numElements, VTXTEX::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXTEX::Tag);
				}
				else if (itNormTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXNORMTEX::Elements, VTXNORMTEX::numElements, VTXNORMTEX::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXNORMTEX::Tag);
				}
				else if (itMeshTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXMESH::Elements, VTXMESH::numElements, VTXMESH::Tag)))
						LOG_ERROR(L"Failed to Load Shader {}", VTXMESH::Tag);
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
