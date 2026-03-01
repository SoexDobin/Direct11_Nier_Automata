#include "pch.h"
#include "FreeCamera.h"

#include <SpdLogger.h>
#include <Transform.h>

FreeCamera::FreeCamera() : Camera() {}
FreeCamera::FreeCamera(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Camera{ device, context } {}
FreeCamera::FreeCamera(const FreeCamera& rhs)
	: Camera{ rhs } {}

HRESULT FreeCamera::Initialize_Prototype()
{
	return Camera::Initialize_Prototype();
}

HRESULT FreeCamera::Initialize(void* arg)
{
	if (arg == nullptr)
		LOG_ERROR(L"Failed to Init FreeCamera No CameraDesc");

	FREE_CAMERA_DESC& desc = *static_cast<FREE_CAMERA_DESC*>(arg);
	m_MouseSensitive = desc.mouseSensitive;

	return Camera::Initialize(arg);
}

void FreeCamera::Priority_Update(Float timeDelta)
{
	if (GAME_INSTANCE->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_Transform->Move_Forward(timeDelta * 10.f);
		LOG_INFO(L"POS {} {} {}", m_Transform->Get_Position().x, m_Transform->Get_Position().y, m_Transform->Get_Position().z);
		LOG_INFO(L"ROT {} {} {}", m_Transform->Get_Rotation().x, m_Transform->Get_Rotation().y, m_Transform->Get_Rotation().z);
	}

	if (GAME_INSTANCE->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_Transform->Move_Backward(timeDelta * 10.f);
		LOG_INFO(L"POS {} {} {}", m_Transform->Get_Position().x, m_Transform->Get_Position().y, m_Transform->Get_Position().z);
		LOG_INFO(L"ROT {} {} {}", m_Transform->Get_Rotation().x, m_Transform->Get_Rotation().y, m_Transform->Get_Rotation().z);
	}

	if (GAME_INSTANCE->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_Transform->Move_Left(timeDelta * 10.f);
		LOG_INFO(L"POS {} {} {}", m_Transform->Get_Position().x, m_Transform->Get_Position().y, m_Transform->Get_Position().z);
		LOG_INFO(L"ROT {} {} {}", m_Transform->Get_Rotation().x, m_Transform->Get_Rotation().y, m_Transform->Get_Rotation().z);
	}

	if (GAME_INSTANCE->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_Transform->Move_Right(timeDelta * 10.f);
		LOG_INFO(L"POS {} {} {}", m_Transform->Get_Position().x, m_Transform->Get_Position().y, m_Transform->Get_Position().z);
		LOG_INFO(L"ROT {} {} {}", m_Transform->Get_Rotation().x, m_Transform->Get_Rotation().y, m_Transform->Get_Rotation().z);
	}

	Long		mouseMove{};

	if ((mouseMove = GAME_INSTANCE->Get_DIMouseMove(DIMM::X)))
	{
		m_Transform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), timeDelta * mouseMove * m_MouseSensitive);
		LOG_INFO(L"POS {} {} {}", m_Transform->Get_Position().x, m_Transform->Get_Position().y, m_Transform->Get_Position().z);
		LOG_INFO(L"ROT {} {} {}", m_Transform->Get_Rotation().x, m_Transform->Get_Rotation().y, m_Transform->Get_Rotation().z);
	}

	if ((mouseMove = GAME_INSTANCE->Get_DIMouseMove(DIMM::Y)))
	{
		m_Transform->Rotate(XMLoadFloat4(reinterpret_cast<Float4*>(&m_Transform->Get_WorldMatrix().m[0][0])), timeDelta * mouseMove * m_MouseSensitive);
		LOG_INFO(L"POS {} {} {}", m_Transform->Get_Position().x, m_Transform->Get_Position().y, m_Transform->Get_Position().z);
		LOG_INFO(L"ROT {} {} {}", m_Transform->Get_Rotation().x, m_Transform->Get_Rotation().y, m_Transform->Get_Rotation().z);
	}

	Camera::Priority_Update(timeDelta);
}

void FreeCamera::Update(Float timeDelta) {}
void FreeCamera::Late_Update(Float timeDelta) {}
void FreeCamera::Fixed_Update(Float fixedDelta){}
HRESULT FreeCamera::Render() { return S_OK; }

Shared<FreeCamera> FreeCamera::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto camera = make_shared<FreeCamera>(device, context);

	if (FAILED(camera->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : FreeCamera");
		return nullptr;
	}

	return camera;
}

Shared<GameObject> FreeCamera::Clone(void* arg)
{
	auto camera = make_shared<FreeCamera>(*this);

	if (FAILED(camera->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : FreeCamera");
		return nullptr;
	}

	return camera;
}
