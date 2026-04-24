#pragma once
#include "Pl0000Parts.h"

NS_BEGIN(Engine)
class Model;
class AABBCollider;
NS_END

NS_BEGIN(Client)

class WP0220Body;
class WP0070Body;

class CLIENT_DLL Pl0000Body final : public Pl0000Parts
{
	RTTR_ENABLE(Pl0000Parts)
public:
	typedef struct tagPl0000BodyDesc : public PL0000PART_DESC
	{} Pl0000BODY_DESC ;

public:
	explicit Pl0000Body();
	explicit Pl0000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Pl0000Body(const Pl0000Body& rhs);
	~Pl0000Body() override = default;

public:
	TRANSFORM_FRAME Get_ModelTransform() const { return m_Model->Get_RootTransformVelocity(m_RootBoneIndex); }

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	HRESULT Render_Shadow() override;
	void Submit_RenderGroup() override;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();
	HRESULT Ready_AnimationNotify();

private:
	int32					m_RootBoneIndex{};
	Matrix					m_SheathMatrix{};
	Shared<AABBCollider>	m_HitBox { nullptr };

public:
	static Shared<Pl0000Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END
