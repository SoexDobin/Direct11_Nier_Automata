#include "PhysicsManager.h"

#include "Game.h"
#include "Model.h"
#include "SpdLogger.h"
#include "String_Helper.h"

#include <physx/PxPhysicsAPI.h>

using namespace physx;

namespace
{
	/// PhysX는 자기 할당자와 에러 콜백을 요구한다. 기본 구현으로 충분하되,
	/// 에러는 엔진 로그로 넘겨서 조용히 사라지지 않게 한다.
	PxDefaultAllocator g_Allocator;

	class EngineErrorCallback final : public PxErrorCallback
	{
	public:
		void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			LOG_ERROR(L"[Physics] PhysX error {}: {} ({}:{})",
				static_cast<int>(code),
				Helper::To_wString(message ? message : ""),
				Helper::To_wString(file ? file : ""),
				line);
		}
	};

	EngineErrorCallback g_ErrorCallback;
}

PhysicsManager::PhysicsManager() {}

PhysicsManager::~PhysicsManager()
{
	Release_Physics();
}

HRESULT PhysicsManager::Initialize(void* arg)
{
	if (FAILED(EngineManager::Initialize(arg)))
		return E_FAIL;

	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_Allocator, g_ErrorCallback);
	if (nullptr == m_Foundation)
	{
		LOG_ERROR(L"[Physics] Failed to create PxFoundation");
		return E_FAIL;
	}

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale{});
	if (nullptr == m_Physics)
	{
		LOG_ERROR(L"[Physics] Failed to create PxPhysics");
		Release_Physics();
		return E_FAIL;
	}

	// 정적 지형만 올리는 단계라 워커는 최소로 둔다. 동적 강체가 생기면 그때 늘린다.
	m_Dispatcher = PxDefaultCpuDispatcherCreate(1);
	if (nullptr == m_Dispatcher)
	{
		LOG_ERROR(L"[Physics] Failed to create CPU dispatcher");
		Release_Physics();
		return E_FAIL;
	}

	PxSceneDesc sceneDesc{ m_Physics->getTolerancesScale() };
	// 중력은 Movement가 직접 적분한다. 씬 중력을 켜면 같은 힘이 두 번 들어간다.
	sceneDesc.gravity = PxVec3{ 0.f, 0.f, 0.f };
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_Scene = m_Physics->createScene(sceneDesc);
	if (nullptr == m_Scene)
	{
		LOG_ERROR(L"[Physics] Failed to create PxScene");
		Release_Physics();
		return E_FAIL;
	}

	m_DefaultMaterial = m_Physics->createMaterial(0.5f, 0.5f, 0.f);
	if (nullptr == m_DefaultMaterial)
	{
		LOG_ERROR(L"[Physics] Failed to create the default material");
		Release_Physics();
		return E_FAIL;
	}

	m_ControllerManager = PxCreateControllerManager(*m_Scene);
	if (nullptr == m_ControllerManager)
	{
		LOG_ERROR(L"[Physics] Failed to create PxControllerManager");
		Release_Physics();
		return E_FAIL;
	}

	LOG_INFO(L"[Physics] PhysX ready");
	return S_OK;
}

void PhysicsManager::Step(Float fixedDelta)
{
	if (!Is_Active() || nullptr == m_Scene || fixedDelta <= 0.f)
		return;

	m_Scene->simulate(fixedDelta);
	m_Scene->fetchResults(true);
}

Bool PhysicsManager::Ensure_StaticMesh(const wstring& modelTag)
{
	if (m_StaticMeshes.contains(modelTag))
		return true;

	if (nullptr == m_Physics)
		return false;

	const int32 levelIndex = GAME_INSTANCE->Get_ContainLevelByModelTag(modelTag);
	if (levelIndex < 0)
	{
		LOG_ERROR(L"[Physics] No level contains model {}", modelTag);
		return false;
	}

	const Shared<Model> model = GAME_INSTANCE->Get_Model(levelIndex, modelTag.c_str());
	if (nullptr == model)
	{
		LOG_ERROR(L"[Physics] Model prototype {} not found", modelTag);
		return false;
	}

	// 여기서 나오는 좌표에는 ModelSettings 사전 변환이 이미 적용돼 있다. 다시 변환하지 않는다.
	vector<Float> positions;
	vector<int32> indices;
	model->Extract_RawMeshData(positions, indices);

	if (positions.empty() || indices.empty())
	{
		LOG_ERROR(L"[Physics] Model {} has no geometry to cook", modelTag);
		return false;
	}

	PxTriangleMeshDesc meshDesc{};
	meshDesc.points.count = static_cast<PxU32>(positions.size() / 3);
	meshDesc.points.stride = sizeof(Float) * 3;
	meshDesc.points.data = positions.data();
	meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
	meshDesc.triangles.stride = sizeof(int32) * 3;
	meshDesc.triangles.data = indices.data();

	if (!meshDesc.isValid())
	{
		LOG_ERROR(L"[Physics] Triangle mesh desc for {} is invalid", modelTag);
		return false;
	}

	const PxCookingParams cookingParams{ m_Physics->getTolerancesScale() };
	PxTriangleMesh* mesh = PxCreateTriangleMesh(cookingParams, meshDesc,
		m_Physics->getPhysicsInsertionCallback());

	if (nullptr == mesh)
	{
		LOG_ERROR(L"[Physics] Failed to cook {} ({} triangles)", modelTag, meshDesc.triangles.count);
		return false;
	}

	m_StaticMeshes.emplace(modelTag, mesh);
	LOG_INFO(L"[Physics] Cooked {} : {} vertices, {} triangles",
		modelTag, meshDesc.points.count, meshDesc.triangles.count);
	return true;
}

