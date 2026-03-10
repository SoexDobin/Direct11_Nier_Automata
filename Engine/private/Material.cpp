#include "Material.h"

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
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		m_MaterialTextures[i].clear();
	}

	Component::On_Destroy();
}

HRESULT Material::Initialize_Prototype(const aiMaterial* aiMaterial, const Char* modelFilePath)
{
	Char drivePath[MAX_PATH] = {};
	Char dirPath[MAX_PATH] = {};

	_splitpath_s(modelFilePath, drivePath, MAX_PATH, dirPath, MAX_PATH, nullptr, 0, nullptr, 0);

	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		uint32 numTextures = { aiMaterial->GetTextureCount(static_cast<aiTextureType>(i))};

		if (0 == numTextures)
			continue;

		m_MaterialTextures[i].reserve(numTextures);

		for (size_t j = 0; j < numTextures; ++j)
		{
			aiString texturePath = {};

			if (FAILED(aiMaterial->GetTexture(static_cast<aiTextureType>(i), j, &texturePath)))
				continue;

			Char fileNamePath[MAX_PATH] = {};
			Char EXTPath[MAX_PATH] = {};

			_splitpath_s(texturePath.C_Str(), nullptr, 0, nullptr, 0, fileNamePath, MAX_PATH, EXTPath, MAX_PATH);

			Char fullPath[MAX_PATH] = {};

			strcpy_s(fullPath, drivePath);
			strcat_s(fullPath, dirPath);
			strcat_s(fullPath, fileNamePath);
			strcat_s(fullPath, EXTPath);

			tChar textureFilePath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, fullPath, strlen(fullPath), textureFilePath, MAX_PATH);

			HRESULT hr = {};
			ComPtr<ID3D11ShaderResourceView> srv{ nullptr };

			if (false == strcmp(EXTPath, ".dds"))
				hr = CreateDDSTextureFromFile(m_Device.Get(), textureFilePath, nullptr, srv.GetAddressOf());
			else if (false == strcmp(EXTPath, ".tga"))
				hr = E_FAIL;
			else
				hr = CreateWICTextureFromFile(m_Device.Get(), textureFilePath, nullptr, srv.GetAddressOf());

			if (FAILED(hr))
			{
				LOG_ERROR(L"Failed to Load Material Path : {}", textureFilePath);
				return E_FAIL;
			}
				

			m_MaterialTextures[i].push_back(srv);
		}
	}
	return S_OK;
}

HRESULT Material::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}


HRESULT Material::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

HRESULT Material::Bind_Material(const Shared<Shader>& shader, const Char* constantName, aiTextureType materialType, uint32 textureIndex)
{
	if (textureIndex >= m_MaterialTextures[materialType].size() || 
		nullptr == m_MaterialTextures[materialType][textureIndex])
		return E_FAIL;

	if (FAILED(shader->Bind_SRV(constantName, m_MaterialTextures[materialType][textureIndex])))
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

Shared<Material> Material::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const aiMaterial* aiMaterial, const Char* modelFilePath)
{
	auto material = make_shared<Material>(device, context);

	if (FAILED(material->Initialize_Prototype(aiMaterial, modelFilePath)))
	{
		LOG_ERROR(L"Failed to Created : CMaterial Path {}", Helper::To_wString(modelFilePath));
		MSG_BOX("Failed to Created : CMaterial");
	}
	return material;
}

Shared<Component> Material::Clone(void* arg)
{
	return nullptr;
}
