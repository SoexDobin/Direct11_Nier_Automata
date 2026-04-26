#include "pch.h"
#include "ThirdPersonCamera.h"
#include <SpdLogger.h>

#include "ClientSettingManager.h"
#include "Game.h"

ThirdPersonCamera::ThirdPersonCamera() : Camera{} {}
ThirdPersonCamera::ThirdPersonCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Camera{device, context} {}
ThirdPersonCamera::ThirdPersonCamera(const ThirdPersonCamera& rhs)
	: Camera{rhs} {}

HRESULT ThirdPersonCamera::Initialize_Prototype()
{
	return Camera::Initialize_Prototype();
}

HRESULT ThirdPersonCamera::Initialize(void* arg)
{
	THIRD_PERSON_CAMERA_DESC localDesc{};
	localDesc.eye = Vector4{ 0.f, 10.f, -10.f, 1.f };
	localDesc.at = Vector4{ 0.f, 0.f, 0.f, 1.f };
	localDesc.up = Vector4{ 0.f, 1.f, 0.f, 1.f };
	localDesc.fovY = XMConvertToRadians(60.f);
	localDesc.aspect = static_cast<Float>(ClientSettingManager::g_EngineDesc.viewportWidth) / static_cast<Float>(ClientSettingManager::g_EngineDesc.viewportHeight);
	localDesc.nearPlane = 0.1f;
	localDesc.farPlane = 250.f;
	
	localDesc.distance = 4.f;
	localDesc.minDistance = 3.f;
	localDesc.maxDistance = 30.f;
	localDesc.offset = Vector3{ 0.f, 1.f, 0.f };
	localDesc.mouseSensitive = 0.1f;
	localDesc.wheelSensitive = 0.01f;

	if (FAILED(Camera::Initialize(&localDesc)))
		return E_FAIL;

	m_ObjectDesc = static_cast<OBJECT_DESC*>(arg);

	m_Distance = localDesc.distance;
	m_TargetDistance = localDesc.distance;

	if (localDesc.minDistance == 0.f)
		m_MinDistance = m_TargetDistance;
	else
		m_MinDistance = localDesc.minDistance;
	if (localDesc.maxDistance == 0.f)
		m_MaxDistance = m_TargetDistance;
	else
		m_MaxDistance = localDesc.maxDistance;
	m_Offset = localDesc.offset;

	Vector3 angles = m_Transform->Get_Rotation();
	m_OrbitX = XMConvertToDegrees(angles.y);
	m_OrbitY = XMConvertToDegrees(angles.x);

	m_MouseSensitive = localDesc.mouseSensitive;
	m_WheelSensitive = localDesc.wheelSensitive;

	m_Friction = 0.8f;
	m_OrbitVelocityX = 0.f;
	m_OrbitVelocityY = 0.f;

	if (FAILED(GAME_INSTANCE->Set_MainCamera(static_pointer_cast<Camera>(shared_from_this()))))
		return E_FAIL;

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

		if (GAME_INSTANCE->Get_DIKeyState(static_cast<uByte>(DIKEYBOARD_ESCAPE)) & 0x80)
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
