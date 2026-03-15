#include "Channel.h"

#include "Animation.h"
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
