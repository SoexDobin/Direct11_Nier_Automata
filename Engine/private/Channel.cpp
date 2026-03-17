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

void Channel::Update_TransformationMatrix(uint32& currentKeyFrameIndex, Float currentTrackPosition, const vector<Shared<Bone>>& bones)
{
	if (0.f == currentTrackPosition)
		currentKeyFrameIndex = 0;

	KEYFRAME lastKeyFrame = m_KeyFrames.back();
	Vector4 scale{}, rotation{}, translation{};

	if (currentTrackPosition >= lastKeyFrame.trackPosition)
	{
		scale = Vector4{lastKeyFrame.scale.x, 
						lastKeyFrame.scale.y, 
						lastKeyFrame.scale.z, 
						0.f 
		};
		rotation = lastKeyFrame.rotation;
		translation = Vector4{lastKeyFrame.position.x, 
							lastKeyFrame.position.y, 
							lastKeyFrame.position.z, 
							1.f 
		};
	}
	else
	{
		while (currentTrackPosition >= m_KeyFrames[currentKeyFrameIndex + 1].trackPosition)
			++currentKeyFrameIndex;

		uint32 curIndex = currentKeyFrameIndex, nextIndex = curIndex + 1;

		Float beforeTrackPos = m_KeyFrames[curIndex].trackPosition;
		Float nextTrackPos = m_KeyFrames[nextIndex].trackPosition;

		Float currentTrackRatio = currentTrackPosition - beforeTrackPos;
		Float trackRatio = nextTrackPos - beforeTrackPos;
		Float ratio = currentTrackRatio / trackRatio;

		// 키프레임 사이 현재 트랙  값 보간
		Vector4 lerpScale{Vector3::Lerp(m_KeyFrames[curIndex].scale, m_KeyFrames[nextIndex].scale, ratio)};
		lerpScale.w = 0.f;
		Vector4 slerpRotation = Quaternion::Slerp(m_KeyFrames[curIndex].rotation, m_KeyFrames[nextIndex].rotation, ratio);
		Vector4 lerpPosition{Vector3::Lerp(m_KeyFrames[curIndex].position, m_KeyFrames[nextIndex].position, ratio)};
		lerpPosition.w = 1.f;

		scale = lerpScale;
		rotation = slerpRotation;
		translation = lerpPosition;
	}
	Matrix boneTransformationMatrix = 
		XMMatrixAffineTransformation(scale, Quaternion::Identity, rotation, translation);

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
