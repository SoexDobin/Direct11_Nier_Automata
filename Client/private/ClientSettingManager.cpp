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
#include <regex>

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

		string levelStr = item["level"];
		LEVEL level = LEVEL::STATIC;
		if (levelStr == "1" || levelStr == "Loading" || levelStr == "LOADING")
			level = LEVEL::LOADING;
		else if (levelStr == "2" || levelStr == "Title" || levelStr == "TITLE")
			level = LEVEL::TITLE;
		else if (levelStr == "3" || levelStr == "GamePlay" || levelStr == "GAMEPLAY")
			level = LEVEL::GAMEPLAY;
		
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

HRESULT ClientSettingManager::Sync_TextureJson_FromCSV() const
{
	auto ParseCSVRow = [](const string& line) -> vector<string>
		{
			vector<string> result;
			bool inQuotes = false;
			string currentToken;
			for (char c : line)
			{
				if (c == '\"') {
					inQuotes = !inQuotes;
				}
				else if (c == ',' && !inQuotes) {
					result.push_back(currentToken);
					currentToken.clear();
				}
				else {
					currentToken += c;
				}
			}
			result.push_back(currentToken);
			return result;
		};

	wstring csvPath = m_ResourcePath + L"TextureSettings.csv";

	// CSV가 없을 경우 템플릿 생성
	if (!filesystem::exists(csvPath))
	{
		std::ofstream outFile(csvPath);
		if (outFile.is_open()) {
			outFile << "Level, Tag, Path, Count" << std::endl;
			outFile << "STATIC, Prototype_Component_Texture_Default, Default/Default.dds, 1" << std::endl;
			outFile.close();
		}
		LOG_INFO(L"Created Template TextureSettings.csv: {}", csvPath);
	}

	if (!filesystem::exists(csvPath)) return S_OK;

	ifstream csvFile(csvPath);
	nlohmann::json jsonRoot;
	string line;

	getline(csvFile, line);
	while (getline(csvFile, line))
	{
		if (line.empty()) continue;
		
		vector<string> columns = ParseCSVRow(line);
		if (columns.size() < 4) continue; // 데이터 열 부족 시 스킵
		string levelStr = columns[0];
		string tag = columns[1];
		string path = columns[2];
		string count = columns[3];
		
		stringstream levelStream(levelStr);
		string singleLevel;
		// 3. 쉼표(,)로 레벨을 쪼갠 뒤 각각 JSON에 push_back
		while (getline(levelStream, singleLevel, ','))
		{
			jsonRoot["TextureSettings"].push_back({
				{"level", Helper::Trim(singleLevel)},
				{"tag", Helper::Trim(tag)},
				{"path", Helper::Trim(path)},
				{"count", stoi(Helper::Trim(count))}
				});
		}
	}

	ofstream jsonFile(m_ProjectSettingPath + L"TextureSettings.json");
	jsonFile << jsonRoot.dump(4);

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
	}

	return S_OK;
}

