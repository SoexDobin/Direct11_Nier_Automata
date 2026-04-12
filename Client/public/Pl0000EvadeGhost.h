#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)


NS_END

NS_BEGIN(Client)

class CLIENT_DLL Pl0000EvadeGhost final : public GameObject
{
	RTTR_ENABLE(GameObject)
public:
	explicit Pl0000EvadeGhost();
	explicit Pl0000EvadeGhost(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Pl0000EvadeGhost(const Pl0000EvadeGhost& rhs);
	~Pl0000EvadeGhost() override = default;

public:
	
};

NS_END