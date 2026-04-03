#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Collider abstract : public Component
{
	RTTR_ENABLE(Component)
public:
	typedef struct tagColliderDesc : public COMPONENT_DESC
	{
		Vector3 offset;
	} COLLIDER_DESC;
protected:
	explicit Collider();
	explicit Collider(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Collider(const Collider& rhs);
	virtual ~Collider() override;
	
public:
	virtual COLLIDER_TYPE Get_ColliderType() const PURE;
	virtual COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::COLLIDER; }
	const Vector3& Get_Offset() const { return m_Offset; }
	virtual void Set_Offset(const Vector3& offset) { m_Offset = offset; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;
	virtual void On_Destroy() override;
	virtual void On_Disable() override;
	virtual void Update(const Matrix& worldMatrix);
	virtual Bool Intersect(const Shared<Collider>& target);

public:
	void Add_OverlapMember(Collider* collider) { m_OverlapMembers.insert(collider); }
	void Release_OverlapMember(Collider* collider) { m_OverlapMembers.erase(collider); }
	Bool Is_Overlapped(Collider* collider) const { return m_OverlapMembers.contains(collider); }

	void Set_IsColliding(Bool isColliding) { m_IsColliding = isColliding; }
	Bool Get_IsColliding() const { return m_IsColliding; }

#ifdef _DEBUG
public:
	virtual HRESULT Render_Debug(const Shared<PrimitiveBatch<VertexPositionColor>>& batch, const Color& color);
#endif

protected:
	Vector3 m_Offset{};
	Bool m_IsColliding{ false };
	unordered_set<Collider*> m_OverlapMembers;

public:
	Shared<Component> Clone(void* arg = nullptr) PURE;

};

NS_END