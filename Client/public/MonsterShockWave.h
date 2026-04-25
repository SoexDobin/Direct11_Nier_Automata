#pragma once
#include "GameObject.h"
#include "Entity.h"

NS_BEGIN(Engine)
class SphereCollider;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL MonsterShockWave final : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	typedef struct tagMonsterShockWaveDesc : public GAMEOBJECT_DESC
	{
		Vector3 position{};				// 생성 위치
		Float radius{ 0.f };			// 충격파 반경
		Entity::DAMAGE_INFO damageInfo; // 적용할 데미지 정보 (Player가 설정)
	} MONSTER_SHOCKWAVE_DESC;
public:
	explicit MonsterShockWave();
	explicit MonsterShockWave(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit MonsterShockWave(const MonsterShockWave& rhs);
	~MonsterShockWave() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;


public:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	HRESULT Ready_Components(const MONSTER_SHOCKWAVE_DESC& desc);

private:
	Bool m_IsFirstFrame{ true };
	Shared<SphereCollider> m_ShockwaveCollider{ nullptr };
	Entity::DAMAGE_INFO m_DamageInfo{};
	unordered_set<uint32> m_HitEntities;
	uint32 m_PlayerLayerIndex{ };

public:
	static Shared<MonsterShockWave> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END