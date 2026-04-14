#pragma once
#include "Em3000Parts.h"

NS_BEGIN(Engine)
class AABBCollider;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL Em3000Body final : public Em3000Parts
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagEm3000BodyDesc : public PARTOBJECT_DESC
	{

	} EM3000BODY_DESC;

public:
	explicit Em3000Body();
	explicit Em3000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3000Body(const Em3000Body& rhs);
	~Em3000Body() override = default;
	
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	HRESULT Begin() override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

public:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();
	HRESULT Ready_AnimationNotify();

private:
	Shared<AABBCollider>	m_HitBox{ nullptr };

public:
	static Shared<Em3000Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
	
};

NS_END