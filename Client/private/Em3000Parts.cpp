#include "pch.h"
#include "Em3000Parts.h"

#include "SpdLogger.h"


Em3000Parts::Em3000Parts() : PartObject{} {}
Em3000Parts::Em3000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
Em3000Parts::Em3000Parts(const Em3000Parts& rhs)
	: PartObject{rhs} {}

HRESULT Em3000Parts::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Monster");
	m_TagMask.Set_Tag({ L"Monster" });

	return PartObject::Initialize_Prototype();
}

HRESULT Em3000Parts::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"Failed to Initialize Em3000Part");
		m_RootBoneIndex = static_cast<tagEm3000PartsDesc*>(arg)->rootBoneIndex;
		return E_FAIL;
	}

	return PartObject::Initialize(arg);
}

const TRANSFORM_FRAME& Em3000Parts::Get_ModelTransform() const
{
	return m_Model->Get_RootTransformVelocity(m_RootBoneIndex);
}

void Em3000Parts::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_Model->Set_Animation(animIndex, blendDuration);
	m_Model->Set_AnimLoop(isLoop);
}
