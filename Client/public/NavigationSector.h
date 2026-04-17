#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class AABBCollider;
class Navigation;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL NavigationSector final : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	typedef struct tagNavigationCollisionDesc : public GAMEOBJECT_DESC
	{
		Weak<Navigation> TargetCollider;
		Vector3 worldPosition{ 0.f, 0.f, 0.f };
		Vector3 collisionExtends{ 0.f, 0.f, 0.f };
	} NAVIGATION_COLLISION_DESC;

public:
	explicit NavigationSector() = default;
	explicit NavigationSector(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit NavigationSector(const NavigationSector& rhs);
	~NavigationSector() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	Shared<AABBCollider> m_AABBCollider{ nullptr };
	Weak<Navigation> m_Navigation{ };

	unordered_set<uint32> m_TargetLayerIndex;

public:
	static Shared<NavigationSector> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
	
};

NS_END