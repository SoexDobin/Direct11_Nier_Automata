#pragma once
#include "PartObject.h"
#include "Model.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class Em3000;

class CLIENT_DLL Em3000Parts abstract : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagEm3000PartsDesc : public PARTOBJECT_DESC
	{
		
	} EM3000PART_DESC;
public:
	explicit Em3000Parts();
	explicit Em3000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3000Parts(const Em3000Parts& rhs);
	virtual ~Em3000Parts() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void Set_Pl0000Container(const Shared<Em3000>& em3000) { m_Em3000 = em3000; }

public:
	Shared<Model> Get_ModelComponent() { return m_Model; }
	const TRANSFORM_FRAME& Get_ModelTransform() const;
	virtual void Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop);
	uint32 Get_CurrentAnimationIndex() const { return m_Model->Get_AnimationIndex(); }
	uint32 Get_NextAnimationIndex() const { return m_Model->Get_NextAnimationIndex(); }
	Float Get_AnimationProgress() const { return m_Model->Get_AnimationProgress(); }
	Bool Is_AnimationFinished() const { return m_Model->Is_AnimationFinished(); }

protected:
	Shared<Shader>	m_Shader{ nullptr };
	Shared<Model>	m_Model{ nullptr };
	Weak<Em3000>	m_Em3000{};

public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END