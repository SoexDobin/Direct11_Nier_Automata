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

	Set_AnchorState(UI_ANCHOR::TOP_LEFT);

	if (arg)
	{
		HP_BAR_WORLD_UI_DESC& desc = *static_cast<HP_BAR_WORLD_UI_DESC*>(arg);
		Set_TargetObjectGuid(desc.target ? desc.target->Get_ObjectGuid() : ObjectGuid{});
		m_WorldOffset = desc.worldOffset;
	}

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
	const Shared<Entity> target = Resolve_Target();
	if (!target)
		return;

	Vector3 worldPos = target->Get_Transform()->Get_Position() + m_WorldOffset;
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
	m_Transform->Set_LocalPosition(screenPos.x, screenPos.y, 0.f);
	Update_UITransform();
}

void HpBarWorldUI::Submit_RenderGroup()
{
	if (!Resolve_Target())
		return;

	if (m_TargetInBack) return;

	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::WORLDUI, shared_from_this());
}

HRESULT HpBarWorldUI::Render()
{
	const Shared<Entity> target = Resolve_Target();
	if (!target) return S_OK;
	if (false == Is_Active()) return S_OK;

	Float hpRatio = target->Get_HP() / target->Get_MaxHP();
	if (FAILED(m_Shader->Bind_RawValue(HpRatio, &hpRatio, sizeof(Float))))
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

HRESULT HpBarWorldUI::Set_TargetObjectGuid(ObjectGuid targetGuid)
{
	if (targetGuid.Is_Valid())
	{
		const Shared<GameObject> object = GAME_INSTANCE->Find(targetGuid);
		if (object && !dynamic_pointer_cast<Entity>(object))
			return E_NOINTERFACE;
	}
	m_TargetGuid = targetGuid;
	m_Target.reset();
	Resolve_Target();
	return S_OK;
}

Shared<Entity> HpBarWorldUI::Resolve_Target()
{
	if (!m_TargetGuid.Is_Valid())
	{
		m_Target.reset();
		return nullptr;
	}
	if (const Shared<Entity> target = m_Target.lock();
		target && !target->Is_Destroy() && target->Get_ObjectGuid() == m_TargetGuid)
		return target;
	const Shared<Entity> target = GAME_INSTANCE->Find<Entity>(m_TargetGuid);
	if (!target || target->Is_Destroy())
	{
		m_Target.reset();
		return nullptr;
	}
	m_Target = target;
	return target;
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


