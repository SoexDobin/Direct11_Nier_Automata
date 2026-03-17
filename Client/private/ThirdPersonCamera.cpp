#include "pch.h"
#include "ThirdPersonCamera.h"
#include <SpdLogger.h>
#include "Game.h"

ThirdPersonCamera::ThirdPersonCamera() : Camera{} {}
ThirdPersonCamera::ThirdPersonCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Camera{device, context} {}
ThirdPersonCamera::ThirdPersonCamera(const ThirdPersonCamera& rhs)
	: Camera{rhs} {}

void ThirdPersonCamera::Set_Target(const Shared<GameObject>& target)
{
	m_Target = target;
	Update_CameraTransform(0.f);
}

Shared<GameObject> ThirdPersonCamera::Get_Target() const
{
	if (m_Target.expired())
	{
		return m_Target.lock();
	}
		
	LOG_ERROR(L"{} No Target", Get_Name());
	return nullptr;
}

HRESULT ThirdPersonCamera::Initialize_Prototype()
{
	return Camera::Initialize_Prototype();
}

HRESULT ThirdPersonCamera::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"Need Third_Person_Camera_Desc to Clone Camera");
		return E_FAIL;
	}
	if (FAILED(Camera::Initialize(arg)))
		return E_FAIL;

	m_ObjectDesc = static_cast<OBJECT_DESC*>(arg);
	THIRD_PERSON_CAMERA_DESC& desc = *static_cast<THIRD_PERSON_CAMERA_DESC*>(m_ObjectDesc);

	m_Distance = desc.distance;
	m_TargetDistance = desc.distance;

	if (desc.minDistance == 0.f)
		m_MinDistance = m_TargetDistance;
	else
		m_MinDistance = desc.minDistance;
	if (desc.maxDistance == 0.f)
		m_MaxDistance = m_TargetDistance;
	else
		m_MaxDistance = desc.maxDistance;
	m_Offset = desc.offset;

	Vector3 angles = m_Transform->Get_Rotation();
	m_OrbitX = XMConvertToDegrees(angles.y);
	m_OrbitY = XMConvertToDegrees(angles.x);

	m_MouseSensitive = desc.mouseSensitive;
	m_WheelSensitive = desc.wheelSensitive;

	m_Friction = 0.9f;
	m_OrbitVelocityX = 0.f;
	m_OrbitVelocityY = 0.f;

	return S_OK;
}

void ThirdPersonCamera::On_Destroy()
{
	Camera::On_Destroy();
}

void ThirdPersonCamera::Priority_Update(Float timeDelta)
{
	if (!m_Target.expired())
	{
		auto targetTransform = m_Target.lock()->Get_Transform();

		if (GAME_INSTANCE->Get_DIKeyState(DIKEYBOARD_ESCAPE) & 0x80)
			GAME_INSTANCE->Set_MouseLock(false);

		if (GAME_INSTANCE->Get_DIMouseState(DIMB::LBUTTON) & 0x80)
			GAME_INSTANCE->Set_MouseLock(true);

		m_OrbitVelocityX += m_MouseSensitive * GAME_INSTANCE->Get_DIMouseMove(DIMM::X);
		m_OrbitVelocityY += m_MouseSensitive * GAME_INSTANCE->Get_DIMouseMove(DIMM::Y);
		
		Float frictionFactor = pow(m_Friction, timeDelta * 60.f);
		m_OrbitVelocityX *= frictionFactor;
		m_OrbitVelocityY *= frictionFactor;

		m_OrbitX += m_OrbitVelocityX * timeDelta * 60.f;
		m_OrbitY += m_OrbitVelocityY * timeDelta * 60.f;
		m_OrbitY = clamp(m_OrbitY, -30.f, 60.f);

		Long mouseWheel = GAME_INSTANCE->Get_DIMouseMove(DIMM::WHEEL);
		if (mouseWheel != 0) {
			m_Distance -= m_WheelSensitive * static_cast<Float>(mouseWheel); // 0.01f는 줌 속도 조절용
			m_Distance = clamp(m_Distance, m_MinDistance, m_MaxDistance);
		}

		Quaternion mouseRot = Quaternion::CreateFromYawPitchRoll(
			XMConvertToRadians(m_OrbitX),
			XMConvertToRadians(m_OrbitY),
			0.f
		);
		Quaternion targetRot = mouseRot;
		
		Vector3 pivotPos = targetTransform->Get_Position() + Vector3::Transform(m_Offset, targetTransform->Get_Quaternion());
		Vector3 lookDir = Vector3::TransformNormal(Vector3(0, 0, -1), Matrix::CreateFromQuaternion(targetRot));
		Vector3 targetPos = pivotPos + lookDir * m_Distance;

		m_Transform->Set_Position(Vector3::Lerp(m_Transform->Get_Position(), targetPos, 0.3f));
		m_Transform->Set_Rotation(targetRot);
	}

	Camera::Priority_Update(timeDelta);
}

void ThirdPersonCamera::Update(Float timeDelta)
{
	
}

void ThirdPersonCamera::Late_Update(Float timeDelta)
{
	
}

void ThirdPersonCamera::Fixed_Update(Float fixedDelta)
{
	
}

Shared<ThirdPersonCamera> ThirdPersonCamera::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto thirdPersonCamera = make_shared<ThirdPersonCamera>(device, context);

	if (FAILED(thirdPersonCamera->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : ThirdPersonCamera");
		return nullptr;
	}

	return thirdPersonCamera;
}

Shared<GameObject> ThirdPersonCamera::Clone(void* arg)
{
	auto thirdPersonCamera = make_shared<ThirdPersonCamera>(*this);

	if (FAILED(thirdPersonCamera->Initialize(arg)))
	{
		MSG_BOX("Failed to CreateComponent : ThirdPersonCamera");
		return nullptr;
	}

	return thirdPersonCamera;
}
