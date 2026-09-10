#include "pch.h"
#include "ClientApp.h"
#include "Client_Function.h"

#include "ClientSettingManager.h"
#include "Game.h"
#include "LevelLoading.h"
#include "LevelGamePlay.h"
#include <mutex>

#include "LoadingFadeIn.h"
#include "LoadingFadeOut.h"
#include "SpdLogger.h"
#include "StaticCamera.h"
#include "SkySphere.h"
#include "GameObject.h"
#include "HpBarWorldUI.h"
#include "Pl0000.h"
#include "TextUI.h"

namespace
{
#ifdef _DEBUG
	constexpr uint32 LauncherClientGateLevel = ETOI(LEVEL::GAMEPLAY);

	Bool Is_LauncherClientGate()
	{
		wchar_t value[2]{};
		return GetEnvironmentVariableW(L"NIER_PHASE6_LAUNCHER_CLIENT_GATE", value,
			static_cast<DWORD>(std::size(value))) != 0;
	}

	Bool Read_Json(const filesystem::path& path, nlohmann::json& outDocument)
	{
		try
		{
			ifstream input(path);
			if (!input.is_open())
				return false;
			input >> outDocument;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	HRESULT Load_LauncherClientGateModels()
	{
		nlohmann::json settings;
		if (!Read_Json(L"../../ProjectSetting/ModelSettings.json", settings) ||
			!settings.contains("ModelSettings") || !settings["ModelSettings"].is_array())
			return E_FAIL;

		const unordered_set<string> requiredTags = {
			"pl0000", "wp0070", "wp0220", "wp3000"
		};
		unordered_set<string> loadedTags;
		for (const nlohmann::json& item : settings["ModelSettings"])
		{
			const string tag = item.value("tag", string{});
			if (!requiredTags.contains(tag))
				continue;
			const auto ReadVector = [&item](const Char* name, const Vector3& fallback) {
				Vector3 value = fallback;
				if (item.contains(name) && item[name].is_object())
				{
					value.x = item[name].value("x", fallback.x);
					value.y = item[name].value("y", fallback.y);
					value.z = item[name].value("z", fallback.z);
				}
				return value;
			};
			const Vector3 position = ReadVector("position", Vector3::Zero);
			const Vector3 rotation = ReadVector("rotation", Vector3::Zero);
			const Vector3 scale = ReadVector("scale", Vector3::One);
			const Matrix preTransform = Matrix::CreateScale(scale) *
				Matrix::CreateFromYawPitchRoll(XMConvertToRadians(rotation.y),
					XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.z)) *
				Matrix::CreateTranslation(position);
			const filesystem::path modelPath = filesystem::path(L"../../Client/bin/resources/") /
				Helper::To_wString(item.value("path", string{}));
			if (FAILED(GAME_INSTANCE->Load_Model(LauncherClientGateLevel, modelPath.c_str(),
				Helper::To_wString(tag), preTransform)))
				return E_FAIL;
			loadedTags.emplace(tag);
		}
		return loadedTags.size() == requiredTags.size() ? S_OK : E_FAIL;
	}

	HRESULT Load_LauncherClientGateTexture()
	{
		const filesystem::path texturePath =
			L"../../Client/bin/resources/Models/pl0000/Textures/0454E8A4.dds";
		if (!filesystem::is_regular_file(texturePath))
			return E_FAIL;
		return GAME_INSTANCE->Load_Texture(LauncherClientGateLevel, texturePath.c_str(), 1,
			L"UI_Hp_Bar");
	}

	HRESULT Run_LauncherClientGate()
	{
		nlohmann::json checks = nlohmann::json::array();
		Bool passed = true;
		const filesystem::path reportPath =
			L"../../.codex-tmp/phase5-gates/phase6-launcher-client-gates.json";
		const auto Save = [&checks, &passed, &reportPath]() {
			nlohmann::json report = {
				{ "schemaVersion", 1 },
				{ "stage", "launcher-client" },
				{ "processId", GetCurrentProcessId() },
				{ "passed", passed },
				{ "checks", checks }
			};
			std::error_code errorCode;
			filesystem::create_directories(reportPath.parent_path(), errorCode);
			ofstream output(reportPath, ios::binary | ios::trunc);
			if (errorCode || !output.is_open())
				return false;
			output << report.dump(4);
			output.flush();
			return output.good();
		};
		const auto Check = [&checks, &passed, &Save](const Char* name, Bool result) {
			checks.push_back({ { "name", name }, { "passed", result }, { "detail", "" } });
			passed = passed && result;
			Save();
		};

		Check("project-prefab-repository-loaded",
			GAME_INSTANCE->Get_PrefabDocuments().size() == 1);
		const auto initialLevel = GAME_INSTANCE->Get_CurrentLevel();
		const uint32 initialLevelIndex = GAME_INSTANCE->Get_CurrentLevelIndex();
		Check("failed-initial-level-is-not-published",
			GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), nullptr) == E_INVALIDARG &&
			GAME_INSTANCE->Get_CurrentLevel() == initialLevel &&
			GAME_INSTANCE->Get_CurrentLevelIndex() == initialLevelIndex);
		const auto& staticGameObjects = GAME_INSTANCE->Get_Prototypes(ETOI(LEVEL::STATIC));
		Check("static-type-prototypes-ready",
			staticGameObjects.contains(L"TextUI") &&
			staticGameObjects.contains(L"Pl0000") &&
			GAME_INSTANCE->Can_InstantiateGameObject(
				"TextUI", LauncherClientGateLevel) &&
			GAME_INSTANCE->Can_InstantiateGameObject(
				"Pl0000", LauncherClientGateLevel));
		const Bool resourcesReady = SUCCEEDED(Load_LauncherClientGateModels()) &&
			SUCCEEDED(Load_LauncherClientGateTexture()) &&
			SUCCEEDED(ClientSettingManager::GetInstance()->Load_Navigation_FromBinary());
		Check("gameplay-gate-resources-ready", resourcesReady);

