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
	if (nullptr == arg)
	{
		LOG_ERROR(L"Need Camera_Desc to Clone Camera");
		return E_FAIL;
	}
	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;

	m_ObjectDesc = static_cast<OBJECT_DESC*>(arg);
	CAMERA_DESC& desc = *static_cast<CAMERA_DESC*>(m_ObjectDesc);
	m_Transform->Set_Position(Vector3{ desc.eye });
	m_Transform->LookAt(Vector3{ desc.at });

	m_FovY = desc.fovY;
	m_Aspect = desc.aspect;
	m_Near = desc.nearPlane;
	m_Far = desc.farPlane;

	Update_CameraTransform(0.f);
	GAME_INSTANCE->Add_Camera(static_pointer_cast<Camera>(shared_from_this()));
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
	Matrix projMat = XMMatrixPerspectiveFovLH(
		m_FovY,
		m_Aspect,
		m_Near,
		m_Far
	);

	GAME_INSTANCE->Set_Transform(D3DTS::VIEW, viewMat);
	GAME_INSTANCE->Set_Transform(D3DTS::PROJ, projMat);
}
