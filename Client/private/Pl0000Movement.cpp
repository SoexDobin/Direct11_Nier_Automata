#include "pch.h"
#include "Pl0000Movement.h"

#include <Game.h>
#include <SpdLogger.h>
#include "Camera.h"
#include "Pl0000Input.h"
#include "Pl0000.h"
#include "Navigation.h"
#include "NavigationManager.h"

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

void Pl0000Movement::Set_TargetNavigation(const Shared<Navigation>& navigation)
{
	m_Navigation.reset();
	m_Navigation = navigation;
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
		// 1. 코어 인덱스 백업: 현재(프레임 시작)의 가장 안전한 정중앙 셀 인덱스
		int32 coreCellIndex = nav->Get_CurrentCellIndex();
		// 2. 지상 이동 중(Grounded)일 때 보이지 않는 벽 (원본 로직 동일 유지)
		if (m_IsGrounded)
		{
			Bool validNav = nav->Has_NeighborCell(nextPosition);
			if (!validNav)
			{
				nav->Set_CurrentCellIndex(coreCellIndex); // 오염 방지 롤백

				Vector3 rollbackPos = ownerTransform->Get_Position();
				rollbackPos.y += physicsDelta.y;
				Float groundHeight = nav->Get_HeightAtPoint(rollbackPos);
				if (rollbackPos.y <= groundHeight) {
					rollbackPos.y = groundHeight;
					m_Velocity.y = 0.f;
				}
				ownerTransform->Set_Position(rollbackPos);
				return;
			}
			else
			{
				// 정상적으로 지상 통과 시, 내 발밑을 코어 인덱스로 갱신
				coreCellIndex = nav->Get_CurrentCellIndex();
			}
		}
		Float targetGroundHeight = -FLT_MAX;
		Bool foundValidGround = false;
		// 3. Y Velocity가 0 이하여서 추락 중단(하강)일 때만 바닥을 찾습니다.
		if (m_Velocity.y <= 0.f)
		{
			constexpr Float offsetRadius = 0.5f;
			Vector3 offsets[5] = {
				Vector3{0.f, 0.f, 0.f},              // 1. Center (먼저 수행)
				Vector3{offsetRadius, 0.f, 0.f},     // 2. Right
				Vector3{-offsetRadius, 0.f, 0.f},    // 3. Left
				Vector3{0.f, 0.f, offsetRadius},     // 4. Forward
				Vector3{0.f, 0.f, -offsetRadius}     // 5. Backward
			};
			for (int i = 0; i < 5; ++i)
			{
				Vector3 samplePos = nextPosition + offsets[i];

				Bool isValid = nav->Has_NeighborCell(samplePos);
				if (!isValid)
				{
					isValid = nav->Compute_CurrentCellByPosition(samplePos);
				}
				if (isValid)
				{
					Float nav_y = nav->Get_HeightAtPoint(samplePos);
					if (nav_y > targetGroundHeight)
					{
						targetGroundHeight = nav_y;
						foundValidGround = true;
						// [매우 중요] 정중앙(i==0) 이거나, 코어 인덱스가 없을 때만 코어 인덱스를 갱신합니다.
						// 모서리(1~4번) 탐색으로 인해 엔진 인덱스가 엉뚱한 곳으로 튀는 것을 막습니다.
						if (i == 0 || coreCellIndex == -1)
						{
							coreCellIndex = nav->Get_CurrentCellIndex();
						}
					}
				}
			}
		}



		if (coreCellIndex != -1)
		{
			nav->Set_CurrentCellIndex(coreCellIndex);
		}
		// 착지 판정
		if (foundValidGround && nextPosition.y <= targetGroundHeight)
		{
			nextPosition.y = targetGroundHeight;
			m_Velocity.y = 0.f;
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


