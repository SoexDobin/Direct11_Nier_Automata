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
#include "GameObject.h"
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
		else if (levelStr == "2" || levelStr == "Logo" || levelStr == "LOGO")
			level = LEVEL::LOGO;
		else if (levelStr == "3" || levelStr == "GamePlay" || levelStr == "GAMEPLAY")
			level = LEVEL::GAMEPLAY;
		
        if (level != baseLevel)
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
		else if (levelStr == "2" || levelStr == "Logo" || levelStr == "LOGO")
			level = LEVEL::LOGO;
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

		stringstream stream(line);
		string level, tag, path;
		string px, py, pz, rx, ry, rz, sx, sy, sz;

		getline(stream, level, ',');
		getline(stream, tag, ',');
		getline(stream, path, ',');
		getline(stream, px, ','); getline(stream, py, ','); getline(stream, pz, ','); // Position
		getline(stream, rx, ','); getline(stream, ry, ','); getline(stream, rz, ','); // Rotation (Euler)
		getline(stream, sx, ','); getline(stream, sy, ','); getline(stream, sz, ','); // Scale

		// 빈 문자열 및 기본값 처리 람다 (Sync 시 적용)
		auto GetFloat = [](string str, float defaultValue) {
			string trimmed = Helper::Trim(str);
			// 따옴표 제거 (혹시 포함될 경우)
			trimmed.erase(remove(trimmed.begin(), trimmed.end(), '\"'), trimmed.end());
			if (trimmed.empty()) return defaultValue;
			try { return stof(trimmed); } catch (...) { return defaultValue; }
		};

		auto CleanString = [](string str) {
			string trimmed = Helper::Trim(str);
			// 앞뒤 따옴표 제거
			if (trimmed.size() >= 2 && trimmed.front() == '\"' && trimmed.back() == '\"')
				trimmed = trimmed.substr(1, trimmed.size() - 2);
			return Helper::Trim(trimmed); // 따옴표 제거 후 한 번 더 트림
		};

		string cleanLevel = CleanString(level);
		string cleanTag = CleanString(tag);
		string cleanPath = CleanString(path);

		float fPx = GetFloat(px, 0.f); float fPy = GetFloat(py, 0.f); float fPz = GetFloat(pz, 0.f);
		float fRx = GetFloat(rx, 0.f); float fRy = GetFloat(ry, 0.f); float fRz = GetFloat(rz, 0.f);
		float fSx = GetFloat(sx, 1.f); float fSy = GetFloat(sy, 1.f); float fSz = GetFloat(sz, 1.f);

		jsonRoot["ModelSettings"].push_back({
			{"level", cleanLevel},
			{"tag", cleanTag},
			{"path", cleanPath},
			{"position", {{"x", fPx}, {"y", fPy}, {"z", fPz}}},
			{"rotation", {{"x", fRx}, {"y", fRy}, {"z", fRz}}},
			{"scale", {{"x", fSx}, {"y", fSy}, {"z", fSz}}}
			});
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

HRESULT ClientSettingManager::Ready_Client_Prototypes(LEVEL baseLevel) const
{
    rttr::type gameObjectType = rttr::type::get<GameObject>();
    rttr::type componentType = rttr::type::get<Component>();
    auto allTypes = rttr::type::get_types();

    // 전역/정적 세트를 사용하여 여러 번 호출(STATIC, LOGO 등)되더라도 
    // 동일 타입의 프로토타입은 전 생명주기 동안 단 한 번만 등록되도록 보장합니다.
    static unordered_set<string> globalProcessedTypes;

    for (auto& type : allTypes)
    {
        // 1. GameObject 또는 Component를 상속받은(derived_from) 자기자신 제외 클래스만 순회
        bool isGameObject = type.is_derived_from(gameObjectType) && type != gameObjectType;
        bool isComponent = type.is_derived_from(componentType) && type != componentType;

        if ((isGameObject || isComponent) && !type.is_pointer())
        {
            // 중복 처리 방지: 동일한 타입 이름이 이미 등록되었다면 건너뜀
            string typeName = type.get_name().to_string();
            if (globalProcessedTypes.contains(typeName))
                continue;

            // 2. 파이썬이 통일해준 Create 메서드 가져오기
            rttr::method createMethod = type.get_method("Create");
            if (createMethod.is_valid())
            {
                // 3. 메타데이터 파싱 (기본값: STATIC 0번 레벨)
                uint32 targetLevel = 0; 
                rttr::variant metaLevel = createMethod.get_metadata("Level");
                if (metaLevel.is_valid())
                {
                    if (metaLevel.is_type<uint32>())
                        targetLevel = metaLevel.get_value<uint32>();
                    else if (metaLevel.is_type<int>()) 
                        targetLevel = static_cast<uint32>(metaLevel.get_value<int>());
                    else if (metaLevel.is_type<int32>()) 
                        targetLevel = static_cast<uint32>(metaLevel.get_value<int32>());
                    else if (metaLevel.is_type<LEVEL>()) 
                        targetLevel = ETOI(metaLevel.get_value<LEVEL>());
                }
                
                // 0번(STATIC)이더라도 현재 baseLevel과 일치할 때만 진행 (중복 등록 방지)
                if (targetLevel != ETOI(baseLevel))
                    continue;

                // 해당 레벨에 등록하기로 결정된 타입만 마킹 (실제 등록 직전)
                globalProcessedTypes.insert(typeName);

                LOG_INFO(L"[RTTR-Debug] Try Reg: {}, Target: {}, Current: {}", Helper::To_wString(typeName), targetLevel, ETOI(baseLevel));

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
						staticMesh.begin(), staticMesh.end(),
						CaseInsensitiveCompare
					);
				auto itAnimTex =
					std::search(
						tagName.begin(), tagName.end(),
						animMesh.begin(), animMesh.end(),
						CaseInsensitiveCompare
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
				else
				{
					LOG_WARN(L"Shader File {} Does Not Follow Naming Convention", entry.path().filename().wstring());
				}
			}
		}
	}
	
	return S_OK;
}
