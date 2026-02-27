#include "pch.h"
#include "EditorCamera.h"

EditorCamera::EditorCamera() : Camera{} {}
EditorCamera::EditorCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Camera{ device, context } {}
EditorCamera::EditorCamera(const Shared<EditorCamera>& rhs)
	: Camera{rhs} {}

HRESULT EditorCamera::Initialize_Prototype()
{

	return Camera::Initialize_Prototype();
}

HRESULT EditorCamera::Initialize(void* arg)
{

	return Camera::Initialize(arg);
}

void EditorCamera::Priority_Update(Float timeDelta)
{
	Camera::Priority_Update(timeDelta);
}

void EditorCamera::Update(Float timeDelta)
{
	Camera::Update(timeDelta);
}

void EditorCamera::Late_Update(Float timeDelta)
{
	Camera::Late_Update(timeDelta);
}

void EditorCamera::Fixed_Update(Float fixedDelta)
{
	Camera::Fixed_Update(fixedDelta);
}

Shared<EditorCamera> EditorCamera::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, void* cameraDesc)
{
	auto editorCam = make_shared<EditorCamera>(device, context);

	if (FAILED(editorCam->Initialize(cameraDesc))) {
		LOG_ERROR(L"Failed to initialize EditorCamera");
		return nullptr;
	}

	return editorCam;
}
