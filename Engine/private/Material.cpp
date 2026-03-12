#include "Material.h"

#include <assimp/material.h>

#include "Game.h"
#include "Shader.h"
#include "SpdLogger.h"

Material::Material() : Component{} {}
Material::Material(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{device, context}
{
}
Material::Material(const Material& rhs)
	: Component{rhs}
{
}

void Material::On_Destroy()
{
	for (size_t i = 0; i < m_TextureTypeMax; i++)
	{
		m_MaterialTextures[i].clear();
	}

	Component::On_Destroy();
}

HRESULT Material::Initialize_Prototype(const MODEL_MATERIAL& materialData)
{
	Char drivePath[MAX_PATH] = {};
	Char dirPath[MAX_PATH] = {};

	m_TextureTypeMax = materialData.textureTypeMax;
	m_MaterialTextures = make_shared<vector<ComPtr<ID3D11ShaderResourceView>>[]>(m_TextureTypeMax);


	for (const auto& texEntry : materialData.textures)
	{
		uint32 typeIndex = texEntry.typeIndex;
		// 유효한 텍스처 타입 인덱스인지 안전 검사
		if (typeIndex >= m_TextureTypeMax)
			continue;
		Char szFileName[MAX_PATH] = {};
		Char szEXT[MAX_PATH] = {};
		// 컨버터가 저장한 텍스처 파일 이름과 확장자 분리
		_splitpath_s(texEntry.path.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);
		Char szFullPath[MAX_PATH] = {};
		// Model에서 주입해준 디렉토리 경로 + 텍스처 파일명 + 확장자로 절대/상대 경로 완성

		strcpy_s(szFullPath, materialData.directoryPath.c_str());
		strcat_s(szFullPath, szFileName);
		strcat_s(szFullPath, szEXT);
		// DirectX 텍스처 로더는 wstring을 요구하므로 변환

		WCHAR szTextureFilePath[MAX_PATH] = {};
		MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<int32>(strlen(szFullPath)), szTextureFilePath, MAX_PATH);
		HRESULT hr = S_OK;
		ComPtr<ID3D11ShaderResourceView> srv = nullptr;
		
		if (false == _stricmp(szEXT, ".dds"))
		{
			hr = CreateDDSTextureFromFile(m_Device.Get(), szTextureFilePath, nullptr, srv.GetAddressOf());
		}
		else if (false == _stricmp(szEXT, ".tga"))
		{	
			LOG_ERROR(L"TGA File Service not available");
			hr = E_FAIL;
		}
		else
		{
			hr = CreateWICTextureFromFile(m_Device.Get(), szTextureFilePath, nullptr, srv.GetAddressOf());
		}
		
		if (FAILED(hr))
		{
			LOG_ERROR(L"Failed To Load Texture : {}", szTextureFilePath);
			continue; // 혹은 return E_FAIL;
		}
		

		m_MaterialTextures[typeIndex].push_back(srv);
	}

	return Component::Initialize_Prototype();
}

HRESULT Material::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}


HRESULT Material::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

HRESULT Material::Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 textureTypeIndex, uint32 textureIndex)
{
	if (textureIndex >= m_MaterialTextures[textureTypeIndex].size() || 
		nullptr == m_MaterialTextures[textureTypeIndex][textureIndex])
		return E_FAIL;

	if (FAILED(shader->Bind_SRV(constantName, m_MaterialTextures[textureTypeIndex][textureIndex])))
	{
		LOG_ERROR(L"Failed to Bind srv to {}", Helper::To_wString(constantName));
		return E_FAIL;
	}

	return S_OK;
}

Shared<Material> Material::CreatePrototype()
{
	auto material = make_shared<Material>(
		GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

	return material;
}

Shared<Material> Material::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_MATERIAL& modelMaterial)
{
	auto material = make_shared<Material>(device, context);

	if (FAILED(material->Initialize_Prototype(modelMaterial)))
	{
		LOG_ERROR(L"Failed to Created : Material {}", Helper::To_wString(modelMaterial.name));
		MSG_BOX("Failed to Created : Material");
	}
	return material;
}

Shared<Component> Material::Clone(void* arg)
{
	return nullptr;
}
