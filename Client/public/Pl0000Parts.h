#pragma once
#include <Model.h>

#include "PartObject.h"

NS_BEGIN(Engine)
class Model;
NS_END

NS_BEGIN(Client)

class Pl0000;

class CLIENT_DLL Pl0000Parts abstract :public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagPl0000PartsDesc : public PARTOBJECT_DESC
	{
		
	} PL0000PART_DESC;
public:
	explicit Pl0000Parts();
	explicit Pl0000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Pl0000Parts(const Pl0000Parts& rhs);
	virtual ~Pl0000Parts() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void Set_Pl0000Container(const Shared<Pl0000>& pl0000) { m_Pl0000 = pl0000; }

public:
	Shared<Model> Get_ModelComponent() { return m_Model; }
	Shared<Shader> Get_ShaderComponent() { return m_Shader; }
	virtual void Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop);
	uint32 Get_CurrentAnimationIndex() const { return m_Model->Get_AnimationIndex(); }
	uint32 Get_NextAnimationIndex() const { return m_Model->Get_NextAnimationIndex(); }
	Float Get_AnimationProgress() const { return m_Model->Get_AnimationProgress(); }
	Bool Is_AnimationFinished() const { return m_Model->Is_AnimationFinished(); }

protected:
	Shared<Shader>	m_Shader{ nullptr };
	Shared<Model>	m_Model{ nullptr };
	Weak<Pl0000>	m_Pl0000{};

public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END