#include "pch.h"
#include "P10000Parts.h"

#include <Model.h>


P10000Parts::P10000Parts() : PartObject{} {}

P10000Parts::P10000Parts(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}

P10000Parts::P10000Parts(const P10000Parts& rhs)
	: PartObject{rhs} {}

HRESULT P10000Parts::Initialize_Prototype()
{
	return PartObject::Initialize_Prototype();
}

HRESULT P10000Parts::Initialize(void* arg)
{
	return PartObject::Initialize(arg);
}

void P10000Parts::Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop)
{
	m_Model->Set_Animation(animIndex, blendDuration);
	m_Model->Set_AnimLoop(isLoop);
}

