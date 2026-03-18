#include "Texture.h"
#include <tchar.h>

#include "Shader.h"
#include "Game.h"
#include "SpdLogger.h"

Texture::Texture() : Component {} {}

Texture::Texture(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
    : Component{ device, context }, m_RGBA{ Vector4::One } {
}

Texture::Texture(const Texture& rhs)
    : Component{ rhs }, m_NumSRVs{ rhs.m_NumSRVs }, m_SRVs{ rhs.m_SRVs }, 
    m_FilePath{ rhs.m_FilePath }, m_RGBA{ rhs.m_RGBA }, m_TextureTag{ rhs.m_TextureTag } {
}

HRESULT Texture::Initialize_Prototype(const tChar* textureFilePath, uint32 numSRVs, const wstring& textureTag)
{
    return Component::Initialize_Prototype();
}

HRESULT Texture::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Texture::Initialize(void* arg)
{
    if (arg == nullptr)
    {
        LOG_ERROR(L"There is no TextureDesc");
        MSG_BOX("There is no TextureDesc");
        return E_FAIL;
    }

    TEXTURE_DESC& desc = *static_cast<TEXTURE_DESC*>(arg);
    m_levIndex = desc.m_levIndex;
    m_SRVs.shrink_to_fit();

    if (!desc.m_TextureTag.empty())
    {
        // 1. 태그로 메타데이터 우선 획득
        const TEXTURE_DESC& registDesc = *GAME_INSTANCE->Get_TextureDesc(desc.m_levIndex, desc.m_TextureTag);

        m_NumSRVs = registDesc.m_NumSRVs;
        m_FilePath = registDesc.m_FilePath;
        m_SRVs.clear();
        m_SRVs.reserve(m_NumSRVs);
        // 2. 메타데이터에 기록된 정확한 개수만큼 리소스 요청
        for (uint32 i = 0; i < m_NumSRVs; ++i)
        {
            tChar szFullPath[MAX_PATH] = TEXT("");
            _stprintf_s(szFullPath, m_FilePath.c_str(), i);
            const ComPtr<ID3D11ShaderResourceView>& pSRV = GAME_INSTANCE->Get_Texture(desc.m_levIndex, szFullPath);

            if (pSRV == nullptr) {
                LOG_ERROR(L"[Texture] : Failed to find Tag '{}' Texture At '{}'", desc.m_TextureTag, szFullPath);
                return E_FAIL;
            }
            m_SRVs.push_back(pSRV);
        }
        m_TextureTag = desc.m_TextureTag;
    }
   
    return Component::Initialize(arg);
}

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

void Texture::Set_TextureByIndex(uint32 texIndex)
{
    if (texIndex >= m_SRVs.size() || m_SRVs.empty())
    {
        LOG_ERROR(L"Texture Out of Bounds");
        return;
    }

    m_SRVs[texIndex];
}

Shared<Texture> Texture::CreatePrototype()
{
    auto texture = make_shared<Texture>(
        GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

    if (FAILED(texture->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Texture");
    }

    return texture;
}

Shared<Texture> Texture::Create(const ComPtr<ID3D11Device> &device,
                                const ComPtr<ID3D11DeviceContext> &context,
                                const tChar* textureFilePath, uint32 numSRVs, const wstring& textureTag)
{
	auto texture = make_shared<Texture>(device, context);

	if (FAILED(texture->Initialize_Prototype(textureFilePath, numSRVs, textureTag))) {
		MSG_BOX("Failed to Created : Texture");
	}

	return texture;
}

Shared<Component> Texture::Clone(void* arg) 
{
    auto texture = make_shared<Texture>(*this);

    if (FAILED(texture->Initialize(arg))) 
    {
    	MSG_BOX("Failed to Created : Texture");
    }

    return texture;
}
