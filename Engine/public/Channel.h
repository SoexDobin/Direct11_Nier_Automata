#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class Bone;

class Channel final : public Component
{
	RTTR_ENABLE(Component)
public:
	explicit Channel();
	explicit Channel(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Channel(const Channel& rhs);
	~Channel() override = default;

public:
	void On_Destroy() override;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::CHANNEL; }
	HRESULT Initialize_Prototype(const MODEL_CHANNEL& modelChannel);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	void Update_TransformationMatrix(Float currentTrackPosition, const vector<Shared<Bone>>& bones);

private:
	uint32 m_NumKeyFrames{};
	vector<KEYFRAME> m_KeyFrames;
	uint32 m_CurrentKeyFrameIndex{};
	int32 m_BoneIndex{ -1 };

public:
	static Shared<Channel> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_CHANNEL& keyFrame);
	Shared<Component> Clone(void* arg = nullptr) override { return nullptr; }
};

NS_END