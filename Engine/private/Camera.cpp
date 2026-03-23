#include "Camera.h"
#include "Game.h"
#include "Transform.h"
#include "SpdLogger.h"

Camera::Camera() : GameObject{} {}
Camera::Camera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {}
Camera::Camera(const Camera& rhs)
	: GameObject{ rhs }, m_FovY{rhs.m_FovY}, m_Aspect{rhs.m_Aspect}, m_Near{rhs.m_Near}, m_Far{rhs.m_Far} {}

HRESULT Camera::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT Camera::Initialize(void* arg)
{
	CAMERA_DESC localDesc{};
	if (nullptr == arg)
	{
		localDesc.eye = Vector4{ 0.f, 5.f, -10.f, 1.f };
		localDesc.at = Vector4{ 0.f, 0.f, 0.f, 1.f };
		localDesc.up = Vector4{ 0.f, 1.f, 0.f, 0.f };
		localDesc.fovY = XMConvertToRadians(60.0f);
		localDesc.aspect = 1.6f;
		localDesc.nearPlane = 0.1f;
		localDesc.farPlane = 1000.f;
		arg = &localDesc;
	}

	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;

	m_ObjectDesc = static_pointer_cast<OBJECT_DESC>(make_shared<CAMERA_DESC>(*static_cast<CAMERA_DESC*>(arg))).get();
	CAMERA_DESC& desc = *static_cast<CAMERA_DESC*>(arg);
	m_Transform->Set_Position(Vector3{ desc.eye });
	m_Transform->LookAt(Vector3{ desc.at });

	m_FovY = desc.fovY;
	m_Aspect = desc.aspect;
	m_Near = desc.nearPlane;
	m_Far = desc.farPlane;

	Update_CameraTransform(0.f);

	return S_OK;
}

void Camera::Set_Aspect(Float aspect)
{
	m_Aspect = aspect;
}

void Camera::Bind_Aspect(Float aspect)
{
	m_Aspect = aspect;
	Bind_CameraTransform();
}

void Camera::Set_Target(const Shared<GameObject>& target)
{
	m_Target = target;
	m_TargetID = m_Target.lock() ? m_Target.lock()->Get_ObjectID() : 0;
	Update_CameraTransform(0.f);
}

Shared<GameObject> Camera::Get_Target() const
{
	if (!m_Target.expired())
	{
		return m_Target.lock();
	}

	return nullptr;
}

void Camera::Set_TargetID(uint32 targetID)
{
	m_TargetID = targetID;
	if (m_TargetID != 0)
	{
		// 1. 현재 레벨에서 검색
		m_Target = GAME_INSTANCE->Find_ObjectByObjectID(GAME_INSTANCE->Get_CurrentLevelIndex(), m_TargetID);
		
		// 2. 못 찾았으면 Static 레벨(0)에서 검색
		if (m_Target.expired())
		{
			m_Target = GAME_INSTANCE->Find_ObjectByObjectID(0, m_TargetID);
		}

		if (const auto& pObj = m_Target.lock())
		{
			LOG_INFO(L"[Camera] TargetID {} assigned to object '{}'", m_TargetID, pObj->Get_Name());
		}
		else
		{
			LOG_WARN(L"[Camera] Failed to find object for TargetID {}", m_TargetID);
		}
	}
	else
	{
		m_Target.reset();
	}
	Update_CameraTransform(0.f);
}

void Camera::Priority_Update(Float timeDelta)
{
	Update_CameraTransform(timeDelta);
}

void Camera::Update_CameraTransform(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
}

void Camera::Bind_CameraTransform() const
{
	m_Transform->Update_WorldMatrix();
	const auto& worldMatrix = m_Transform->Get_WorldMatrix();

	Matrix viewMat = worldMatrix.Invert();

	Float fAspect = m_Aspect;
	if (XMScalarNearEqual(fAspect, 0.0f, 0.00001f))
	{
		fAspect = 0.001f;
	}

	Matrix projMat = XMMatrixPerspectiveFovLH(
		m_FovY,
		fAspect,
		m_Near,
		m_Far
	);

	GAME_INSTANCE->Set_Transform(D3DTS::VIEW, viewMat);
	GAME_INSTANCE->Set_Transform(D3DTS::PROJ, projMat);
}
