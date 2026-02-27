#pragma once

#include "Engine_Define.h"

NS_BEGIN(Client)

class ClientSettingManager
{
	DECLARE_SINGLETON(ClientSettingManager)
	ClientSettingManager() = default;
	~ClientSettingManager() = default;
public:
	HRESULT Load_EngineDesc() const;
	HRESULT Apply_LayerAndTagSettings() const;
	HRESULT Load_Texture(LEVEL level) const;
	HRESULT Load_Shader() const;

private:
	wstring m_ResourcePath = L"../bin/resources/";
	wstring m_ShaderPath = L"../bin/shaders/";
	wstring m_ProjectSettingPath = L"../../ProjectSetting/";
};

NS_END
