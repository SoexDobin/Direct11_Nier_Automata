#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Bone final : public Component
{
	RTTR_ENABLE(Component)
public:
	explicit Bone();
	explicit Bone(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~Bone() override = default;

public:
	void On_Destroy() override { Component::On_Destroy(); }
	void On_Disable() override { Component::On_Disable(); }
	void On_Enable() override { Component::On_Enable(); }
	void Set_Active(Bool isActive) override { Component::Set_Active(isActive); }
	
public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::BONE; };
	HRESULT Initialize_Prototype(const MODEL_BONE& modelBone);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	Bool Is_SameBone(const Char* boneName) const { return !strcmp(boneName, m_BoneName); }
	const Matrix* Get_CombinedTransformationMatrixPtr() const { return &m_CombinedTransformationMatrix; }
	void Update_TransformationMatrix(const Matrix& transformationMatrix);
	void Update_CombinedTransformationMatrix(const vector<Shared<Bone>>& modelBones, const Matrix& preTransformMatrix);

private:
	Char		m_BoneName[MAX_PATH]{};
	int32		m_ParentBoneIndex{};
	Matrix		m_TransformationMatrix{};
	Matrix		m_CombinedTransformationMatrix{};

public:
	static Shared<Bone> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_BONE& boneData);
	Shared<Component> Clone(void* arg = nullptr) override { return nullptr; }
};

NS_END