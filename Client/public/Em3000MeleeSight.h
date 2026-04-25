#pragma once
#include "MonsterSight.h"

NS_BEGIN(Engine)
class SphereCollider;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL Em3000MeleeSight final : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagMonsterSightDesc : public PARTOBJECT_DESC
	{
		Float radius{};
		Vector3 offset{};
	} MONSTER_SIGHT_DESC;

public:
	explicit Em3000MeleeSight() = default;
	explicit Em3000MeleeSight(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3000MeleeSight(const Em3000MeleeSight& rhs);
	~Em3000MeleeSight() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Enable() override;
	void On_Disable() override;

public:
	Bool Has_Target() const { return m_HasTarget; }
	Bool Is_TargetFront() const { return m_IsTargetFront; }

private:
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;

private:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	Bool m_HasTarget{ false };
	Bool m_IsTargetFront{ false };

	Shared<SphereCollider> m_Sight{ nullptr };
	uint32 m_PlayerPhysicalLayerIndex{};

public:
	static Shared<Em3000MeleeSight> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;

};

NS_END