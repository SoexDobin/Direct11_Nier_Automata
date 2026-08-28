#include "Animation.h"

#include <utility>
#include "Channel.h"
#include "SpdLogger.h"
#include "String_Helper.h"
#include "Bone.h"
#include <cmath>

Animation::Animation() : Component{} {}
Animation::Animation(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{ device, context } {}

Animation::Animation(const Animation& rhs)
	: Component{ rhs }, 
	m_Duration{ rhs.m_Duration }, 
	m_TickPerSecond{ rhs.m_TickPerSecond },
	m_NumChannels{rhs.m_NumChannels}, 
	m_CurrentKeyFrameIndices{rhs.m_CurrentKeyFrameIndices}
{
	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		m_Channels.push_back(static_pointer_cast<Channel>(rhs.m_Channels[i]->Clone()));
	}
}

void Animation::On_Destroy()
{
	m_Channels.clear();
	m_CurrentKeyFrameIndices.clear();
	Component::On_Destroy();
}

HRESULT Animation::Initialize_Prototype(const MODEL_ANIMATION& modelAnimation)
{
	m_Name = Helper::To_wString(modelAnimation.name);
	m_Duration = modelAnimation.duration;
	m_TickPerSecond = modelAnimation.tickPerSecond;
	m_NumChannels = modelAnimation.numChannel;

	m_CurrentKeyFrameIndices.resize(m_NumChannels);

	m_Channels.clear();
	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		auto channel = Channel::Create(m_Device, m_Context, modelAnimation.channels[i]);
		if (nullptr == channel)
			return E_FAIL;

		m_Channels.push_back(channel);
	}

	return S_OK;
}

HRESULT Animation::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Animation::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

void Animation::Set_Progress(Float progress)
{
	m_CurrentTrackPosition = m_Duration * progress;

	std::fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);
	
	for (auto& channel : m_Channels)
	{
		channel->ResetVelocityState();
		// m_IsFirstUpdate=true, m_PrevTrackPosition=-1.f
	}
}

const TRANSFORM_FRAME& Animation::Get_TransformVelocity(int32 boneIndex) const
{
	static TRANSFORM_FRAME emptyFrame{ Vector3::One, Vector4(0.f, 0.f, 0.f, 1.f), Vector3::Zero };
	for (const auto& channel : m_Channels)
	{
		if (channel->Get_BoneIndex() == boneIndex)
			return channel->Get_TransformDelta();
	}
	return emptyFrame;
}

Bool Animation::Update_TransformationMatrix(Float timeDelta, const vector<Shared<Bone>>& bones, Bool isLoop, int32 rootNodeIndex)
{
	m_CurrentTrackPosition += m_TickPerSecond * timeDelta; // 트랙의 시간 비율 * timedelta 을 누적하여 현재 트랙 지점을 업데이트

	if (m_CurrentTrackPosition >= m_Duration) //  현재 트랙이 애니메이션 길이를 넘으면 
	{
		if (false == isLoop) 
		{
			m_CurrentTrackPosition = m_Duration; // 마지막 프레임 고정
			return true; // 그만
		}

		m_CurrentTrackPosition = fmod(m_CurrentTrackPosition, m_Duration); // 나머지 시간 보존 (Aliasing 방지)
	}

	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		if (m_IsLocalTransformationPresent) // 루트 노드의 영향을 받으면
			m_Channels[i]->Update_TransformationMatrix(m_CurrentKeyFrameIndices[i], m_CurrentTrackPosition, m_Duration, bones, rootNodeIndex, isLoop);
		else 
			m_Channels[i]->Update_TransformationMatrix(m_CurrentKeyFrameIndices[i], m_CurrentTrackPosition, m_Duration, bones, -1, isLoop);
	}

	return false;
}

void Animation::Blend_TransformationMatrix(Float timeDelta, const Shared<Animation>& nextAnim, Float blendRatio, const vector<Shared<Bone>>& bones, Bool isCurLoop, Bool isNextLoop, int32 rootNodeIndex)
{
	// 1. 각 애니메이션의 시간 업데이트 (나머지 보존)
	m_CurrentTrackPosition += m_TickPerSecond * timeDelta;
	if (m_CurrentTrackPosition >= m_Duration)
	{
		if (isCurLoop)
			m_CurrentTrackPosition = fmod(m_CurrentTrackPosition, m_Duration);
		else
			m_CurrentTrackPosition = m_Duration;
	}
		
	
	nextAnim->m_CurrentTrackPosition += nextAnim->m_TickPerSecond * timeDelta;
	if (nextAnim->m_CurrentTrackPosition >= nextAnim->m_Duration)
	{
		if (isNextLoop)
			nextAnim->m_CurrentTrackPosition = fmod(nextAnim->m_CurrentTrackPosition, nextAnim->m_Duration);
		else
			nextAnim->m_CurrentTrackPosition = nextAnim->m_Duration;
	}
		

	// 2. 블렌딩 수행
	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		TRANSFORM_FRAME curTrans{}, nextTrans{};

		// 각 채널로부터 보간된 Transform 획득
		m_Channels[i]->Get_ChannelTransform(m_CurrentTrackPosition, m_CurrentKeyFrameIndices[i], m_Duration, true, curTrans);
		m_Channels[i]->Update_Velocity(curTrans, m_CurrentTrackPosition);

		nextAnim->m_Channels[i]->Get_ChannelTransform(nextAnim->m_CurrentTrackPosition, nextAnim->m_CurrentKeyFrameIndices[i], nextAnim->m_Duration, isNextLoop, nextTrans);
		nextAnim->m_Channels[i]->Update_Velocity(nextTrans, nextAnim->m_CurrentTrackPosition);

		Vector3 targetScale = Vector3::Lerp(curTrans.scale, nextTrans.scale, blendRatio);
		Vector4 targetRot = Quaternion::Slerp(curTrans.rotation, nextTrans.rotation, blendRatio);
		Vector3 targetPos = Vector3::Lerp(curTrans.position, nextTrans.position, blendRatio);

		Matrix targetMatrix{};
		if (m_Channels[i]->Get_BoneIndex() == rootNodeIndex)
			targetMatrix = XMMatrixAffineTransformation(targetScale, Quaternion::Identity, targetRot, Vector3::Zero);
		else
			targetMatrix = XMMatrixAffineTransformation(targetScale, Quaternion::Identity, targetRot, targetPos);

		bones[m_Channels[i]->Get_BoneIndex()]->Update_TransformationMatrix(targetMatrix);
	}
}

Shared<Animation> Animation::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_ANIMATION& animationData)
{
	auto animation = make_shared<Animation>(device, context);

	if (FAILED(animation->Initialize_Prototype(animationData)))
	{
		LOG_ERROR(L"Failed to Created : Animation {}", Helper::To_wString(animationData.name));
		MSG_BOX("Failed to Created : Animation");
		return nullptr;
	}

	return animation;
}

Shared<Component> Animation::Clone(void* arg)
{
	auto animation = make_shared<Animation>(*this);

	if (FAILED(animation->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : Animation");
		MSG_BOX("Failed to Cloned : Animation");
	}

	return animation;
}
