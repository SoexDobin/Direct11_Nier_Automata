#pragma once
#include "GameObject.h"

namespace Engine { struct boneSnapShot; class Model; class Shader; }
using BONE_SNAPSHOT = Engine::boneSnapShot;

NS_BEGIN(Client)

class CLIENT_DLL Pl0000EvadeGhost final : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	typedef struct tagEvadeGhostDesc : public GAMEOBJECT_DESC
	{
		vector<BONE_SNAPSHOT>	snapShots;		
		Matrix					worldMatrix;
		Float					lifeTime{ 0.5f };
		Vector4					ghostColor{ 1.0f, 0.85f, 0.3f, 1.0f };
		Shared<Model>			model;
		Shared<Shader>			shader;
	} EVADE_GHOST_DESC;

public:
	explicit Pl0000EvadeGhost();
	explicit Pl0000EvadeGhost(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context);
	explicit Pl0000EvadeGhost(const Pl0000EvadeGhost &rhs);
	~Pl0000EvadeGhost() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void *arg) override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

private:
	HRESULT Bind_ShaderResources();

private:
	Shared<Shader>				m_RefShader{ nullptr };
	Shared<Model>				m_RefModel{ nullptr };

	vector<BONE_SNAPSHOT>		m_SnapShots;
	Matrix						m_SpawnWorldMatrix{};

	Float						m_LifeTime{ 0.0f };
	Float						m_Elapsed{ 0.0f };
	Float						m_Alpha{ 1.0f };
	Vector4						m_GhostColor{ 1.0f, 1.0f, 1.0f, 1.0f };

public:
	Bool DoNotSerialize() const { return true; }

public:
	static Shared<Pl0000EvadeGhost> Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context);
	Shared<GameObject> Clone(void *arg) override;
};

NS_END