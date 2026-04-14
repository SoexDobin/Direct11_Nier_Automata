#include "pch.h"
#include "Pl0000Movement.h"

#include <Game.h>
#include <SpdLogger.h>
#include "Camera.h"
#include "Pl0000Input.h"
#include "Pl0000.h"
#include "Navigation.h"

Pl0000Movement::Pl0000Movement() : Movement{} {}
Pl0000Movement::Pl0000Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Movement{device, context} {}
Pl0000Movement::Pl0000Movement(const Pl0000Movement& rhs) : Movement{rhs} {}

HRESULT Pl0000Movement::Initialize_Prototype()
{
	return Movement::Initialize_Prototype();
}

HRESULT Pl0000Movement::Initialize(void* arg)
{
	if (FAILED(Movement::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Pl0000Movement::Begin()
{
	if (!m_Owner.expired())
	{
		m_OwnerContainer = static_pointer_cast<Pl0000>(m_Owner.lock());
		if (m_OwnerContainer.expired())
		{
			LOG_ERROR(L"Failed To Find Pl0000 Container");
			return E_FAIL;
		}

		m_Input = m_Owner.lock()->Get_Component<Pl0000Input>();
		if (m_Input.expired())
		{
			LOG_ERROR(L"Failed To Find Pl0000Input");
			return E_FAIL;
		}

		m_Navigation = m_Owner.lock()->Get_Component<Navigation>();
		if (m_Navigation.expired())
		{
			LOG_ERROR(L"Failed To Find Pl0000 Navigation");
			return E_FAIL;
		}
	}

	return S_OK;
}

void Pl0000Movement::Update_Movement(Float timeDelta)
{
	if (m_Owner.expired() || m_Input.expired() || m_Navigation.expired()) return;

	Shared<Transform> ownerTransform = m_Owner.lock()->Get_Transform();

	if (!m_IsGrounded)
	{
		m_Velocity.y -= m_Gravity * timeDelta;
	}
	Vector3 physicsDelta = m_Velocity * timeDelta;
	
	TRANSFORM_FRAME transformFrame = m_OwnerContainer.lock()->Get_BodyModelTransform();
	Vector3 rootPositionVelocity = transformFrame.position;

	if (m_IsGrounded)
	{
		m_LastGroundedRootPositionVelocity = rootPositionVelocity;
		m_LastGroundedRootPositionVelocity.y = 0.f; // H O X Y 모르니
	}
	else // grounded
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
		// 기본적으로는 이동하는 방향(direction)을 회전 타겟으로 잡습니다.
		Vector3 targetRotDir = m_CurrentMoveData.direction;
		// 만약 명시적인 시선 방향(lookDirection) 정보가 들어왔다면 로직을 덮어씌웁니다.
		if (m_CurrentMoveData.lookDirection.LengthSquared() > 0.f)
		{
			targetRotDir = m_CurrentMoveData.lookDirection;
		}
		// 최종적으로 바라볼 방향 벡터가 유효할 때만 회전시킵니다.
		if (targetRotDir.LengthSquared() > 0.f)
		{
			Float targetYaw = atan2f(targetRotDir.x, targetRotDir.z);
			Quaternion targetQuat = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.f, 0.f);
			Float turnSpeed = m_TurnSpeed * timeDelta;
			Quaternion currentQuat = ownerTransform->Get_Quaternion();
			Quaternion nextQuat = Quaternion::Slerp(currentQuat, targetQuat, turnSpeed);
			ownerTransform->Set_Rotation(nextQuat);
		}
	}
	
	Vector3 worldMoveVelocity{};
	if (m_CurrentMoveData.useRootMotionDir)
	{
		worldMoveVelocity = Vector3::Transform(rootPositionVelocity * -1.f, ownerTransform->Get_Quaternion());
	}
	else
	{
		Float rootSpeed = rootPositionVelocity.Length();
		worldMoveVelocity = m_CurrentMoveData.direction * rootSpeed;
	}

	worldMoveVelocity *= m_RootMotionScale;
	
	Vector3 nextPosition = ownerTransform->Get_Position() + worldMoveVelocity * timeDelta + physicsDelta;
	nextPosition += m_CorrectionDelta;
	Reset_Correction();

	if (auto nav = m_Navigation.lock())
	{
		Bool validNav = nav->Has_NeighborCell(nextPosition);

		if (!m_IsGrounded || validNav)
		{
			Float groundHeight = -FLT_MAX;
			
			if (validNav)
			{
				groundHeight = nav->Get_HeightAtPoint(nextPosition);
			}
			else
			{
				// 공중에서 점프하여 NavMesh 구역을 벗어난 경우 (또는 다른 섬으로 건너뛰는 경우)
				// 밑에 유효한 셀이 있는지 글로벌 탐색 시도
				nav->Compute_CurrentCellByPosition(nextPosition);
				if (nav->Get_CurrentCellIndex() != -1)
				{
					groundHeight = nav->Get_HeightAtPoint(nextPosition);
				}
			}

			if (nextPosition.y <= groundHeight)
			{
				nextPosition.y = groundHeight;
				m_Velocity = Vector3::Zero;
				m_IsGrounded = true;
			}
			else
			{
				m_IsGrounded = false;
			}

			ownerTransform->Set_Position(nextPosition);
		}
		else
		{
			// 지상에서 벗어나려 한 경우 (보이지 않는 벽) => X,Z축 롤백 수행
			Vector3 rollbackPos = ownerTransform->Get_Position();

			rollbackPos.y += physicsDelta.y;

			Float groundHeight = nav->Get_HeightAtPoint(rollbackPos);
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

Shared<Pl0000Movement> Pl0000Movement::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Pl0000Movement>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000Movement");
		return nullptr;
	}

	return prototype;
}

Shared<Component> Pl0000Movement::Clone(void* arg)
{
	auto instance = make_shared<Pl0000Movement>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000Movement");
		return nullptr;
	}

	return instance;
}


