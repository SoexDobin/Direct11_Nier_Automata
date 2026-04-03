#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class Collider;

class CollisionManager final : public EngineManager
{
	NO_COPY(CollisionManager)
public:
	explicit CollisionManager();
	~CollisionManager() override;

public: 
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;
	void Set_Active(Bool isActive) override;

public: 
	void Add_Collider(const Shared<Collider>& collider);
	void Remove_Collider(const Shared<Collider>& collider);

public: 
	void Update_Collision() const;

public: /* 디버그 렌더링 */
#ifdef _DEBUG
	HRESULT Initialize_Debug();
	void Render_Debug() const;
	void Toggle_DebugMode() { m_DebugMode = !m_DebugMode; }
	Bool Get_DebugMode() const { return m_DebugMode; }
#endif

private:
	vector<Shared<Collider>> m_Colliders;
#ifdef _DEBUG
	Bool m_DebugMode = { true };
	ComPtr<ID3D11Device> m_Device{ nullptr }; 
	ComPtr<ID3D11DeviceContext> m_Context{ nullptr };
	ComPtr<ID3D11InputLayout> m_InputLayout{ nullptr };
	Shared<PrimitiveBatch<VertexPositionColor>> m_Batch{ nullptr };
	Shared<BasicEffect> m_Effect{ nullptr };
#endif
public:
	static Unique<CollisionManager> Create();
};

NS_END