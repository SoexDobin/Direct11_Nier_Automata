#include "Texture.h"

#include <tchar.h>

Texture::Texture(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component(device, context)
{

}

Texture::Texture(const Shared<Texture>& rhs)
	: Component(rhs), m_NumSRVs(rhs->m_NumSRVs), m_SRVs(rhs->m_SRVs)
{
}

HRESULT Texture::Initialize_Prototype(const tChar* textureFilePath, uint32 numSRVs)
{
	m_NumSRVs = numSRVs;

	for (uint32 i = 0; i < m_NumSRVs; ++i)
	{
		tChar szFullPath[MAX_PATH] = TEXT("");
		wsprintf(szFullPath, textureFilePath, i);

		tChar szDrive[MAX_PATH] = TEXT("");
		tChar szDir[MAX_PATH] = TEXT("");
		tChar szName[MAX_PATH] = TEXT("");
		tChar szExt[MAX_PATH] = TEXT("");

		_tsplitpath_s(szFullPath, 
			szDrive, MAX_PATH, 
			szDir, MAX_PATH, 
			szName, MAX_PATH, 
			szExt, MAX_PATH);

		HRESULT hr = {};
		ComPtr<ID3D11Resource> texture{ nullptr };
		ComPtr<ID3D11ShaderResourceView> srv{ nullptr };

		try
		{
			if (!lstrcmp(szExt, TEXT(".dds")))
			{
				hr = CreateDDSTextureFromFile(m_Device.Get(), szFullPath, texture.GetAddressOf(), srv.GetAddressOf());
			}
			else if (!lstrcmp(szExt, TEXT(".tga")))
			{
				MSG_BOX("TGA Texture Loading Not Supported Yet");
				return E_FAIL;
			}
			else
			{
				hr = CreateWICTextureFromFile(m_Device.Get(), szFullPath, texture.GetAddressOf(), srv.GetAddressOf());
			}
			if (FAILED(hr))
			{
				// TODO : 로딩 실패 시 크래시 대신 실패 처리
				return E_FAIL;
			}
		}
		catch (...)
		{
			// TODO : DirectXTK 등의 내부 예외 발생 시 안전하게 처리
			return E_FAIL;
		}
		m_SRVs.push_back(srv);
	}
	return S_OK;
}

HRESULT Texture::Initialize(const Shared<void>& arg)
{
	return Component::Initialize(arg);
}

void Texture::On_Destroy()
{
	m_SRVs.clear();

	Component::On_Destroy();
}

Shared<Texture> Texture::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* textureFilePath, uint32 numSRVs)
{
	auto texture = make_shared<Texture>(device, context);

	if (FAILED(texture->Initialize_Prototype(textureFilePath, numSRVs)))
	{
		MSG_BOX("Failed to Created : Texture");
	}

	return texture;
}

Shared<Component> Texture::Clone(const Shared<void>& arg)
{
	auto texture = make_shared<Texture>(shared_from_this());

	if (FAILED(texture->Initialize(arg)))
	{
		MSG_BOX("Failed to Created : Texture");
	}

	return texture;
}
