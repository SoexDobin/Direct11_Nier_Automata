#include "pch.h"
#include "HowitzerBullet.h"

#include "Game.h"
#include <SpdLogger.h>
#include <SphereCollider.h>
#include <Model.h>

#include "Bullet.h"

HowitzerBullet::HowitzerBullet(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Bullet{device, context} {}
HowitzerBullet::HowitzerBullet(const HowitzerBullet& rhs)
	: Bullet{rhs} {}


HRESULT HowitzerBullet::Initialize_Prototype()
{
	return Bullet::Initialize_Prototype();
}

HRESULT HowitzerBullet::Initialize(void* arg)
{
	if (FAILED(Bullet::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize HowitzerBullet");
		return E_FAIL;
	}

	m_TargetLayerIndex = ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(m_Desc.targetLayer));
	m_DamageInfo = m_Desc.damageInfo;
	m_Transform->Set_Position(m_Desc.initialPosition);
	m_Transform->LookAt(m_Desc.initialPosition + m_Desc.direction);
	// 곡선 비행을 사용한다면 초기 3D Velocity 벡터를 생성해 줍니다.
	m_CurvedVelocity = m_Desc.direction * (m_Desc.speed * 0.5f); // 발사방향 + 초기속도
	m_CurvedVelocity.y += 10.0f; // 조금 띄워주려면 이처럼 Up Vector 힘을 줍니다.


	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready_Components HowitzerBullet");
		return E_FAIL;
	}
	return S_OK;
}
void HowitzerBullet::Update(Float timeDelta)
{
	if (m_TargetY <= m_Transform->Get_Position().y)
	{
		// TODO : 플레이어 타겟 충격파존
		// TODO Destroy 

		return;
	}
	auto howitzerDesc = static_cast<HOWITZER_BULLET_DESC>(m_Desc);

	// 포물선 이동: Y축 속도를 중력 가속도만큼 빼줍니다. (V = Vo + at)
	m_CurvedVelocity.y -= howitzerDesc.gravityStrength * timeDelta;
	// 포지션 업데이트 (프레임마다의 현재 이동속도 벡터 적용)
	Vector3 currentPos = m_Transform->Get_Position();
	currentPos += m_CurvedVelocity * timeDelta;
	m_Transform->Set_Position(currentPos);
	// 진행하는 방향을 자연스럽게 바라보게 하려면 LookAt을 업데이트합니다.
	m_Transform->LookAt(currentPos + m_CurvedVelocity);
	// 지면에 닿았다면 폭발 (예시: y가 0.f 이하, 혹은 물리엔진 Raycast)
	if (currentPos.y <= 0.0f)
	{
		currentPos.y = 0.0f;
		m_Transform->Set_Position(currentPos);

		//Explode();
		Object::Destroy(shared_from_this());
		return;
	}
	
	m_Collider->Update(m_Transform->Get_WorldMatrix());
	m_Transform->Update_WorldMatrix();
}

void HowitzerBullet::Late_Update(Float timeDelta)
{
	Bullet::Late_Update(timeDelta);
}

HRESULT HowitzerBullet::Render()
{
	return Bullet::Render();
}

void HowitzerBullet::Submit_RenderGroup()
{
	Bullet::Submit_RenderGroup();
}

void HowitzerBullet::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	Bullet::OnCollisionEnter(ownCollider, targetCollider);
}

HRESULT HowitzerBullet::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ m_Desc.resourceTag };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	SphereCollider::SPHERE_COLLIDER_DESC sphereColliderDesc{};
	sphereColliderDesc.radius = 0.5f;
	sphereColliderDesc.offset = Vector3::Zero;
	m_Collider = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &sphereColliderDesc);
	if (nullptr == m_Collider)
		return E_FAIL;

	m_Transform->Set_Scale(0.3f, 0.3f, 0.3f);

	return S_OK;
}

HRESULT HowitzerBullet::Bind_ShaderResources()
{
	return S_OK;
}

Shared<HowitzerBullet> HowitzerBullet::Create(const ComPtr<ID3D11Device>& device,
                                              const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<HowitzerBullet>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : HowitzerBullet");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> HowitzerBullet::Clone(void* arg)
{
	auto instance = make_shared<HowitzerBullet>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : HowitzerBullet");
		return nullptr;
	}

	return instance;
}


