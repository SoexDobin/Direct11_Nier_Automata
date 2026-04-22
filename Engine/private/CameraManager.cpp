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
		Update_Frustum();
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

Bool CameraManager::IsInFrustum(const BoundingSphere& worldSphere)
{
	for (int i = 0; i < 6; ++i)
	{
		Float dotProduct =
			worldSphere.Center.x * m_FrustumPlanes[i].x +
			worldSphere.Center.y * m_FrustumPlanes[i].y +
			worldSphere.Center.z * m_FrustumPlanes[i].z +
			m_FrustumPlanes[i].w;
		// 평면으로부터 구의 중심까지의 거리가 반경의 음수값보다 작으면 완전히 뒤(화면 밖)에 있는 것
		if (dotProduct < -worldSphere.Radius)
		{
			return false; // 컬링!
		}
	}
	return true; // 렌더링
}

void CameraManager::Update_Frustum()
{
	BoundingFrustum localFrustum{};
	BoundingFrustum::CreateFromMatrix(localFrustum, GAME_INSTANCE->Get_Transform(D3DTS::PROJ));

	if (auto mainCamera = m_MainCamera.lock())
	{
		localFrustum.Transform(m_Frustum, mainCamera->Get_Transform()->Get_WorldMatrix());
	}

	Matrix viewMat = GAME_INSTANCE->Get_Transform(D3DTS::VIEW);
	Matrix projMat = GAME_INSTANCE->Get_Transform(D3DTS::PROJ);
	Matrix vp = viewMat * projMat;

	m_FrustumPlanes[0] = Vector4{ vp._14 + vp._11, vp._24 + vp._21, vp._34 + vp._31, vp._44 + vp._41 }; // Left
	m_FrustumPlanes[1] = Vector4{ vp._14 - vp._11, vp._24 - vp._21, vp._34 - vp._31, vp._44 - vp._41 }; // Right
	m_FrustumPlanes[2] = Vector4{ vp._14 + vp._12, vp._24 + vp._22, vp._34 + vp._32, vp._44 + vp._42 }; // Top
	m_FrustumPlanes[3] = Vector4{ vp._14 - vp._12, vp._24 - vp._22, vp._34 - vp._32, vp._44 - vp._42 }; // Bottom
	m_FrustumPlanes[4] = Vector4{ vp._14 + vp._13, vp._24 + vp._23, vp._34 + vp._33, vp._44 + vp._43 }; // Near
	m_FrustumPlanes[5] = Vector4{ vp._14 - vp._13, vp._24 - vp._23, vp._34 - vp._33, vp._44 - vp._43 }; // Far

	for (uint32 i = 0; i < 6; ++i)
	{
		Float length = sqrtf(
			m_FrustumPlanes[i].x * m_FrustumPlanes[i].x +
			m_FrustumPlanes[i].y * m_FrustumPlanes[i].y +
			m_FrustumPlanes[i].z * m_FrustumPlanes[i].z
		);
		m_FrustumPlanes[i].x /= length;
		m_FrustumPlanes[i].y /= length;
		m_FrustumPlanes[i].z /= length;
		m_FrustumPlanes[i].w /= length;
	}

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