HRESULT ClientSettingManager::Sync_ModelJson_FromCSV() const
{
	auto ParseCSVRow = [](const string& line) -> vector<string>
		{
			vector<string> result;
			bool inQuotes = false;
			string currentToken;
			for (char c : line)
			{
				if (c == '\"') {
					inQuotes = !inQuotes;
				}
				else if (c == ',' && !inQuotes) {
					result.push_back(currentToken);
					currentToken.clear();
				}
				else {
					currentToken += c;
				}
			}
			result.push_back(currentToken);
			return result;
		};

	wstring csvPath = m_ResourcePath + L"ModelSettings.csv";

	if (!filesystem::exists(csvPath))
	{
		ofstream outFile(csvPath);
		if (outFile.is_open())
		{
			outFile << "Level, Tag, Path, PosX, PosY, PosZ, RotX, RotY, RotZ, ScaleX, ScaleY, ScaleZ" << std::endl;
			outFile << "STATIC, Prototype_Model_Default, Models/Default.Model, 0, 0, 0, 0, 0, 0, 1, 1, 1" << std::endl;
			outFile.close();
		}
		LOG_INFO(L"Created Template ModelSettings.csv: {}", csvPath);
	}

	if (!filesystem::exists(csvPath)) return S_OK;

	ifstream csvFile(csvPath);
	nlohmann::json jsonRoot;
	string line;


	getline(csvFile, line);
	while (getline(csvFile, line))
	{
		if (line.empty()) continue;
		// 1. 안전 파서 사용
		vector<string> columns = ParseCSVRow(line);
		if (columns.size() < 12) continue;
		string levelStr = columns[0];
		string tag = columns[1];
		string path = columns[2];
		string px = columns[3], py = columns[4], pz = columns[5];
		string rx = columns[6], ry = columns[7], rz = columns[8];
		string sx = columns[9], sy = columns[10], sz = columns[11];
	
		auto GetFloat = [](string str, Float defaultValue)
			{
				string trimmed = Helper::Trim(str);
				if (trimmed.empty()) return defaultValue;
				try { return stof(trimmed); }
				catch (...) { return defaultValue; }
			};
		Float fPx = GetFloat(px, 0.f); Float fPy = GetFloat(py, 0.f); Float fPz = GetFloat(pz, 0.f);
		Float fRx = GetFloat(rx, 0.f); Float fRy = GetFloat(ry, 0.f); Float fRz = GetFloat(rz, 0.f);
		Float fSx = GetFloat(sx, 1.f); Float fSy = GetFloat(sy, 1.f); Float fSz = GetFloat(sz, 1.f);
		// 2. 레벨 분리 및 각각 푸쉬
		stringstream levelStream(levelStr);
		string singleLevel;
		while (getline(levelStream, singleLevel, ','))
		{
			jsonRoot["ModelSettings"].push_back({
				{"level", Helper::Trim(singleLevel)},
				{"tag", Helper::Trim(tag)},
				{"path", Helper::Trim(path)},
				{"position", {{"x", fPx}, {"y", fPy}, {"z", fPz}}},
				{"rotation", {{"x", fRx}, {"y", fRy}, {"z", fRz}}},
				{"scale", {{"x", fSx}, {"y", fSy}, {"z", fSz}}}
				});
		}
	}

	ofstream jsonFile(m_ProjectSettingPath + L"ModelSettings.json");
	jsonFile << jsonRoot.dump(4);
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
    rttr::type gameObjectType = rttr::type::get<GameObject>();
    rttr::type componentType = rttr::type::get<Component>();
    auto allTypes = rttr::type::get_types();

    for (auto& type : allTypes)
    {
        // 1. GameObject 또는 Component를 상속받은(derived_from) 자기자신 제외 클래스만 순회
        bool isGameObject = type.is_derived_from(gameObjectType) && type != gameObjectType;
        bool isComponent = type.is_derived_from(componentType) && type != componentType;

        if ((isGameObject || isComponent) && !type.is_pointer())
        {
            // 중복 처리 방지 로직은 엔진의 Add_Prototype 단계에 맡깁니다.
            string typeName = type.get_name().to_string();

            // 2. 파이썬이 통일해준 Create 메서드 가져오기
            rttr::method createMethod = type.get_method("Create");
            if (createMethod.is_valid())
            {
                // 3. 메타데이터 파싱 (기본값: STATIC 0번 레벨)
				std::vector<uint32> targetLevels;
				rttr::variant metaLevel = createMethod.get_metadata("Level");
				if (metaLevel.is_valid())
				{
					// [단일 데이터] 파싱 (기존 호환성)
					if (metaLevel.is_type<uint32>())
					{
						targetLevels.push_back(metaLevel.get_value<uint32>());
					}
					else if (metaLevel.is_type<int>())
					{
						targetLevels.push_back(static_cast<uint32>(metaLevel.get_value<int>()));
					}
					else if (metaLevel.is_type<int32>())
					{
						targetLevels.push_back(static_cast<uint32>(metaLevel.get_value<int32>()));
					}
					else if (metaLevel.is_type<LEVEL>())
					{
						targetLevels.push_back(ETOI(metaLevel.get_value<LEVEL>()));
					}
					// [복수 데이터 (배열/벡터)] 파싱
					else if (metaLevel.is_type<std::vector<uint32>>())
					{
						targetLevels = metaLevel.get_value<std::vector<uint32>>();
					}
					else if (metaLevel.is_type<std::vector<int>>())
					{
						auto arr = metaLevel.get_value<std::vector<int>>();
						for (int lv : arr)
						{
							targetLevels.push_back(static_cast<uint32>(lv));
						}
					}
					else if (metaLevel.is_type<std::vector<LEVEL>>())
					{
						auto arr = metaLevel.get_value<std::vector<LEVEL>>();
						for (LEVEL lv : arr)
						{
							targetLevels.push_back(ETOI(lv));
						}
					}
				}
				else
				{
					// 메타데이터가 아예 없으면 기본값(STATIC : 0) 취급
					targetLevels.push_back(0);
				}
				// 현재 등록 시점인 baseLevel이 등록 허가 리스트(targetLevels)에 포함되어 있는지 판별
				bool bMatched = false;
				for (uint32 lv : targetLevels)
				{
					if (lv == ETOI(baseLevel))
					{
						bMatched = true;
						break;
					}
				}
				// 허가된 레벨이 아니라면 생성 및 PrototypeManager 등록 건너뛰기
				if (!bMatched)
				{
					continue;
				}


                //LOG_INFO(L"[RTTR-Debug] Try Reg: {}, Target: {}, Current: {}", Helper::To_wString(typeName), targetLevel, ETOI(baseLevel));

                // 4. Create Invoke (매개변수: Device, Context)
                rttr::variant result = createMethod.invoke({}, GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());
                
                // 5. 엔진 PrototypeManager에 삽입
                if (result.is_valid())
                {
                    Shared<Object> prototype = nullptr;
                    if (isGameObject)
                    {
                        prototype = result.get_value<Shared<GameObject>>();
                    }
                    else if (isComponent)
                    {
                        prototype = result.get_value<Shared<Component>>();
                    }
                    
                    if (prototype == nullptr)
                    {
                        LOG_ERROR(L"Failed to cast prototype to GameObject or Component: {}", Helper::To_wString(typeName));
                        continue;
                    }

                    // 태그는 RTTR 클래스 이름(std::string)을 wstring으로 변환하여 사용
                    wstring tag = Helper::To_wString(type.get_name().to_string());
                    
                    // 현재 준비 중인 레벨(baseLevel)에 등록
                    if (SUCCEEDED(GAME_INSTANCE->Add_Prototype(ETOI(baseLevel), prototype, tag)))
                    {
                        LOG_INFO(L"Auto-Registered Client Prototype: {} (Level: {})", tag, ETOI(baseLevel));
                    }
                    else
                    {
                        LOG_ERROR(L"Failed To Register Client Prototype: {}", tag);
                    }
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
				std::wstring staticWorldMesh = L"vtxworldmesh";
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
				auto itWorldMeshTex = std::search(
					tagName.begin(), tagName.end(),
					staticWorldMesh.begin(), staticWorldMesh.end(), CaseInsensitiveCompare
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
				else if (itWorldMeshTex != tagName.end())
				{
					if (FAILED(GAME_INSTANCE->Load_Shader(ETOI(LEVEL::STATIC), (m_ShaderPath + tagName).c_str(), VTXMESH::Elements, VTXMESH::numElements, VTXWORLDMESH)))
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
	wstring fullPath = m_ProjectSettingPath + L"LevelData_" + std::to_wstring(ETOI(level)) + L".json";

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

