#pragma once
#include "PartObject.h"

NS_BEGIN(Engine)
class Model;
class AABBCollider;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL Em0010Body final : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagEm0010BodyDesc : public PARTOBJECT_DESC
	{
		
	} EM0010BODY_DESC;

public:
	explicit Em0010Body();
	explicit Em0010Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em0010Body(const Em0010Body& rhs);
	~Em0010Body() override = default;
	
public:
	const TRANSFORM_FRAME& Get_ModelTransform() const;
	Shared<Model> Get_ModelComponent() { return m_Model; }
	void Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop);
	uint32 Get_CurrentAnimationIndex() const;
	uint32 Get_NextAnimationIndex() const;
	Float Get_AnimationProgress() const;
	Bool Is_AnimationFinished() const;

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
	void Submit_RenderGroup() override;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();
	HRESULT Ready_AnimationNotify();

private:
	int32 m_RootBoneIndex{};
	Shared<Shader>	m_Shader{ nullptr };
	Shared<Model>	m_Model{ nullptr };
	Shared<AABBCollider> m_HitBox{ nullptr };

public:
	static Shared<Em0010Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END