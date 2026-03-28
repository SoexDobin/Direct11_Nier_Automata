#include "Channel.h"

#include "Animation.h"
#include "Bone.h"
#include "SpdLogger.h"
#include "String_Helper.h"

Channel::Channel() : Component{} {}
Channel::Channel(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{ device, context } {}

Channel::Channel(const Channel& rhs)
	: Component{ rhs }, m_NumKeyFrames{ rhs.m_NumKeyFrames }, m_KeyFrames{ rhs.m_KeyFrames }, m_BoneIndex{rhs.m_BoneIndex} 
{
	m_IsFirstUpdate = true; // 클론된 인스턴스는 새로운 상태로 시작해야 함
	m_PrevTrackPosition = -1.f;
	m_Transformation = { Vector3::One, Vector4::UnitW, Vector3::Zero };
}

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

void Channel::Update_Velocity(const TRANSFORM_FRAME& currentFrame, Float currentTrackPosition)
{
	if (m_IsFirstUpdate)
	{
		m_PrevTransform = currentFrame;
		m_Transformation = TRANSFORM_FRAME{ Vector3::One, Quaternion::Identity, Vector3::Zero };
		m_IsFirstUpdate = false;
	}
	else
	{
		if (currentTrackPosition < m_PrevTrackPosition)
		{
			m_PrevTransform = currentFrame;
		}
		else
			// 1. 위치 델타 계산
			m_Transformation.position = currentFrame.position - m_PrevTransform.position;

			// 2. 회전 델타 계산 (Q_curr * inv(Q_prev))
			Quaternion qtCurr(currentFrame.rotation);
			Quaternion qtPrev(m_PrevTransform.rotation);
			Quaternion qtInvPrev; qtPrev.Inverse(qtInvPrev);

			Quaternion qtDelta = qtCurr * qtInvPrev;
			m_Transformation.rotation = Vector4(qtDelta.x, qtDelta.y, qtDelta.z, qtDelta.w);

			m_PrevTransform = currentFrame;
		
	}
	m_PrevTrackPosition = currentTrackPosition;
}

void Channel::Update_TransformationMatrix(uint32& currentKeyFrameIndex, Float currentTrackPosition, Float duration, const vector<Shared<Bone>>& bones, int32 rootNodeIndex)
{
	TRANSFORM_FRAME currentFrame{};
	Get_ChannelTransform(currentTrackPosition, currentKeyFrameIndex, duration, currentFrame);
	Update_Velocity(currentFrame, currentTrackPosition);

	Matrix boneMatrix{};
	if (m_BoneIndex == rootNodeIndex)
		boneMatrix =
		XMMatrixAffineTransformation(currentFrame.scale, Quaternion::Identity, currentFrame.rotation, Vector3::Zero);
	else 
		boneMatrix =
		XMMatrixAffineTransformation(currentFrame.scale, Quaternion::Identity, currentFrame.rotation, currentFrame.position);

	bones[m_BoneIndex]->Update_TransformationMatrix(boneMatrix);
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

Shared<Component> Channel::Clone(void* arg)
{
	auto instance = make_shared<Channel>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Channel");
		return nullptr;
	}

	return instance;
}


