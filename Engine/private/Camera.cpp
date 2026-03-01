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
		
	CAMERA_DESC& desc = *static_cast<CAMERA_DESC*>(arg);
	m_Transform->Set_Position(Vector3{ desc.eye });
	m_Transform->LookAt(Vector3{ desc.at });

	uint32 numViewports = { 1 };
	D3D11_VIEWPORT viewportDesc{};

	m_Context->RSGetViewports(&numViewports, &viewportDesc);

	m_FovY = desc.fovY;
	m_Aspect = viewportDesc.Width / viewportDesc.Height;
	m_Near = desc.nearPlane;
	m_Far = desc.farPlane;

	Update_CameraTransform();

	return S_OK;
}

void Camera::Update_CameraTransform() const
{
	const Matrix& cameraMatrix = m_Transform->Get_WorldMatrix();
	GAME_INSTANCE->Set_Transform(D3DTS::VIEW,
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&cameraMatrix)));

	GAME_INSTANCE->Set_Transform(D3DTS::PROJ,
		XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far));

	m_Transform->Update_WorldMatrix();
}
