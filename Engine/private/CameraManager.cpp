#include "CameraManager.h"

#include "Camera.h"
#include "SpdLogger.h"
#include "Transform.h"

CameraManager::CameraManager()
	: EngineManager{} {}

HRESULT CameraManager::Initialize_Prototype()
{
	return EngineManager::Initialize_Prototype();
}

HRESULT CameraManager::Initialize(void* arg)
{
	return EngineManager::Initialize(arg);
}

HRESULT CameraManager::Add_Camera(const Shared<Camera>& camera)
{
	if (camera->Get_ObjectID() <= 0)
	{
		LOG_ERROR(L"Fail to Add Camera By ObjectID");
		return E_FAIL;
	}
	if (camera->Get_Name().empty())
	{
		LOG_ERROR(L"Fail to Add Camera By Empty Name");
		return E_FAIL;
	}

	for (Weak<Camera> cameraPtr : m_Cameras)
	{
		if (!cameraPtr.expired())
		{
			if (cameraPtr.lock()->Get_ObjectID() == 
				camera->Get_ObjectID())
			{
				LOG_ERROR(L"Already Added Camera");
				return E_FAIL;
			}
		}
	}

	m_Cameras.push_back(camera);

	if (m_MainCamera.expired())
		return Set_MainCamera(camera);

	return S_OK;
}

Shared<Camera> CameraManager::Get_MainCamera() const
{
	if (m_MainCamera.expired())
	{
		LOG_ERROR(L"There is no MainCamera");
		return nullptr;
	}

	return m_MainCamera.lock();
}

HRESULT CameraManager::Set_MainCamera(const Shared<Camera>& camera)
{
	if (camera == nullptr)
	{
		LOG_WARN("nullptr GameCamera");
		return E_FAIL;
	}

	m_MainCamera = camera;

	return S_OK;
}

void CameraManager::Bind_MainCamera_Transform()
{
	std::erase_if(m_Cameras, [](const std::weak_ptr<Camera>& cam) -> Bool { return cam.expired(); });

	if (auto mainCamera = m_MainCamera.lock())
	{
		const auto& worldMatrix = mainCamera->Get_Transform()->Get_WorldMatrix();

		Matrix viewMat = worldMatrix.Invert();
		Matrix projMat = XMMatrixPerspectiveFovLH(
			mainCamera->Get_FovY(),
			mainCamera->Get_Aspect(),
			mainCamera->Get_NearPlane(),
			mainCamera->Get_FarPlane()
		);

		GAME_INSTANCE->Set_Transform(D3DTS::VIEW, viewMat);
		GAME_INSTANCE->Set_Transform(D3DTS::PROJ, projMat);
	}
	else
		LOG_ERROR(L"There is no MainCamera");
}

HRESULT CameraManager::Clear_Cameras()
{
	m_Cameras.shrink_to_fit();
	m_Cameras.clear();
	m_MainCamera.reset();

	return S_OK;
}

Unique<CameraManager> CameraManager::Create()
{
	auto cameraManager = make_unique<CameraManager>();

	if (FAILED(cameraManager->Initialize()))
	{
		LOG_ERROR(L"Failed to Create CameraManager");
		return nullptr;
	}

	return cameraManager;
}
