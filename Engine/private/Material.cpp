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
	: Component{rhs}, m_TextureTypeMax{rhs.m_TextureTypeMax},
	  m_TextureMask{rhs.m_TextureMask}, m_MaterialTextures{rhs.m_MaterialTextures}
{
}

namespace
{
	uint32 To_MaterialTextureMask(uint32 textureTypeIndex)
	{
		switch (textureTypeIndex)
		{
		case 1:  return MATERIAL_TEXTURE_BASE_COLOR; // aiTextureType_DIFFUSE
		case 4:  return MATERIAL_TEXTURE_EMISSIVE;   // aiTextureType_EMISSIVE
		case 6:  return MATERIAL_TEXTURE_NORMAL;     // aiTextureType_NORMALS
		case 8:  return MATERIAL_TEXTURE_OPACITY;    // aiTextureType_OPACITY
		case 14: return MATERIAL_TEXTURE_EMISSIVE;   // aiTextureType_EMISSION_COLOR
		case 15: // aiTextureType_METALNESS
		case 16: // aiTextureType_DIFFUSE_ROUGHNESS
		case 17: return MATERIAL_TEXTURE_ORM;        // aiTextureType_AMBIENT_OCCLUSION
		default: return 0;
		}
	}
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
	m_TextureTypeMax = materialData.textureTypeMax;
	m_MaterialTextures = make_shared<vector<ComPtr<ID3D11ShaderResourceView>>[]>(m_TextureTypeMax);


	for (const auto& texEntry : materialData.textures)
	{
		uint32 typeIndex = texEntry.typeIndex;
		// 유효한 텍스처 타입 인덱스인지 안전 검사
		if (typeIndex >= m_TextureTypeMax)
			continue;
		filesystem::path texturePath = filesystem::path(materialData.directoryPath) / texEntry.path;
		if (!filesystem::is_regular_file(texturePath))
		{
			texturePath = filesystem::path(materialData.directoryPath) /
				"Textures" / filesystem::path(texEntry.path).filename();
		}
		const string extension = texturePath.extension().string();
		const wstring textureFilePath = texturePath.wstring();
		// 컨버터가 저장한 텍스처 파일 이름과 확장자 분리
		// Model에서 주입해준 디렉토리 경로 + 텍스처 파일명 + 확장자로 절대/상대 경로 완성

		// DirectX 텍스처 로더는 wstring을 요구하므로 변환

		HRESULT hr = S_OK;
		ComPtr<ID3D11ShaderResourceView> srv = nullptr;
		
		if (false == _stricmp(extension.c_str(), ".dds"))
		{
			hr = CreateDDSTextureFromFile(m_Device.Get(), textureFilePath.c_str(), nullptr, srv.GetAddressOf());
		}
		else if (false == _stricmp(extension.c_str(), ".tga"))
		{	
			LOG_ERROR(L"TGA File Service not available");
			hr = E_FAIL;
		}
		else
		{
			hr = CreateWICTextureFromFile(m_Device.Get(), textureFilePath.c_str(), nullptr, srv.GetAddressOf());
		}
		
		if (FAILED(hr))
		{
			LOG_ERROR(L"Failed To Load Texture : {}", textureFilePath);
			continue; // 혹은 return E_FAIL;
		}
		

		m_MaterialTextures[typeIndex].push_back(srv);
		m_TextureMask |= To_MaterialTextureMask(typeIndex);
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

	if (shader->Supports_CBuffer(ConstantBuffer::Material))
	{
		MaterialCB materialBuffer{};
		materialBuffer.textureMask = m_TextureMask;
		if (FAILED(shader->Bind_CBufferData(materialBuffer)))
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
