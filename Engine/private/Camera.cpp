#include "Camera.h"

Camera::Camera() : GameObject{} {}
Camera::Camera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {}
Camera::Camera(const Shared<Camera>& rhs)
	: GameObject{rhs} {}

HRESULT Camera::Initialize_Prototype()
{

	return GameObject::Initialize_Prototype();
}

HRESULT Camera::Initialize(void* arg)
{

	return GameObject::Initialize(arg);
}

void Camera::Update_TransformMatrices()
{
}
