#pragma once

#include "Engine_Define.h"

NS_BEGIN(Client)

class ClientSettingManager
{
	DECLARE_SINGLETON(ClientSettingManager)
	ClientSettingManager() = default;
	~ClientSettingManager() = default;

public:
	uint32 Get_LevelCount() const { return ETOI(LEVEL::LEVEL_END); }

	void Set_ResourcePath(const wstring& path) { m_ResourcePath = path; }
	void Set_ShaderPath(const wstring& path) { m_ShaderPath = path; }

public:
	HRESULT Load_EngineDesc(ENGINE_DESC& outDesc) const;
	HRESULT Apply_LayerAndTagSettings() const;
	HRESULT Load_Texture(LEVEL level) const;
	HRESULT Load_Shader() const;

private:
	wstring m_ResourcePath = L"../bin/resources/";
	wstring m_ShaderPath = L"../bin/shaders/";
	wstring m_ProjectSettingPath = L"../../ProjectSetting/";
};

NS_END
