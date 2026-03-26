#include "Animation.h"
#include "Channel.h"
#include "SpdLogger.h"
#include "String_Helper.h"
#include "Bone.h"

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
	m_Channels.clear();
	for (auto& channel : rhs.m_Channels)
	{
		m_Channels.push_back(static_pointer_cast<Channel>(channel->Clone()));
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

	m_RootTotalTranslation = modelAnimation.rootTotalTranslation; // (0, 0, -362)
	m_RootTotalRotation = modelAnimation.rootTotalRotation;

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

void Animation::Enable_RootMotionChannel(int32 boneIndex)
{
	for (auto& channel : m_Channels)
	{
		if (channel->Get_BoneIndex() == boneIndex)
		{
			channel->Set_NeutralizeTranslation(true);
			break;
		}
	}
}

const TRANSFORM_FRAME& Animation::Get_TransformDelta(int32 boneIndex) const
{
	static TRANSFORM_FRAME emptyFrame{ Vector3::One, Vector4(0.f, 0.f, 0.f, 1.f), Vector3::Zero };
	for (const auto& channel : m_Channels)
	{
		if (channel->Get_BoneIndex() == boneIndex)
			return channel->Get_TransformDelta();
	}
	return emptyFrame;
}

void Animation::Reset_RootMotionState()
{
	m_CurrentTrackPosition = 0.f;
	m_PrevTrackAccTranslation = Vector3::Zero;
	m_LastFrameDelta = Vector3::Zero;

	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		m_CurrentKeyFrameIndices[i] = 0;
		m_Channels[i]->Reset_DeltaState();
	}
}

Bool Animation::Update_TransformationMatrix(Float timeDelta, const vector<Shared<Bone>>& bones, Bool isLoop)
{
	m_CurrentTrackPosition += m_TickPerSecond * timeDelta;

	Float progress = m_CurrentTrackPosition / m_Duration;
	Vector3 currentAcc = m_RootTotalTranslation * progress;
	m_LastFrameDelta = currentAcc - m_PrevTrackAccTranslation;

	if (m_CurrentTrackPosition >= m_Duration)
	{
		if (false == isLoop)
			return true;

		m_CurrentTrackPosition = 0;
		m_PrevTrackAccTranslation = Vector3::Zero;
		m_LastFrameDelta = Vector3::Zero;
	}
	else
	{
		m_PrevTrackAccTranslation = currentAcc;
	}

	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		m_Channels[i]->Update_TransformationMatrix(m_CurrentKeyFrameIndices[i], m_CurrentTrackPosition, m_Duration, bones);
	}

	return false;
}

void Animation::Blend_TransformationMatrix(Float timeDelta, const Shared<Animation>& nextAnim, Float blendRatio, const vector<Shared<Bone>>& bones)
{
	m_CurrentTrackPosition += m_TickPerSecond * timeDelta;
	if (m_CurrentTrackPosition >= m_Duration)
		m_CurrentTrackPosition = 0;

	nextAnim->m_CurrentTrackPosition += nextAnim->m_TickPerSecond * timeDelta;
	if (nextAnim->m_CurrentTrackPosition >= nextAnim->m_Duration)
	{
		nextAnim->m_CurrentTrackPosition = 0.f;
	}

	// 각 채널의 Delta를 갱신하기 위해 Update_TransformationMatrix 호출
	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		m_Channels[i]->Update_TransformationMatrix(m_CurrentKeyFrameIndices[i], m_CurrentTrackPosition, m_Duration, bones);
	}
	for (uint32 i = 0; i < nextAnim->m_NumChannels; ++i)
	{
		nextAnim->m_Channels[i]->Update_TransformationMatrix(nextAnim->m_CurrentKeyFrameIndices[i], nextAnim->m_CurrentTrackPosition, nextAnim->m_Duration, bones);
	}

	// 블렌딩된 포즈를 본에 덮어쓰기
	for (uint32 i = 0; i < m_NumChannels; ++i)
	{
		TRANSFORM_FRAME curTrans{}, nextTrans{};

		m_Channels[i]->Get_ChannelTransform(m_CurrentTrackPosition, m_CurrentKeyFrameIndices[i], m_Duration, curTrans);
		nextAnim->m_Channels[i]->Get_ChannelTransform(nextAnim->m_CurrentTrackPosition, nextAnim->m_CurrentKeyFrameIndices[i], nextAnim->m_Duration, nextTrans);

		Vector3 targetScale = Vector3::Lerp(curTrans.scale, nextTrans.scale, blendRatio);
		Vector4 targetRot = Quaternion::Slerp(curTrans.rotation, nextTrans.rotation, blendRatio);
		Vector3 targetPos = Vector3::Lerp(curTrans.position, nextTrans.position, blendRatio);

		// NeutralizeTranslation 활성화된 루트 본 채널은 첫 프레임 위치로 고정

		Vector3 bonePos = targetPos;
		if (m_Channels[i]->Is_NeutralizeTranslation())
		{
		//	// 루트 본: 두 애니메이션 모두 첫 프레임 위치를 사용하여 제자리 유지
			bonePos = Vector3::Lerp(m_Channels[i]->Get_FirstFramePosition(), nextAnim->m_Channels[i]->Get_FirstFramePosition(), blendRatio);
		}

		Matrix targetMatrix = XMMatrixAffineTransformation(targetScale, Quaternion::Identity, targetRot, bonePos);
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
