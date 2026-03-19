#pragma once
#include "PartObject.h"

NS_BEGIN(Client)

class CLIENT_DLL P10000Body final : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	explicit P10000Body();
	explicit P10000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit P10000Body(const P10000Body& rhs);
	~P10000Body() override = default;


};

NS_END