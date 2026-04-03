#pragma once
#include "ContainerObject.h"

NS_BEGIN(Client)

class CLIENT_DLL Entity abstract : public ContainerObject
{
	RTTR_ENABLE(ContainerObject)
public:
	typedef struct tagEntityObject : public CONTAINEROBJECT_DESC
	{} ENTITY_CONTAINER;

public:
	typedef struct tagDamageInfo
	{
		Float	damage{ 0.f };
		Float	knockbackForce{ 0.f };
		Vector3 attackerPos{ 0.f, 0.f, 0.f };
		Vector3 knockBackDir{0.f, 0.f, 0.f };
		Bool	isGroggyAttack{ false };
	} DAMAGE_INFO;

public:
	// TODO 공통 상태, 기능, 이벤트 정립

public:
	explicit Entity();
	explicit Entity(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Entity(const Entity& rhs);
	virtual ~Entity() override = default;

public: /* Entity interface */
	virtual void TakeDamage() {};

public:
	virtual Shared<GameObject> Clone(void* arg) PURE;
};

NS_END