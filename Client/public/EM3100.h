#pragma once
#include "Monster.h"

NS_BEGIN(Engine)
class Model;
class Shader;
class SphereCollider;
class AABBCollider;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL EM3100 final : public Monster
{
	RTTR_ENABLE(Monster)
public:
	explicit EM3100();
	explicit EM3100(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit EM3100(const EM3100& rhs);
	~EM3100() override = default;
	
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

public:
	void OnCollisionEnter(const Shared<GameObject>& collision) override;
	void OnCollisionStay(const Shared<GameObject>& collision) override;
	void OnCollisionExit(const Shared<GameObject>& collision) override;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();
	
private:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Model> m_Model{ nullptr };
	Shared<AABBCollider> m_HitBox{nullptr};
	Shared<SphereCollider> m_InteractionZone{ nullptr };

	Shared<MonsterStateMachine> m_States{ nullptr };

public:
	static Shared<EM3100> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END