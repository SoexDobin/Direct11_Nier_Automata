#include "LightGameObject.h"

#include "Game.h"

LightGameObject::LightGameObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {}
LightGameObject::LightGameObject(const LightGameObject& rhs)
	: GameObject{rhs} {}

HRESULT LightGameObject::Initialize_Prototype() {
	return GameObject::Initialize_Prototype();
}

HRESULT LightGameObject::Initialize(void* arg)
{
	LIGHT_OBJECT_DESC localDesc{};
	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;
	if (nullptr == arg)
	{
		// arg가 없으면 localDesc에 디폴트 값 할당
		localDesc.lightDesc.type = LIGHT::POINT;
		localDesc.lightDesc.position = Vector4(0.f, 0.f, 0.f, 1.f);
		localDesc.lightDesc.range = 5.0f;
		localDesc.lightDesc.diffuse = Vector4(1.f, 1.f, 1.f, 1.f);
		localDesc.lightDesc.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.f);
		localDesc.lightDesc.specular = Vector4(1.f, 1.f, 1.f, 1.f);
		arg = &localDesc; // 인자 포인터를 디폴트 구조체로 덮어씌움
	}
	if ((m_ObjectDesc = static_cast<OBJECT_DESC*>(arg)))
	{
		LIGHT_OBJECT_DESC& desc = *static_cast<LIGHT_OBJECT_DESC*>(arg);
		m_Transform->Set_Position(Vector3(desc.lightDesc.position.x, desc.lightDesc.position.y, desc.lightDesc.position.z));
		// 이제 정상적으로 m_Light가 할당됨
		m_Light = Light::Create(desc.lightDesc);
		if (FAILED(GAME_INSTANCE->Add_Light(m_Light)))
			return E_FAIL;
	}
	return S_OK;
}

void LightGameObject::On_Destroy()
{
	if (m_Light)
	{
		if (GAME_INSTANCE) // 싱글톤 파괴 중(게임 종료)일 때는 호출하지 않음
		{
			if (FAILED(GAME_INSTANCE->Remove_Light(m_Light)))
				return;
		}
		m_Light.reset();
	}

	GameObject::On_Destroy();
}

void LightGameObject::On_Enable()
{
	GameObject::On_Enable();
}

void LightGameObject::On_Disable()
{
	GameObject::On_Disable();
}

void LightGameObject::Priority_Update(Float timeDelta)
{

}

void LightGameObject::Update(Float timeDelta)
{

}

void LightGameObject::Late_Update(Float timeDelta)
{
	if (m_Light)
	{
		m_Transform->Update_WorldMatrix();

		m_Light->Get_LightDesc_Ref().position 
		= Vector4(m_Transform->Get_Position().x, m_Transform->Get_Position().y, m_Transform->Get_Position().z, 1.f);
		
		if (m_Light->Get_LightDesc_Ref().type == LIGHT::DIRECTIONAL)
		{
			Vector3 look = m_Transform->Get_Look();
			look.Normalize();
			m_Light->Get_LightDesc_Ref().direction = Vector4(look.x, look.y, look.z, 0.f);
		}
	}
}

const LIGHT_DESC& LightGameObject::Get_LightDesc() const
{
	return m_Light->Get_LightDesc();
}

LIGHT_DESC& LightGameObject::Get_LightDesc_Ref()
{
	return m_Light->Get_LightDesc_Ref();
}
