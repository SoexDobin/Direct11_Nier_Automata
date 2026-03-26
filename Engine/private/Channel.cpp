#include "Channel.h"

#include "Animation.h"
#include "Bone.h"
#include "SpdLogger.h"
#include "String_Helper.h"

Channel::Channel() : Component{} {}
Channel::Channel(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{ device, context } {}

Channel::Channel(const Channel& rhs)
	: Component{ rhs }, m_NumKeyFrames{ rhs.m_NumKeyFrames }, m_KeyFrames{ rhs.m_KeyFrames }, m_BoneIndex{rhs.m_BoneIndex} {
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

void Channel::Update_TransformationMatrix(uint32& currentKeyFrameIndex, Float currentTrackPosition, Float duration, const vector<Shared<Bone>>& bones, int32 rootNodeIndex)
{
	// rootNodeIndex 가 존재한다면
	// 해당 Channel이 가진 KeyFrame을 가지고 Matrix를 구성하고 이게 pl0000 이면 최상위 노드이니 
	// 해당 노드의 역행렬을 모두 곱해주어서 애니메이션이 더이상 rootNode의 이동량을 따라가지 않도록 하기

	// 위 연산을 이루는 동안 Channel은 이전 KeyFrame 데이터 - 현재 KeyFrame 데이터 을 통해 Get_ChannelTransform시 넘겨줄 변화량 제공
	// 물론 사용은 어차피 거의 실제 Client 쪽에 사용할 rootNode만 사용할거임

	TRANSFORM_FRAME currentFrame{};
	Get_ChannelTransform(currentTrackPosition, currentKeyFrameIndex, duration, currentFrame);

	if (m_IsFirstUpdate)
	{
		m_PrevTransform = currentFrame;
		m_TransformationDelta = TRANSFORM_FRAME{ Vector3::One, Quaternion::Identity, Vector3::Zero };
		m_IsFirstUpdate = false;
	}
	else
	{
		// 1. 위치 델타 계산
		m_TransformationDelta.position = currentFrame.position - m_PrevTransform.position;
		// 2. 회전 델타 계산 (Q_curr * inv(Q_prev))
		Quaternion qtCurr(currentFrame.rotation);
		Quaternion qtPrev(m_PrevTransform.rotation);
		Quaternion qtInvPrev; qtPrev.Inverse(qtInvPrev);

		Quaternion qtDelta = qtCurr * qtInvPrev;
		m_TransformationDelta.rotation = Vector4(qtDelta.x, qtDelta.y, qtDelta.z, qtDelta.w);

		m_PrevTransform = currentFrame;
	}


	if (rootNodeIndex != -1)
		m_NodeMatrix =
		XMMatrixAffineTransformation(currentFrame.scale, Quaternion::Identity, currentFrame.rotation, Vector3::Zero);
	else 
		m_NodeMatrix =
		XMMatrixAffineTransformation(currentFrame.scale, Quaternion::Identity, currentFrame.rotation, currentFrame.position);

	

	bones[m_BoneIndex]->Update_TransformationMatrix(m_NodeMatrix);
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


