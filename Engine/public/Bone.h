#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Bone final : public Component
{
	RTTR_ENABLE(Component)
public:
	explicit Bone();
	explicit Bone(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Bone(const Bone& rhs);
	~Bone() override = default;

public:
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;
	void Set_Active(Bool isActive) override;
	
public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::BONE; };
	HRESULT Initialize_Prototype(const MODEL_BONE& modelBone);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

private:


public:
	static Shared<Bone> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_BONE& boneData);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END