uint32 PhysicsManager::Add_StaticActor(const wstring& modelTag, const Matrix& worldMatrix)
{
	if (nullptr == m_Scene || !Ensure_StaticMesh(modelTag))
		return 0;

	Vector3 scale{}, translation{};
	Quaternion rotation{};
	Matrix decomposable = worldMatrix;   // SimpleMath의 Decompose는 const가 아니다.
	if (!decomposable.Decompose(scale, rotation, translation))
	{
		LOG_ERROR(L"[Physics] World matrix for {} is not decomposable", modelTag);
		return 0;
	}

	const PxTransform pose{
		PxVec3{ translation.x, translation.y, translation.z },
		PxQuat{ rotation.x, rotation.y, rotation.z, rotation.w } };

	// PxTransform은 스케일을 담지 못한다. 스케일은 지오메트리 쪽으로 넣는다.
	const PxTriangleMeshGeometry geometry{
		m_StaticMeshes.at(modelTag),
		PxMeshScale{ PxVec3{ scale.x, scale.y, scale.z } } };

	PxRigidStatic* actor = m_Physics->createRigidStatic(pose);
	if (nullptr == actor)
	{
		LOG_ERROR(L"[Physics] Failed to create static actor for {}", modelTag);
		return 0;
	}

	if (nullptr == PxRigidActorExt::createExclusiveShape(*actor, geometry, *m_DefaultMaterial))
	{
		actor->release();
		LOG_ERROR(L"[Physics] Failed to create shape for {}", modelTag);
		return 0;
	}

	m_Scene->addActor(*actor);

	const uint32 handle = m_NextActorHandle++;
	m_StaticActors.emplace(handle, actor);

	LOG_INFO(L"[Physics] Static actor {} registered for {} at ({:.2f}, {:.2f}, {:.2f})",
		handle, modelTag, translation.x, translation.y, translation.z);
	return handle;
}

void PhysicsManager::Remove_StaticActor(uint32 handle)
{
	const auto found = m_StaticActors.find(handle);
	if (found == m_StaticActors.end())
		return;

	if (found->second)
	{
		if (m_Scene)
			m_Scene->removeActor(*found->second);
		found->second->release();
	}

	m_StaticActors.erase(found);
}

Bool PhysicsManager::Raycast(const Vector3& origin, const Vector3& direction,
	Float distance, Vector3& outHit) const
{
	if (nullptr == m_Scene || distance <= 0.f)
		return false;

	Vector3 unitDirection = direction;
	unitDirection.Normalize();

	PxRaycastBuffer hit;
	// eMESH_BOTH_SIDES: 삼각형 winding이 뒤집혀 있어도 맞힌다. DirectX(왼손)와
	// PhysX(오른손)가 만나는 지점이라, 지형이 있는데 레이가 통과하는 사고를 막는다.
	const PxHitFlags hitFlags = PxHitFlag::eDEFAULT | PxHitFlag::eMESH_BOTH_SIDES;

	if (!m_Scene->raycast(
		PxVec3{ origin.x, origin.y, origin.z },
		PxVec3{ unitDirection.x, unitDirection.y, unitDirection.z },
		distance, hit, hitFlags))
		return false;

	if (!hit.hasBlock)
		return false;

	outHit = Vector3{ hit.block.position.x, hit.block.position.y, hit.block.position.z };
	return true;
}

