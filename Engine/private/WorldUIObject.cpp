#include "WorldUIObject.h"

#include "Game.h"

WorldUIObject::WorldUIObject() : UIObject{} {}
WorldUIObject::WorldUIObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: UIObject{device, context} {}
WorldUIObject::WorldUIObject(const WorldUIObject& rhs)
	: UIObject{rhs} {}

HRESULT WorldUIObject::Initialize_Prototype()
{
	return UIObject::Initialize_Prototype();
}

HRESULT WorldUIObject::Initialize(void* arg)
{
	return UIObject::Initialize(arg);
}

