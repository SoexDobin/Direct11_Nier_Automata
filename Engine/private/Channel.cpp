#include "Channel.h"

#include "Animation.h"
#include "Bone.h"
#include "SpdLogger.h"
#include "String_Helper.h"

Channel::Channel() : Component{} {}
Channel::Channel(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{ device, context } {}

Channel::Channel(const Channel& rhs)
	: Component{rhs}, m_NumKeyFrames{rhs.m_NumKeyFrames}, m_KeyFrames{rhs.m_KeyFrames} {}

void Channel::On_Destroy()
{
	m_KeyFrames.clear();
	Component::On_Destroy();
}

HRESULT Channel::Initialize_Prototype(const MODEL_CHANNEL& modelChannel)
{
	m_BoneIndex = modelChannel.boneIndex;
	m_NumKeyFrames = modelChannel.numKeyFrames;

	m_KeyFrames.clear();
	m_KeyFrames = modelChannel.keyFrames;

	return Component::Initialize_Prototype();
}

HRESULT Channel::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Channel::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

void Channel::Get_ChannelTransform(Float currentTrackPosition, uint32& currentKeyFrameIndex, Float duration, _Out_ TRANSFORM_FRAME& outTransform)
{
	if (currentTrackPosition <= 0.f)
		currentTrackPosition = 0;

	if (currentTrackPosition < m_KeyFrames[currentKeyFrameIndex].trackPosition)
	{
		currentKeyFrameIndex = 0;
	}

	KEYFRAME firstKeyFrame = m_KeyFrames.front();
	KEYFRAME lastKeyFrame = m_KeyFrames.back();

	if (currentTrackPosition >= lastKeyFrame.trackPosition)
	{
		Float ratio = 0.f;
		Float trackRange = duration - lastKeyFrame.trackPosition;

		if (trackRange > 0.f)
			ratio = (currentTrackPosition - lastKeyFrame.trackPosition) / trackRange;

		outTransform.scale = Vector3::Lerp(lastKeyFrame.scale, firstKeyFrame.scale, ratio);
		outTransform.rotation = Quaternion::Slerp(lastKeyFrame.rotation, firstKeyFrame.rotation, ratio);
		outTransform.position = Vector3::Lerp(lastKeyFrame.position, firstKeyFrame.position, ratio);
	}
	else
	{
		while (currentTrackPosition >= m_KeyFrames[currentKeyFrameIndex + 1].trackPosition)
			++currentKeyFrameIndex;

		uint32 curIndex = currentKeyFrameIndex;
		uint32 nextIndex = curIndex + 1;

		Float beforeTrackPos = m_KeyFrames[curIndex].trackPosition;
		Float nextTrackPos = m_KeyFrames[nextIndex].trackPosition;
		Float ratio = (currentTrackPosition - beforeTrackPos) / (nextTrackPos - beforeTrackPos);

		outTransform.scale = Vector3::Lerp(m_KeyFrames[curIndex].scale, m_KeyFrames[nextIndex].scale, ratio);
		outTransform.rotation = Quaternion::Slerp(m_KeyFrames[curIndex].rotation, m_KeyFrames[nextIndex].rotation, ratio);
		outTransform.position = Vector3::Lerp(m_KeyFrames[curIndex].position, m_KeyFrames[nextIndex].position, ratio);
	}
}

void Channel::Update_TransformationMatrix(uint32& currentKeyFrameIndex, Float currentTrackPosition, Float duration, const vector<Shared<Bone>>& bones)
{
	TRANSFORM_FRAME frame{};
	Get_ChannelTransform(currentTrackPosition, currentKeyFrameIndex, duration, frame);
	
	Matrix boneTransformationMatrix =
		XMMatrixAffineTransformation(frame.scale, Quaternion::Identity, frame.rotation, frame.position);

	bones[m_BoneIndex]->Update_TransformationMatrix(boneTransformationMatrix);
}

Shared<Channel> Channel::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_CHANNEL& keyFrame)
{
	auto channel = make_shared<Channel>(device, context);

	if (FAILED(channel->Initialize_Prototype(keyFrame)))
	{
		LOG_ERROR(L"Failed to Created : Channel {}", Helper::To_wString(keyFrame.name));
		MSG_BOX("Failed to Created : Channel");
	}

	return channel;
}