uint32 PhysicsManager::Create_Controller(const CHARACTER_CONTROLLER_DESC& desc, const Vector3& footPosition)
{
	if (nullptr == m_ControllerManager || nullptr == m_DefaultMaterial)
		return 0;

	PxCapsuleControllerDesc capsuleDesc{};
	capsuleDesc.radius = desc.radius;
	capsuleDesc.height = desc.height;
	capsuleDesc.upDirection = PxVec3{ 0.f, 1.f, 0.f };
	capsuleDesc.slopeLimit = cosf(XMConvertToRadians(desc.slopeLimitDegrees));
	capsuleDesc.stepOffset = desc.stepOffset;
	capsuleDesc.contactOffset = desc.contactOffset;
	capsuleDesc.material = m_DefaultMaterial;
	capsuleDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	// desc.position은 캡슐 중심이다. 생성 후 발 위치로 다시 맞추므로 여기서는 대략만 둔다.
	capsuleDesc.position = PxExtendedVec3{ footPosition.x, footPosition.y + desc.radius + desc.height * 0.5f, footPosition.z };

	if (!capsuleDesc.isValid())
	{
		LOG_ERROR(L"[Physics] Character controller desc is invalid (radius {:.2f}, height {:.2f}, step {:.2f})",
			desc.radius, desc.height, desc.stepOffset);
		return 0;
	}

	PxController* controller = m_ControllerManager->createController(capsuleDesc);
	if (nullptr == controller)
	{
		LOG_ERROR(L"[Physics] Failed to create character controller");
		return 0;
	}

	controller->setFootPosition(PxExtendedVec3{ footPosition.x, footPosition.y, footPosition.z });

	const uint32 handle = m_NextActorHandle++;
	m_Controllers.emplace(handle, controller);

	LOG_INFO(L"[Physics] Character controller {} at foot ({:.2f}, {:.2f}, {:.2f}), radius {:.2f}, total height {:.2f}",
		handle, footPosition.x, footPosition.y, footPosition.z, desc.radius, desc.height + desc.radius * 2.f);
	return handle;
}

uint32 PhysicsManager::Move_Controller(uint32 handle, const Vector3& displacement, Float timeDelta)
{
	const auto found = m_Controllers.find(handle);
	if (found == m_Controllers.end() || nullptr == found->second)
		return 0;

	const PxControllerFilters filters{};
	const PxControllerCollisionFlags flags = found->second->move(
		PxVec3{ displacement.x, displacement.y, displacement.z }, 0.001f, timeDelta, filters);

	uint32 result = 0;
	if (flags & PxControllerCollisionFlag::eCOLLISION_SIDES) result |= CONTROLLER_COLLISION_SIDES;
	if (flags & PxControllerCollisionFlag::eCOLLISION_UP)    result |= CONTROLLER_COLLISION_UP;
	if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)  result |= CONTROLLER_COLLISION_DOWN;
	return result;
}

void PhysicsManager::Set_ControllerFootPosition(uint32 handle, const Vector3& footPosition)
{
	const auto found = m_Controllers.find(handle);
	if (found == m_Controllers.end() || nullptr == found->second)
		return;

	found->second->setFootPosition(PxExtendedVec3{ footPosition.x, footPosition.y, footPosition.z });
}

Vector3 PhysicsManager::Get_ControllerFootPosition(uint32 handle) const
{
	const auto found = m_Controllers.find(handle);
	if (found == m_Controllers.end() || nullptr == found->second)
		return Vector3::Zero;

	const PxExtendedVec3 foot = found->second->getFootPosition();
	return Vector3{ static_cast<Float>(foot.x), static_cast<Float>(foot.y), static_cast<Float>(foot.z) };
}

void PhysicsManager::Release_Controller(uint32 handle)
{
	const auto found = m_Controllers.find(handle);
	if (found == m_Controllers.end())
		return;

	if (found->second)
		found->second->release();
	m_Controllers.erase(found);
}

void PhysicsManager::Release_Physics()
{
	// 컨트롤러는 자기 캡슐 액터를 씬에 두고 있으므로 가장 먼저 정리한다.
	for (auto& [handle, controller] : m_Controllers)
		if (controller) controller->release();
	m_Controllers.clear();

	// 액터와 메시를 씬보다 먼저 정리한다. 순서가 뒤집히면 종료 시 죽는다.
	for (auto& [handle, actor] : m_StaticActors)
	{
		if (!actor) continue;
		if (m_Scene)
			m_Scene->removeActor(*actor);
		actor->release();
	}
	m_StaticActors.clear();

	for (auto& [tag, mesh] : m_StaticMeshes)
		if (mesh) mesh->release();
	m_StaticMeshes.clear();

	// 생성의 역순으로 해제한다. PhysX는 소유 관계를 직접 관리하므로 순서가 곧 계약이다.
	if (m_ControllerManager) { m_ControllerManager->release(); m_ControllerManager = nullptr; }
	if (m_DefaultMaterial) { m_DefaultMaterial->release(); m_DefaultMaterial = nullptr; }
	if (m_Scene) { m_Scene->release(); m_Scene = nullptr; }
	if (m_Dispatcher) { m_Dispatcher->release(); m_Dispatcher = nullptr; }
	if (m_Physics) { m_Physics->release(); m_Physics = nullptr; }
	if (m_Foundation) { m_Foundation->release(); m_Foundation = nullptr; }
}

void PhysicsManager::On_Destroy() { Release_Physics(); }
void PhysicsManager::On_Disable() { EngineManager::On_Disable(); }
void PhysicsManager::On_Enable() { EngineManager::On_Enable(); }
void PhysicsManager::Set_Active(Bool isActive) { EngineManager::Set_Active(isActive); }

Unique<PhysicsManager> PhysicsManager::Create()
{
	auto manager = make_unique<PhysicsManager>();

	if (FAILED(manager->Initialize(nullptr)))
	{
		LOG_ERROR(L"[Physics] Failed to Initialize PhysicsManager");
		return nullptr;
	}

	return manager;
}