		// UI/Sprite-only opt-in fallback preserves sequence indices and the authored path.
		const wchar_t* missingTexture = L"../../.codex-tmp/phase5-gates/absent-ui-%d.dds";
		const wstring fallbackTag = L"Phase9B_MissingUI";
		const auto tagsBeforeFallback = GAME_INSTANCE->Get_TextureTags(LauncherClientGateLevel);
		const HRESULT strictResult = GAME_INSTANCE->Load_Texture(
			LauncherClientGateLevel, missingTexture, 2, fallbackTag);
		Check("missing-texture-strict-load-preserves-registry", FAILED(strictResult) &&
			GAME_INSTANCE->Get_TextureTags(LauncherClientGateLevel) == tagsBeforeFallback);
		const HRESULT fallbackResult = GAME_INSTANCE->Load_Texture(
			LauncherClientGateLevel, missingTexture, 2, fallbackTag, true);
		const auto* fallbackDesc = GAME_INSTANCE->Get_TextureDesc(LauncherClientGateLevel, fallbackTag);
		const auto fallbackSrv = GAME_INSTANCE->Get_Texture(LauncherClientGateLevel,
			L"../../.codex-tmp/phase5-gates/absent-ui-0.dds");
		Check("ui-fallback-keeps-path-and-sequence", fallbackResult == S_FALSE &&
			fallbackDesc && fallbackDesc->m_FilePath == missingTexture && fallbackDesc->m_NumSRVs == 2 &&
			fallbackSrv && fallbackSrv == GAME_INSTANCE->Get_Texture(LauncherClientGateLevel,
				L"../../.codex-tmp/phase5-gates/absent-ui-1.dds"));
		Bool transparentFallback = false;
		if (fallbackSrv)
		{
			ComPtr<ID3D11Resource> resource;
			fallbackSrv->GetResource(resource.GetAddressOf());
			ComPtr<ID3D11Texture2D> texture;
			if (SUCCEEDED(resource.As(&texture)))
			{
				D3D11_TEXTURE2D_DESC desc{};
				texture->GetDesc(&desc);
				desc.Usage = D3D11_USAGE_STAGING;
				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				ComPtr<ID3D11Texture2D> staging;
				if (SUCCEEDED(GAME_INSTANCE->Get_Device()->CreateTexture2D(&desc, nullptr, staging.GetAddressOf())))
				{
					const auto context = GAME_INSTANCE->Get_Context();
					context->CopyResource(staging.Get(), texture.Get());
					D3D11_MAPPED_SUBRESOURCE data{};
					if (SUCCEEDED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &data)))
					{
						transparentFallback = desc.Width == 1 && desc.Height == 1 &&
							*static_cast<const uint32*>(data.pData) == 0;
						context->Unmap(staging.Get(), 0);
					}
				}
			}
		}
		Check("ui-fallback-pixel-is-transparent", transparentFallback);
		Check("ui-fallback-reload-is-idempotent", GAME_INSTANCE->Load_Texture(
			LauncherClientGateLevel, missingTexture, 2, fallbackTag, true) == S_OK &&
			GAME_INSTANCE->Load_Texture(LauncherClientGateLevel, missingTexture, 2, fallbackTag) == strictResult);

		Shared<LevelGamePlay> gameplayLevel;
		if (resourcesReady)
			gameplayLevel = LevelGamePlay::Create(GAME_INSTANCE->Get_Device(),
				GAME_INSTANCE->Get_Context());
		const Bool sceneLoaded = gameplayLevel && SUCCEEDED(GAME_INSTANCE->Change_Level(
			LauncherClientGateLevel, gameplayLevel));
		Check("client-gameplay-scene-loaded", sceneLoaded);

		Shared<Pl0000> player;
		Shared<TextUI> uiRoot;
		if (sceneLoaded)
		{
			for (const auto& [instanceId, object] :
				GAME_INSTANCE->Get_GameObjects(LauncherClientGateLevel))
			{
				if (!object || object->Get_Parent())
					continue;
				if (object->Get_Name() == L"Phase6_Pl0000")
					player = dynamic_pointer_cast<Pl0000>(object);
				else if (object->Get_Name() == L"Phase6_UIRoot")
					uiRoot = dynamic_pointer_cast<TextUI>(object);
			}
		}
		const Shared<TextUI> uiPanel = uiRoot && uiRoot->Get_Children().size() == 1
			? dynamic_pointer_cast<TextUI>(uiRoot->Get_Children()[0]) : nullptr;
		const Shared<HpBarWorldUI> hpBar = uiPanel && uiPanel->Get_Children().size() == 1
			? dynamic_pointer_cast<HpBarWorldUI>(uiPanel->Get_Children()[0]) : nullptr;
		const Bool hierarchyRestored = player && uiRoot && uiPanel && hpBar &&
			uiPanel->Get_Name() == L"Phase6_UIPanel" && hpBar->Get_Name() == L"Phase6_HpBar" &&
			uiRoot->Get_AnchorState() == UI_ANCHOR::TOP_LEFT &&
			uiPanel->Get_AnchorState() == UI_ANCHOR::CENTER &&
			hpBar->Get_AnchorState() == UI_ANCHOR::TOP_CENTER;
		Check("launcher-client-pl0000-and-ui-restored", hierarchyRestored);
		Check("launcher-client-objectref-restored", hierarchyRestored &&
			hpBar->Get_TargetObjectGuid() == player->Get_ObjectGuid());
		const size_t objectCount = GAME_INSTANCE->Get_GameObjects(LauncherClientGateLevel).size();
		Check("failed-level-transition-preserves-current-world", sceneLoaded && player &&
			GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), nullptr) == E_INVALIDARG &&
			GAME_INSTANCE->Get_CurrentLevel() == gameplayLevel &&
			GAME_INSTANCE->Get_CurrentLevelIndex() == LauncherClientGateLevel &&
			GAME_INSTANCE->Get_GameObjects(LauncherClientGateLevel).size() == objectCount &&
			GAME_INSTANCE->Find(player->Get_ObjectGuid()) == player && !player->Is_Destroy());

		return passed && Save() ? S_OK : E_FAIL;
	}
