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

void Em3000Movement::Set_TargetNavigation(const Shared<Navigation>& navigation)
{
	m_Navigation.reset();
	m_Navigation = navigation;
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
	// ── 기존 중력/물리 처리 (유지) ──────────────────────────────
	if (false == m_IsGrounded)
	{
		m_Velocity.y -= m_Gravity * timeDelta;
	}
	Vector3 physicalDelta = m_Velocity * timeDelta;
	// ── 애니메이션 루트 본 TransformFrame 획득 ─────────────────
	// Get_BodyModelTransform()은 Em3000Body::Get_ModelTransform()
	// → m_Model->Get_RootTransformVelocity(m_RootBoneIndex) 반환
	TRANSFORM_FRAME transformFrame = m_OwnerContainer.lock()->Get_BodyModelTransform();
	// ── Position 속도 (기존 패턴 유지) ─────────────────────────
	Vector3 rootPositionVelocity = transformFrame.position;
	if (m_IsGrounded)
	{
		m_LastGroundedRootPositionVelocity = rootPositionVelocity;
		m_LastGroundedRootPositionVelocity.y = 0.f;
	}
	else
	{
		rootPositionVelocity = m_CurrentMoveData.isMove
			? m_LastGroundedRootPositionVelocity
			: Vector3::Zero;
	}
	// ── [★ 핵심 추가] Velocity Quaternion → Y축 누적 회전 ──────
	if (m_CurrentMoveData.canRotation)
	{
		// 1순위: State에서 명시적으로 direction/lookDirection을 지정한 경우 (기존 Slerp 방식)
		Vector3 targetRotDir = m_CurrentMoveData.lookDirection.LengthSquared() > 0.f
			? m_CurrentMoveData.lookDirection
			: m_CurrentMoveData.direction;
		if (targetRotDir.LengthSquared() > 0.f)
		{
			// 기존 패턴 그대로: 목표 방향으로 Slerp
			Float targetYaw = atan2f(targetRotDir.x, targetRotDir.z);
			Quaternion targetQuat = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.f, 0.f);
			Quaternion currentQuat = ownerTransform->Get_Quaternion();
			Quaternion nextQuat = Quaternion::Slerp(currentQuat, targetQuat, m_TurnSpeed * timeDelta);
			ownerTransform->Set_Rotation(nextQuat);
		}
		else
		{
			// 2순위: direction 없으면 Velocity Quaternion의 Y축으로 직접 회전
			// transformFrame.rotation = qtCurr * qtInvPrev (프레임 간 델타 Quaternion)
			Quaternion qtDelta = transformFrame.rotation;
			// 델타 Quaternion에서 Yaw(Y축 회전량)만 추출
			Float yaw = 2.f * atan2f(
				2.f * (qtDelta.w * qtDelta.y + qtDelta.x * qtDelta.z),
				1.f - 2.f * (qtDelta.y * qtDelta.y + qtDelta.z * qtDelta.z)
			);
			// timeDelta로 보상: 델타는 "지난 프레임 1회분"이므로
			// Animation이 TickPerSecond*timeDelta로 이미 이동한 양임
			// → 그대로 누적 (timeDelta 추가 곱셈 불필요)
			Quaternion currentQuat = ownerTransform->Get_Quaternion();
			Quaternion deltaQuat = Quaternion::CreateFromYawPitchRoll(yaw, 0.f, 0.f);
			ownerTransform->Set_Rotation(currentQuat * deltaQuat);
		}
	}
	// ── 위치 이동 (기존 패턴 유지) ─────────────────────────────
	Vector3 worldMoveVelocity{};
	if (m_CurrentMoveData.isMove || m_CurrentMoveData.isAttack)
	{
		if (m_CurrentMoveData.useRootMotionDir)
		{
			worldMoveVelocity = Vector3::Transform(
				rootPositionVelocity * -1.f,
				ownerTransform->Get_Quaternion());
			worldMoveVelocity *= m_CurrentMoveData.rootMotionScale;
		}
		else
		{
			Float rootSpeed = rootPositionVelocity.Length();
			worldMoveVelocity = m_CurrentMoveData.direction * rootSpeed * m_CurrentMoveData.rootMotionScale;
		}
	}
	worldMoveVelocity *= m_RootMotionScale;
	Vector3 nextPosition = ownerTransform->Get_Position()
		+ worldMoveVelocity * timeDelta
		+ physicalDelta;
	nextPosition += m_CorrectionDelta;
	Reset_Correction();
	// ── Navigation 처리 (기존 유지) ────────────────────────────
	if (auto nav = m_Navigation.lock())
	{
		if (nav->Has_NeighborCell(nextPosition))
		{
			nextPosition.y = nav->Get_HeightAtPoint(nextPosition);
			m_IsGrounded = true;
			ownerTransform->Set_Position(nextPosition);
		}
		else
		{
			Vector3 rollbackPos = ownerTransform->Get_Position();
			Float groundHeight = nav->Get_HeightAtPoint(rollbackPos);
			if (rollbackPos.y <= groundHeight)
			{
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


