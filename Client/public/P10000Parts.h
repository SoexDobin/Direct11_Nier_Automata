#pragma once
#include <Model.h>

#include "PartObject.h"

NS_BEGIN(Engine)
class Model;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL P10000Parts abstract :public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	explicit P10000Parts();
	explicit P10000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit P10000Parts(const P10000Parts& rhs);
	virtual ~P10000Parts() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	Shared<Model> Get_ModelComponent() { return m_Model; }
	void Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop);
	uint32 Get_CurrentAnimationIndex() const { return m_Model->Get_AnimationIndex(); }

protected:
	Shared<Shader> m_Shader{ nullptr };
	Shared<Model> m_Model{ nullptr };
public:
	Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END