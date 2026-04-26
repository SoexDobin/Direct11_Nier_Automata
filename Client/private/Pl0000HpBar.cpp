#include "pch.h"
#include "Pl0000HpBar.h"
#include "Entity.h"
#include <Game.h>
#include <SpdLogger.h>
#include <VIBuffer_Rect.h>

Pl0000HpBar::Pl0000HpBar() : UIObject{} {}

Pl0000HpBar::Pl0000HpBar(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: UIObject{device, context} {}

Pl0000HpBar::Pl0000HpBar(const Pl0000HpBar& rhs)
	: UIObject{rhs} {}

HRESULT Pl0000HpBar::Initialize_Prototype()
{
	return UIObject::Initialize_Prototype();
}

HRESULT Pl0000HpBar::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"Failed to Initialize Pl0000HpBar nullptr arg");
		return E_FAIL;
	}

	PL0000_HP_BAR_DESC& desc = *static_cast<PL0000_HP_BAR_DESC*>(arg);

	if (desc.sizeX == 0.f)
	{
		desc.anchor = UI_ANCHOR::TOP_LEFT;
		desc.x = 250.f;
		desc.y = 50.f;
		desc.sizeX = 400.f;
		desc.sizeY = 15.f;
	}

	if (FAILED(UIObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Initialize Pl0000HpBar");
		return E_FAIL;
	}

	m_Target = desc.target;
	Set_AnchorState(desc.anchor);
	Update_UITransform();

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready_Components Pl0000HpBar");
		return E_FAIL;
	}

	return S_OK;
}

void Pl0000HpBar::Priority_Update(Float timeDelta)
{
	if (m_Target.expired() || m_Target.lock()->Is_Destroy())
	{
		Destroy(shared_from_this());
		return;
	}
}

void Pl0000HpBar::Update(Float timeDelta)
{
	UIObject::Update(timeDelta);
}

void Pl0000HpBar::Late_Update(Float timeDelta)
{
	if (m_Target.expired() || m_Target.lock()->Is_Destroy())
		return;

	m_HpRatio = m_Target.lock()->Get_HP() / m_Target.lock()->Get_MaxHP();
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

HRESULT Pl0000HpBar::Render()
{
	if (m_Target.expired()) return S_OK;
	if (false == Is_Active()) return S_OK;

	if (FAILED(m_Shader->Bind_RawValue(HpRatio, &m_HpRatio, sizeof(Float))))
		return E_FAIL;

	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, DefaultMap, 0)))
		return E_FAIL;

	if (FAILED(m_BufferRect->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Shader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_BufferRect->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT Pl0000HpBar::Ready_Components()
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	auto shaderDesc = Shader::SHADER_DESC{ VTXTEX::Tag, VTXTEX::Elements, VTXTEX::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	auto textureDesc = Texture::TEXTURE_DESC{ levIndex, L"UI_Hp_Bar" };
	m_Texture = Add_Component<Texture>(levIndex, &textureDesc);
	if (nullptr == m_Texture)
		return E_FAIL;

	m_BufferRect = Add_Component<VIBuffer_Rect>();
	if (nullptr == m_BufferRect)
		return E_FAIL;

	return S_OK;
}

Shared<Pl0000HpBar> Pl0000HpBar::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Pl0000HpBar>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000HpBar");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Pl0000HpBar::Clone(void* arg)
{
	auto instance = make_shared<Pl0000HpBar>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000HpBar");
		return nullptr;
	}

	return instance;
}
