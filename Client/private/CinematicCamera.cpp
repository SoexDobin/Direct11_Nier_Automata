#include "pch.h"
#include "CinematicCamera.h"
#include "ClientSettingManager.h"
#include "Transform.h"

CinematicCamera::CinematicCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Camera{ device, context } {}
CinematicCamera::CinematicCamera(const CinematicCamera& rhs)
	: Camera{ rhs } {}

HRESULT CinematicCamera::Initialize_Prototype()
{
	return Camera::Initialize_Prototype();
}

HRESULT CinematicCamera::Initialize(void* arg)
{
	if (nullptr == arg)
		return E_FAIL;

	m_CineDesc = *static_cast<CINEMATIC_CAMERA_DESC*>(arg);

	if (m_CineDesc.nearPlane <= 0.f) m_CineDesc.nearPlane = 0.1f;
	if (m_CineDesc.farPlane <= m_CineDesc.nearPlane) m_CineDesc.farPlane = 1000.f;
	if (m_CineDesc.aspect <= 0.f) m_CineDesc.aspect = 1.777f; 

	
	m_CineDesc.eye = Vector4(m_CineDesc.startPosition.x, m_CineDesc.startPosition.y, m_CineDesc.startPosition.z, 1.f);

	Matrix matRot = Matrix::CreateFromYawPitchRoll(m_CineDesc.startRotation.y, m_CineDesc.startRotation.x, m_CineDesc.startRotation.z);
	Vector3 look = Vector3::TransformNormal(Vector3::UnitZ, matRot);
	Vector3 lookPos = m_CineDesc.startPosition + look;
	m_CineDesc.at = Vector4(lookPos.x, lookPos.y, lookPos.z, 1.f);
	
	m_CineDesc.fovY = XMConvertToRadians(m_CineDesc.startFov); 
	
	if (FAILED(Camera::Initialize(&m_CineDesc)))
		return E_FAIL;

	return S_OK;
}

void CinematicCamera::Priority_Update(Float timeDelta)
{
	if (!m_IsInvoked) return;

	Update_CameraTransform(timeDelta);
}

void CinematicCamera::Update(Float timeDelta)
{
	if (!m_IsInvoked) return;

	m_AccumulatedTime += timeDelta;

	// 회전 보간용 쿼터니언 준비
	Quaternion qStart = Quaternion::CreateFromYawPitchRoll(m_CineDesc.startRotation.y, m_CineDesc.startRotation.x, m_CineDesc.startRotation.z);
	Quaternion qTarget = Quaternion::CreateFromYawPitchRoll(m_CineDesc.targetRotation.y, m_CineDesc.targetRotation.x, m_CineDesc.targetRotation.z);

	// 1. 시작 대기 시간 (startWaitTime)
	if (m_AccumulatedTime < m_CineDesc.startWaitTime)
	{
		m_Transform->Set_Position(m_CineDesc.startPosition);
		m_Transform->Set_Rotation(qStart);
		m_FovY = XMConvertToRadians(m_CineDesc.startFov);
		return;
	}

	Float moveElapsed = m_AccumulatedTime - m_CineDesc.startWaitTime;

	// 2. 이동 시간 (travelTime)
	if (moveElapsed < m_CineDesc.travelTime)
	{
		Float ratio = moveElapsed / m_CineDesc.travelTime;
		
		// 위치 보간
		m_Transform->Set_Position(Vector3::Lerp(m_CineDesc.startPosition, m_CineDesc.targetPosition, ratio));

		// 회전 보간 (Slerp 적용 - 회전 꼬임 방지)
		m_Transform->Set_Rotation(Quaternion::Slerp(qStart, qTarget, ratio));

		// FOV 보간
		m_FovY = XMConvertToRadians(m_CineDesc.startFov) + (XMConvertToRadians(m_CineDesc.targetFov) - XMConvertToRadians(m_CineDesc.startFov)) * ratio;

		return;
	}

	// 3. 복귀 시간 (returnTime)
	Float returnElapsed = moveElapsed - m_CineDesc.travelTime;
	if (returnElapsed < m_CineDesc.returnTime)
	{
		if (m_PrevCamera)
		{
			Float ratio = returnElapsed / m_CineDesc.returnTime;
			
			Vector3 prevPos = m_PrevCamera->Get_Transform()->Get_Position();
			Quaternion qPrev = m_PrevCamera->Get_Transform()->Get_Quaternion();
			Float prevFov = m_PrevCamera->Get_FovY();

			m_Transform->Set_Position(Vector3::Lerp(m_CineDesc.targetPosition, prevPos, ratio));
			m_Transform->Set_Rotation(Quaternion::Slerp(qTarget, qPrev, ratio));
			m_FovY = XMConvertToRadians(m_CineDesc.targetFov) + (prevFov - XMConvertToRadians(m_CineDesc.targetFov)) * ratio;
		}
		return;
	}

	// 4. 시네마틱 종료
	GAME_INSTANCE->Set_MainCamera(m_PrevCamera);

	m_IsInvoked = false;
	m_AccumulatedTime = 0.f;
	m_PrevCamera = nullptr;
}

void CinematicCamera::InvokeCinematic()
{
	m_PrevCamera = GAME_INSTANCE->Get_MainCamera();

	m_IsInvoked = true;
	m_AccumulatedTime = 0.f;

	// 시작 시점의 FOV 갱신
	m_InitialFov = m_FovY;

	GAME_INSTANCE->Set_MainCamera(static_pointer_cast<Camera>(shared_from_this()));
}

Shared<CinematicCamera> CinematicCamera::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<CinematicCamera>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CinematicCamera");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> CinematicCamera::Clone(void* arg)
{
	auto instance = make_shared<CinematicCamera>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : CinematicCamera");
		return nullptr;
	}

	return instance;
}
