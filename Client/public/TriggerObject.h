#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class SphereCollider;
NS_END

NS_BEGIN(Client)

class TriggerObject final : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	struct TRIGGER_DESC : public GAMEOBJECT_DESC
	{
		wstring								targetLayerName{};
		Vector3								position{ 0.f, 0.f, 0.f };
		Float								radius{ 1.f };
		uint32								levelIndex{ 0 };
		vector<function<void()>>			callbacks;
	};

public:
	explicit  TriggerObject() = default;
	explicit TriggerObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit TriggerObject(const TriggerObject& prototype);
	~TriggerObject() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void Update(Float timeDelta) override;

public:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	std::vector<std::function<void()>>		m_Callbacks;
	Shared<SphereCollider>					m_SphereCollider { nullptr };
	uint32									m_LevelIndex { 0 };
	uint32									m_TargetLayerIndex{ 0 };
	Bool									m_IsTriggered{ false };

public:
	static Shared<TriggerObject> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg = nullptr) override;
};
NS_END
