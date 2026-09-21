#pragma once
#include "EngineManager.h"

/// PhysX 타입은 이 헤더로 새어나오지 않는다. Client/Editor는 Game 파사드만 쓰고
/// SDK 헤더를 직접 include 하지 않는다.
namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxDefaultCpuDispatcher;
	class PxScene;
	class PxMaterial;
	class PxControllerManager;
	class PxTriangleMesh;
	class PxRigidStatic;
	class PxController;
}

NS_BEGIN(Engine)

/// 캐릭터 컨트롤러 생성 인자. PhysX 타입 없이 Client가 채운다.
/// PhysX 캡슐의 전체 높이는 height + radius * 2 이다.
struct CHARACTER_CONTROLLER_DESC
{
	Float radius{ 0.35f };
	Float height{ 1.0f };
	Float slopeLimitDegrees{ 45.f };
	Float stepOffset{ 0.4f };
	Float contactOffset{ 0.1f };
};

/// Move 결과. 여러 비트가 동시에 설 수 있다.
enum CONTROLLER_COLLISION : uint32
{
	CONTROLLER_COLLISION_SIDES = 1u << 0,
	CONTROLLER_COLLISION_UP    = 1u << 1,
	CONTROLLER_COLLISION_DOWN  = 1u << 2,
};

/// 월드 충돌과 캐릭터 이동을 소유한다. 현재 단계는 정적 씬만 만들고 고정 스텝에서 돌린다.
class PhysicsManager final : public EngineManager
{
	NO_COPY(PhysicsManager)
public:
	explicit PhysicsManager();
	~PhysicsManager() override;

public:
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;
	void Set_Active(Bool isActive) override;

public:
	/// 고정 스텝 1회. 씬이 없거나 비활성이면 아무것도 하지 않는다.
	void Step(Float fixedDelta);

	Bool Is_Ready() const { return nullptr != m_Scene; }

public:
	/// 모델 태그의 지오메트리를 구워 캐시에 넣는다. 이미 있으면 그대로 쓴다.
	Bool Ensure_StaticMesh(const wstring& modelTag);

	/// 구운 메시를 월드 행렬 자리에 정적 액터로 올린다. 0이면 실패.
	uint32 Add_StaticActor(const wstring& modelTag, const Matrix& worldMatrix);
	void   Remove_StaticActor(uint32 handle);

	/// 지면 확인용. 나중에 카메라 충돌에도 쓴다.
	Bool Raycast(const Vector3& origin, const Vector3& direction, Float distance,
		_Out_ Vector3& outHit) const;

	/// 정적 충돌이 하나라도 올라와 있는지. 없으면 캐릭터 컨트롤러를 써도 설 곳이 없다.
	Bool Has_StaticCollision() const { return !m_StaticActors.empty(); }

public:
	/// 발 위치에 캡슐 컨트롤러를 만든다. 0이면 실패.
	uint32  Create_Controller(const CHARACTER_CONTROLLER_DESC& desc, const Vector3& footPosition);
	/// 변위만큼 쓸어서 옮기고 CONTROLLER_COLLISION 비트를 돌려준다.
	uint32  Move_Controller(uint32 handle, const Vector3& displacement, Float timeDelta);
	void    Set_ControllerFootPosition(uint32 handle, const Vector3& footPosition);
	Vector3 Get_ControllerFootPosition(uint32 handle) const;
	void    Release_Controller(uint32 handle);

private:
	void Release_Physics();

private:
	physx::PxFoundation* m_Foundation{ nullptr };
	physx::PxPhysics* m_Physics{ nullptr };
	physx::PxDefaultCpuDispatcher* m_Dispatcher{ nullptr };
	physx::PxScene* m_Scene{ nullptr };
	physx::PxMaterial* m_DefaultMaterial{ nullptr };
	physx::PxControllerManager* m_ControllerManager{ nullptr };

	// 구운 메시는 태그로 공유하고, 액터는 올린 오브젝트마다 하나씩 잡는다.
	unordered_map<wstring, physx::PxTriangleMesh*> m_StaticMeshes;
	unordered_map<uint32, physx::PxRigidStatic*> m_StaticActors;
	unordered_map<uint32, physx::PxController*> m_Controllers;
	// 정적 액터와 컨트롤러가 한 카운터를 나눠 써서 핸들이 서로 겹치지 않는다.
	uint32 m_NextActorHandle{ 1 };

public:
	static Unique<PhysicsManager> Create();
};

NS_END
