#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Transform final : public Component
{
public:
	typedef struct tagTransformDesc
	{
		// _float		fSpeedPerSec = {};
		// _float		fRotationPerSec = {};
	} TRANSFORM_DESC;
public:
	Transform(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	Transform(const Shared<Transform>& prototype);
	~Transform() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(Shared<void> arg) override;

private:
	Matrix		m_WorldMatrix = {};

public:
	static Shared<Transform> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	Shared<Component> Clone(Shared<void> arg) override;

};

NS_END