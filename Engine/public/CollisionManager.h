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
	HRESULT Initialize_Prototype(uint32 levCount);
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;
	void Set_Active(Bool isActive) override;

public: 
	void Add_Collider(uint32 levIndex, const Shared<Collider>& collider);
	void Remove_Collider(uint32 levIndex, const Shared<Collider>& collider);
	HRESULT Clear_Colliders(uint32 levIndex);

public: 
	void Update_Collision() const;

public: /* 디버그 렌더링 */
#ifdef _DEBUG
	HRESULT Initialize_Debug();
	void Render_Debug() const;
	Bool Toggle_DebugMode()
	{
		m_DebugMode = !m_DebugMode;
		return m_DebugMode;
	}
	Bool Get_DebugMode() const { return m_DebugMode; }
#endif

private:
	vector<vector<Shared<Collider>>> m_Colliders;
	uint32 m_LevelCount{};
#ifdef _DEBUG
	Bool m_DebugMode = { false };
	ComPtr<ID3D11Device> m_Device{ nullptr }; 
	ComPtr<ID3D11DeviceContext> m_Context{ nullptr };
	ComPtr<ID3D11InputLayout> m_InputLayout{ nullptr };
	Shared<PrimitiveBatch<VertexPositionColor>> m_Batch{ nullptr };
	Shared<BasicEffect> m_Effect{ nullptr };
#endif
public:
	static Unique<CollisionManager> Create(uint32 levCount);
};

NS_END