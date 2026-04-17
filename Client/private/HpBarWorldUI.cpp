#include "pch.h"
#include "HpBarWorldUI.h"
#include "Entity.h"
#include <Game.h>
#include <SpdLogger.h>
#include <VIBuffer_Rect.h>

HpBarWorldUI::HpBarWorldUI() : WorldUIObject{} {}
HpBarWorldUI::HpBarWorldUI(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldUIObject{device, context} {}
HpBarWorldUI::HpBarWorldUI(const HpBarWorldUI& rhs)
	: WorldUIObject{rhs} {}

HRESULT HpBarWorldUI::Initialize_Prototype()
{
	return WorldUIObject::Initialize_Prototype();
}

HRESULT HpBarWorldUI::Initialize(void* arg)
{
	if (FAILED(WorldUIObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize HPBarWorldUI");
		return E_FAIL;
	}

	if (nullptr == arg)
	{
		LOG_ERROR(L"Failed to Initialize HPBarWorldUI nullptr arg");
		return E_FAIL;
	}

	Set_AnchorState(UI_ANCHOR::TOP_LEFT);

	HP_BAR_WORLD_UI_DESC& desc = *static_cast<HP_BAR_WORLD_UI_DESC*>(arg);
	m_Target = desc.target;
	m_WorldOffset = desc.worldOffset;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready_Components HPBarWorldUI");
		return E_FAIL;
	}

	return S_OK;
}



void HpBarWorldUI::Priority_Update(Float timeDelta)
{

}

void HpBarWorldUI::Update(Float timeDelta)
{
	UIObject::Update(timeDelta);
}

void HpBarWorldUI::Late_Update(Float timeDelta)
{
	if (m_Target.expired() || m_Target.lock()->Is_Destroy())
	{
		Destroy(shared_from_this());
		return;
	}

	Vector3 worldPos = m_Target.lock()->Get_Transform()->Get_Position() + m_WorldOffset;
	Matrix viewMat = GAME_INSTANCE->Get_Transform(D3DTS::VIEW);
	Matrix projMat = GAME_INSTANCE->Get_Transform(D3DTS::PROJ);

	Vector3 cameraViewMat = XMVector3TransformCoord(worldPos, viewMat);
	if (cameraViewMat.z < 0.f)
	{
		m_TargetInBack = true;
		return;
	}

	D3D11_VIEWPORT viewport = GAME_INSTANCE->Get_ViewportDesc();

	Vector3 screenPos = XMVector3Project(
		worldPos,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		viewport.MinDepth, viewport.MaxDepth,
		projMat, viewMat, Matrix::Identity
	);

	m_TargetInBack = false;
	m_Transform->Set_Position(screenPos.x, screenPos.y, 0.5f);
	Update_UITransform();
}

void HpBarWorldUI::Submit_RenderGroup()
{
	if (m_Target.expired() || m_Target.lock()->Is_Destroy())
		return;

	//if (m_TargetInBack) return;

	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::WORLDUI, shared_from_this());
}

HRESULT HpBarWorldUI::Render()
{
	if (m_Target.expired()) return S_OK;
	if (false == Is_Active()) return S_OK;

	Float hpRatio = m_Target.lock()->Get_HP() / m_Target.lock()->Get_MaxHP();
	if (FAILED(m_Shader->Bind_RawValue(HpRatio, &hpRatio, sizeof(Float))))
		return E_FAIL;

	Vector3 targetWorldPos = m_Target.lock()->Get_Transform()->Get_Position() + m_WorldOffset;
	if (FAILED(m_Shader->Bind_RawValue(TargetPosition, &targetWorldPos, sizeof(Vector3))))
		return E_FAIL;

	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, CameraPosition)))
		return E_FAIL;

	if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, DefaultMap, 0)))
		return E_FAIL;

	if (FAILED(m_BufferRect->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_Shader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_BufferRect->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT HpBarWorldUI::Ready_Components()
{
	auto shaderDesc = Shader::SHADER_DESC{ VTXTEX::Tag, VTXTEX::Elements, VTXTEX::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	auto textureDesc = Texture::TEXTURE_DESC{ ETOI(LEVEL::GAMEPLAY), L"UI_Hp_Bar" };
	m_Texture = Add_Component<Texture>(ETOI(LEVEL::GAMEPLAY), &textureDesc);
	if (nullptr == m_Texture)
		return E_FAIL;

	m_BufferRect = Add_Component<VIBuffer_Rect>();
	if (nullptr == m_BufferRect)
		return E_FAIL;

	return S_OK;

}

Shared<HpBarWorldUI> HpBarWorldUI::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<HpBarWorldUI>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : HpBarWorldUI");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> HpBarWorldUI::Clone(void* arg)
{
	auto instance = make_shared<HpBarWorldUI>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : HpBarWorldUI");
		return nullptr;
	}

	return instance;
}


