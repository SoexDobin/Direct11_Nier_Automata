#include "pch.h"
#include "Em3000Movement.h"
#include "Em3000.h"
#include "Navigation.h"
#include <SpdLogger.h>

Em3000Movement::Em3000Movement() : Movement{} {}
Em3000Movement::Em3000Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Movement{ device, context } {
}
Em3000Movement::Em3000Movement(const Em3000Movement& rhs)
	: Movement{ rhs } {
}

HRESULT Em3000Movement::Initialize_Prototype()
{
	return Movement::Initialize_Prototype();
}

HRESULT Em3000Movement::Initialize(void* arg)
{
	if (FAILED(Movement::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em3000Movement::Begin()
{
	if (!m_Owner.expired())
	{
		m_OwnerContainer = static_pointer_cast<Em3000>(m_Owner.lock());
		if (m_OwnerContainer.expired())
		{
			LOG_ERROR(L"Failed To Find Em0010 Container");
			return E_FAIL;
		}

		m_Navigation = m_Owner.lock()->Get_Component<Navigation>();
		if (m_Navigation.expired())
		{
			LOG_ERROR(L"Failed To Find Em0010 Navigation");
			return E_FAIL;
		}
	}

	return S_OK;
}


Bool Em3000Movement::Has_ReachedTarget(Float threshold) const
{
	if (!m_HasWalkTarget) return true;
	if (m_Owner.expired()) return true;

	Vector3 currentPos = m_Owner.lock()->Get_Transform()->Get_Position();
	Vector3 diff = m_TargetPosition - currentPos;
	diff.y = 0.f;

	return diff.Length() <= threshold;
}

void Em3000Movement::Update_Movement(Float timeDelta)
{
	if (m_Owner.expired()) return;

	Shared<Transform> ownerTransform = m_Owner.lock()->Get_Transform();

	if (false == m_IsGrounded)
	{
		m_Velocity.y -= m_Gravity * timeDelta;
	}

	Vector3 physicalDelta = m_Velocity * timeDelta; // y이동

	TRANSFORM_FRAME transformFrame = m_OwnerContainer.lock()->Get_BodyModelTransform();
	Vector3 rootPositionVelocity = transformFrame.position;

	if (m_IsGrounded)
	{
		m_LastGroundedRootPositionVelocity = rootPositionVelocity;
		m_LastGroundedRootPositionVelocity.y = 0.f;
	}
	else
	{
		if (m_CurrentMoveData.isMove)
		{
			rootPositionVelocity = m_LastGroundedRootPositionVelocity;
		}
		else
		{
			rootPositionVelocity = Vector3::Zero;
		}
	}

	if (m_CurrentMoveData.canRotation)
	{
		// 기본 이동 방향
		Vector3 targetRotDir = m_CurrentMoveData.direction;

		// 명시된 look이 있으면 덮음
		if (m_CurrentMoveData.lookDirection.LengthSquared() > 0.f)
		{
			targetRotDir = m_CurrentMoveData.lookDirection;
		}

		if (targetRotDir.LengthSquared() > 0.f)
		{
			Float targetYaw = atan2f(targetRotDir.x, targetRotDir.z);
			Quaternion targetQuat = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.f, 0.f);

			Float turnDelta = m_TurnSpeed * timeDelta;
			Quaternion currentQuat = ownerTransform->Get_Quaternion();
			Quaternion nextQuat = Quaternion::Slerp(currentQuat, targetQuat, turnDelta);
			ownerTransform->Set_Rotation(nextQuat);
		}
	}

	Vector3 worldMoveVelocity{}; // x이동

	if (m_CurrentMoveData.isMove || m_CurrentMoveData.isAttack)
	{
		if (m_CurrentMoveData.useRootMotionDir)
		{
			worldMoveVelocity = Vector3::Transform(rootPositionVelocity * -1.f, ownerTransform->Get_Quaternion());

			worldMoveVelocity *= m_CurrentMoveData.rootMotionScale;
		}
		else
		{
			Float rootSpeed = rootPositionVelocity.Length();
			worldMoveVelocity = m_CurrentMoveData.direction * rootSpeed * m_CurrentMoveData.rootMotionScale;
		}
	}

	worldMoveVelocity *= m_RootMotionScale;

	Vector3 nextPosition = ownerTransform->Get_Position() + worldMoveVelocity * timeDelta + physicalDelta;

	nextPosition += m_CorrectionDelta;
	Reset_Correction();

	if (auto nav = m_Navigation.lock())
	{
		Float groundHeight = -FLT_MAX;
		Bool validNav = nav->Has_NeighborCell(nextPosition);

		if (validNav)
		{
			groundHeight = nav->Get_HeightAtPoint(nextPosition);
			nextPosition.y = groundHeight;

			m_IsGrounded = true;
			ownerTransform->Set_Position(nextPosition);
		}
		else
		{
			Vector3 rollbackPos = ownerTransform->Get_Position();

			groundHeight = nav->Get_HeightAtPoint(rollbackPos);

			if (rollbackPos.y <= groundHeight) {
				rollbackPos.y = groundHeight;
				m_IsGrounded = true;
				m_Velocity = Vector3::Zero;
			}

			ownerTransform->Set_Position(rollbackPos);
		}
	}
	else
	{
		ownerTransform->Set_Position(nextPosition);
	}
}

Shared<Em3000Movement> Em3000Movement::Create(const ComPtr<ID3D11Device>& device,
	const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3000Movement>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3000Movement");
		return nullptr;
	}

	return prototype;
}

Shared<Component> Em3000Movement::Clone(void* arg)
{
	auto instance = make_shared<Em3000Movement>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3000Movement");
		return nullptr;
	}

	return instance;
}


