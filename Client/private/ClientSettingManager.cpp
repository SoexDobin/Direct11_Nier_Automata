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

HRESULT ClientSettingManager::Load_EngineDesc() const 
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

	g_projectSettings.levCount = json.value("levCount", ETOI(LEVEL::LOGO));
	g_projectSettings.viewportWidth = json.value("viewportWidth", 1920);
	g_projectSettings.viewportHeight = json.value("viewportHeight", 1080);
	g_projectSettings.windowTitle = Helper::To_wString(json.value("windowTitle", "NieRAutomata"));

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
	if (!filesystem::exists(m_ResourcePath))
	{
		SpdLogger::Warn(L"Failed To Find Resource Folder " + m_ResourcePath);
		return E_FAIL; 
	}
	
	std::wregex levelRegEx(L"lev(\\d+)_.*", std::regex_constants::icase);
	std::wregex fileRegEx(L"(.+?)(_*)(\\d+)\\.(dds|png|tga|jpg)", std::regex_constants::icase);
	for (const auto& entry : filesystem::recursive_directory_iterator(m_ResourcePath))
	{
		if (entry.is_regular_file()) // 파일일 경우에만 처리
		{
			std::wstring FilePath = entry.path().wstring();
			std::replace(FilePath.begin(), FilePath.end(), L'\\', L'/');
			
			uint32 levIndex = { 0 };
			std::wsmatch levelMatch;
			if (std::regex_search(FilePath, levelMatch, levelRegEx))
			{
				levIndex = std::stoul(levelMatch[1].str());
			}
			
			std::wsmatch fileMatch;
			std::wstring entryFileName = entry.path().filename().wstring();
			if (std::regex_match(entryFileName, fileMatch, fileRegEx))
			{
				std::wstring baseName = fileMatch[1].str();
				std::wstring separator = fileMatch[2].str();
				std::wstring extension = fileMatch[4].str();

				// 상위 폴더 경로까지 합쳐서 L"../bin/resources/lev0_static/Textures/p10000_%d.dds" 포맷 만들기
				std::wstring parentPath = entry.path().parent_path().wstring();
				std::replace(parentPath.begin(), parentPath.end(), L'\\', L'/');
				std::wstring formatPath = parentPath + L"/" + baseName + separator + L"%d." + extension;


				std::wstring tagName = baseName;
				uint32 sequenceCount = 0;
				for (const auto& peerEntry : filesystem::directory_iterator(entry.path().parent_path()))
				{
					std::wsmatch peerMatch;
					std::wstring peerFileName = peerEntry.path().filename().wstring();
					if (std::regex_match(peerFileName, peerMatch, fileRegEx))
					{
						// 같은 시퀀스(BaseName 일치)라면 카운트 증가
						if (peerMatch[1].str() == baseName)
						{
							sequenceCount++;
						}
					}
				}

				if (level == LEVEL::STATIC)
				{
					for (uint32 i = 0; i < ETOI(LEVEL::LEVEL_END); ++i)
					{
						GAME_INSTANCE->Add_Prototype(i,
							Texture::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),
								formatPath.c_str(), sequenceCount)
						);
					}
				}
				else
				{
					GAME_INSTANCE->Add_Prototype(levIndex,
						Texture::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),
							formatPath.c_str(), sequenceCount)
					);
				}
			}
			else
			{
				std::wstring formatPath = FilePath;
				std::wstring tagName = entry.path().stem().wstring();
				if (level == LEVEL::STATIC)
				{
					for (uint32 i = 0; i < ETOI(LEVEL::LEVEL_END); ++i)
					{
						GAME_INSTANCE->Add_Prototype(i,
							Texture::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),
								formatPath.c_str(), 1)
						);
					}
				}
				else
				{
					GAME_INSTANCE->Add_Prototype(levIndex,
						Texture::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(), 
							formatPath.c_str(), 1)
					);
				}
			}
		}
	}

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

				if (itTex != tagName.end())
				{
					GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC),
						Shader::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),
							(m_ShaderPath + tagName).c_str(),
							VTXTEX::Elements, VTXTEX::numElements)
					);
				}
				else if (itNormTex != tagName.end())
				{
					GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC),
						Shader::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),
							(m_ShaderPath + tagName).c_str(), 
							VTXNORMTEX::Elemnets, VTXNORMTEX::numElements)
					);
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
