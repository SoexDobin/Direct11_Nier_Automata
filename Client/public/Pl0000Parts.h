#pragma once
#include <Model.h>

#include "PartObject.h"

NS_BEGIN(Engine)
class Model;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL Pl0000Parts abstract :public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	explicit Pl0000Parts();
	explicit Pl0000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Pl0000Parts(const Pl0000Parts& rhs);
	virtual ~Pl0000Parts() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	Shared<Model> Get_ModelComponent() { return m_Model; }
	void Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop);
	uint32 Get_CurrentAnimationIndex() const { return m_Model->Get_AnimationIndex(); }
	uint32 Get_NextAnimationIndex() const { return m_Model->Get_NextAnimationIndex(); }

protected:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Model> m_Model{ nullptr };
public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END