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
	void Reset_DeltaState();

	Bool Is_NeutralizeTranslation() const { return m_NeutralizeTranslation; }
	void Set_NeutralizeTranslation(Bool enable)
	{
		m_NeutralizeTranslation = enable;
		if (enable && !m_KeyFrames.empty())
		{
			m_FirstFramePosition = m_KeyFrames.front().position;
		}
	}

	const Vector3& Get_FirstFramePosition() const { return m_FirstFramePosition; }

	const TRANSFORM_FRAME& Get_TransformDelta() const { return m_TransformationDelta; }
	int32 Get_BoneIndex() const { return m_BoneIndex; }
	void Get_ChannelTransform(Float currentTrackPosition, uint32& currentKeyFrameIndex, Float duration, _Out_ TRANSFORM_FRAME& outTransform);
	void Update_TransformationMatrix(uint32& currentKeyFrameIndex, Float currentTrackPosition, Float duration, const vector<Shared<Bone>>& bones);

private:
	uint32				m_NumKeyFrames{};
	vector<KEYFRAME>	m_KeyFrames;
	int32				m_BoneIndex{ -1 };

	TRANSFORM_FRAME		m_PrevTransform{};
	TRANSFORM_FRAME		m_TransformationDelta{};
	Float				m_PrevTrackPosition{ -1.f };
	Bool				m_IsFirstUpdate{ true };
	Bool				m_IsFirstFrame{ true };

	Bool				m_NeutralizeTranslation{ false };
	Vector3				m_FirstFramePosition{};

public:
	static Shared<Channel> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_CHANNEL& keyFrame);
	Shared<Component> Clone(void* arg = nullptr) override;

};

NS_END