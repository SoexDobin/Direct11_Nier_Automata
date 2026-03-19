#pragma once

#include "Engine_Define.h"

NS_BEGIN(Client)

class CLIENT_DLL ClientSettingManager
{
	DECLARE_SINGLETON(ClientSettingManager)
	ClientSettingManager() = default;
	~ClientSettingManager() = default;

public:
	uint32 Get_LevelCount() const { return ETOI(LEVEL::LEVEL_END); }

public: /* Project Path */
	void Set_ResourcePath(const wstring& path) { m_ResourcePath = path; }
	void Set_ShaderPath(const wstring& path) { m_ShaderPath = path; }

public: /* Texture */
	HRESULT Load_Textures_FromJson(LEVEL level) const;
	HRESULT Sync_TextureJson_FromCSV() const;

public: /* Model */
	HRESULT Load_Model_FromJson(LEVEL level) const;
	HRESULT Sync_ModelJson_FromCSV() const;

public: /* Shader */
	HRESULT Load_Shader() const;

public:
	HRESULT Ready_Client_Prototypes(LEVEL level) const;
	HRESULT Load_EngineDesc(ENGINE_DESC& outDesc) const;
	HRESULT Apply_LayerAndTagSettings() const;
	HRESULT Load_LevelData(LEVEL level) const;
	

private:

	wstring m_ResourcePath = L"../../Client/bin/resources/";
	wstring m_ShaderPath = L"../../Client/bin/shaders/";
	wstring m_ProjectSettingPath = L"../../ProjectSetting/";

public:
	static ENGINE_DESC g_EngineDesc;
};

NS_END
