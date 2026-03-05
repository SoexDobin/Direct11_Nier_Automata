#include "Texture.h"

#include "Shader.h"
#include <tchar.h>

Texture::Texture() 
	: Component {} {}

Texture::Texture(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
    : Component{ device, context } {}

Texture::Texture(const Texture& rhs)
    : Component{ rhs }, m_NumSRVs{ rhs.m_NumSRVs }, m_SRVs{ rhs.m_SRVs } {
}

HRESULT Texture::Initialize_Prototype(const tChar* textureFilePath, uint32 numSRVs) {
    m_NumSRVs = numSRVs;

    for (uint32 i = 0; i < m_NumSRVs; ++i) {
        tChar szFullPath[MAX_PATH] = TEXT("");
        wsprintf(szFullPath, textureFilePath, i);

        tChar szDrive[MAX_PATH] = TEXT("");
        tChar szDir[MAX_PATH] = TEXT("");
        tChar szName[MAX_PATH] = TEXT("");
        tChar szExt[MAX_PATH] = TEXT("");

        _tsplitpath_s(szFullPath, szDrive, MAX_PATH, szDir, MAX_PATH, szName,
                      MAX_PATH, szExt, MAX_PATH);

        HRESULT hr = {};
        ComPtr<ID3D11Resource> texture{nullptr};
        ComPtr<ID3D11ShaderResourceView> srv{nullptr};

        if (!lstrcmp(szExt, TEXT(".dds"))) {
            hr = CreateDDSTextureFromFile(m_Device.Get(), szFullPath,
                texture.GetAddressOf(),
                srv.GetAddressOf());
        }
        else if (!lstrcmp(szExt, TEXT(".tga"))) {
            MSG_BOX("TGA Texture Loading Not Supported Yet");
            return E_FAIL;
        }
        else {
            hr = CreateWICTextureFromFile(m_Device.Get(), szFullPath,
                texture.GetAddressOf(),
                srv.GetAddressOf());
        }
        if (FAILED(hr)) {
            return E_FAIL;
        }
    	m_SRVs.push_back(srv);
    }
    return Component::Initialize_Prototype();
}

HRESULT Texture::Initialize(void *arg) { return Component::Initialize(arg); }

void Texture::On_Destroy() {
  m_SRVs.clear();

  Component::On_Destroy();
}

HRESULT Texture::Bind_ShaderResourceView(const Shared<Shader>& shader,
                                         const Char* constantName,
                                         uint32 index) {
	if (index >= m_NumSRVs) {
		return E_INVALIDARG;
	}

	return shader->Bind_SRV(constantName, m_SRVs[index]);
}

HRESULT Texture::Bind_Texture(const wstring& texturefilePath)
{
    // 게임 시작시 리소스 경로의 레벨별 텍스쳐, fpx 파일 전부 읽어오기
	// AddPrototype하면서 해당 경로의 path / typeid 를 ResourceManager에 등록
    // AddComponent할때 path를 주면서 Texture, shader, fbx등 Rescource일 경우 중복 검사는 패스
    // ResourceManager에게 typeid를 받아 해당 typeid이 리소스 컴포넌트 찾아서 Clone

    // rttr을 통해서 Editor쪽에는 해당 리소스 파일들을 레벨별로 분리해서 뷰어로 보여주기
    // 뷰어에 있는 Texture -> Texture, Shader -> Shader 드래그 드랍을 통해서 적용하면 적용되도록 
    // Set Path()이후 Bind_Res

    // Editor는 SaveData 라는 MetaData칸을 따로 할당해서
    // Editor가 Stop상태일때 Ctrl+s, or Exit하면 (만약 Pause나 Play상태에서 하면 Ctrl+s는 무시, Exit는 강제 Stop하고나서 아래꺼 다하고 종료)
    // Hierarchy 의 오브젝트들의 trans, layer, tag, texture, shader등의 데이터를 (GameObject는 부모 자식도)

    // 객체 1개당 rttr 저장 > json에 수기

    
    // 여기 까지되면 AssetManager를 통해서 Clone Scene
    // Create 시트도 만들어야함

    // TODO : Resource Manager 한테 해당 path를 가진 typeid 부탁해서 Clone

    return S_OK;
}

Shared<Texture> Texture::Create(const ComPtr<ID3D11Device> &device,
                                const ComPtr<ID3D11DeviceContext> &context,
                                const tChar* textureFilePath, uint32 numSRVs) 
{
	auto texture = make_shared<Texture>(device, context);

	if (FAILED(texture->Initialize_Prototype(textureFilePath, numSRVs))) {
		MSG_BOX("Failed to Created : Texture");
	}

	return texture;
}

Shared<Component> Texture::Clone(void *arg) 
{
    auto texture = make_shared<Texture>(*this);

    if (FAILED(texture->Initialize(arg))) 
    {
    	MSG_BOX("Failed to Created : Texture");
    }

    return texture;
}