#endif
}

ClientApp::ClientApp() {}

ClientApp::~ClientApp() {}

HRESULT ClientApp::Initialize(const ENGINE_DESC& desc) 
{
	if (FAILED(Client::Register_Client_Reflection()))
		return E_FAIL;
    if (FAILED(GAME_INSTANCE->Refresh_ReflectionRegistry()))
        return E_FAIL;

    ClientSettingManager::g_EngineDesc = desc;

    if (FAILED(ClientSettingManager::GetInstance()->Apply_LayerAndTagSettings())) {
      return E_FAIL;
    }
	if (FAILED(GAME_INSTANCE->Load_PrefabRepository(
		L"../../ProjectSetting/Prefab/")))
		return E_FAIL;
    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
        return E_FAIL;
    
    if (FAILED(Ready_InitialObject()))
        return E_FAIL;

#ifdef _DEBUG
	if (Is_LauncherClientGate())
		return Run_LauncherClientGate();
#endif

    if (FAILED(Ready_StartLevel(static_cast<LEVEL>(ClientSettingManager::g_EngineDesc.startLevel))))
      return E_FAIL;

    return S_OK;
}

HRESULT ClientApp::Ready_StartLevel(LEVEL startLevel) 
{
    const auto loadingLevel = LevelLoading::Create(GAME_INSTANCE->Get_Device(),
        GAME_INSTANCE->Get_Context(), startLevel, true);
    if (!loadingLevel) {
        LOG_ERROR(L"Startup blocked before level {}: Loading initialization failed; see the preceding resource error",
            ETOI(startLevel));
        return E_FAIL;
    }
    return GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), loadingLevel);
}

HRESULT ClientApp::Ready_InitialObject()
{
    GAME_INSTANCE->Add_Font(L"Nier_16", L"../../Client/bin/resources/Font/NierFont_16.spritefont");
    GAME_INSTANCE->Add_Font(L"Nier_32", L"../../Client/bin/resources/Font/NierFont_32.spritefont");
    GAME_INSTANCE->Add_Font(L"Nier_64", L"../../Client/bin/resources/Font/NierFont_64.spritefont");

    if (FAILED(GAME_INSTANCE->Register_ReflectedPrototypes()))
        return E_FAIL;

	const Shared<TextUI> textUiPrototype = TextUI::CreatePrototype();
	if (!textUiPrototype || FAILED(GAME_INSTANCE->Add_TypePrototype(
		ETOI(LEVEL::STATIC), textUiPrototype)))
		return E_FAIL;

    return S_OK;
}

Unique<ClientApp> ClientApp::Create(const ENGINE_DESC &desc) {
  Unique<ClientApp> mainApp = make_unique<ClientApp>();

  if (FAILED(mainApp->Initialize(desc))) {
    LOG_ERROR(L"ClientApp initialization failed; see the preceding startup error");
    return nullptr;
  }
  return mainApp;
}
