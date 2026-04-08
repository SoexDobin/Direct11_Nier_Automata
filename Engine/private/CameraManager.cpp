#include "CameraManager.h"
#include "Game.h"
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
	m_LevCount = *static_cast<uint32*>(arg);
	m_Cameras.resize(m_LevCount);

	return EngineManager::Initialize(arg);
}

HRESULT CameraManager::Add_Camera(uint32 levIndex, const Shared<Camera>& camera)
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

	for (auto cameraPtr : m_Cameras[levIndex])
	{
		if (cameraPtr->Get_ObjectID() == camera->Get_ObjectID())
		{
			LOG_ERROR(L"Already Added Camera");
			return E_FAIL;
		}
	}

	m_Cameras[levIndex].push_back(camera);

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

vector<Shared<Camera>> CameraManager::Get_Cameras(uint32 levIndex)
{
	return m_Cameras[levIndex];
}

HRESULT CameraManager::Set_MainCamera(const Shared<Camera>& camera)
{
	std::erase_if(m_Cameras[GAME_INSTANCE->Get_CurrentLevelIndex()], [](auto& cam) -> Bool { return cam->Is_Destroy(); });

	if (camera == nullptr)
	{
		LOG_WARN("nullptr GameCamera");
		return E_FAIL;
	}

	m_MainCamera = camera;

	GAME_INSTANCE->Update_CameraPipeline();

	return S_OK;
}

void CameraManager::Bind_MainCamera_Transform()
{
	std::erase_if(m_Cameras[GAME_INSTANCE->Get_CurrentLevelIndex()], [](auto& cam) -> Bool { return cam->Is_Destroy(); });

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

HRESULT CameraManager::Clear_Cameras(uint32 levIndex)
{
	m_Cameras[levIndex].shrink_to_fit();
	m_Cameras[levIndex].clear();

	return S_OK;
}
HRESULT CameraManager::Clear_AllCameras()
{
	for (auto& camerasInLevel : m_Cameras)
	{
		camerasInLevel.clear();
		camerasInLevel.shrink_to_fit();
	}
	m_MainCamera.reset();

	return S_OK;
}

Unique<CameraManager> CameraManager::Create(uint32 levCount)
{
	auto cameraManager = make_unique<CameraManager>();

	if (FAILED(cameraManager->Initialize(&levCount)))
	{
		LOG_ERROR(L"Failed to Create CameraManager");
		return nullptr;
	}

	return cameraManager;
}
