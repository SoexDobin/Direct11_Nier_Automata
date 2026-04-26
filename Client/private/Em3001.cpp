#include "pch.h"
#include "Em3001.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Em3000Parts.h"
#include "Model.h"

Em3001::Em3001(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Em3000Parts{device, context} {}
Em3001::Em3001(const Em3001& rhs) 
	: Em3000Parts{ rhs } {}

HRESULT Em3001::Initialize_Prototype()
{
	if (FAILED(Em3000Parts::Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed Initialize_Prototype Em3001");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em3001::Initialize(void* arg)
{
	if (FAILED(Em3000Parts::Initialize(arg)))
	{
		LOG_ERROR(L"Failed Initialize Em3001");
		return E_FAIL;
	}

	m_OffsetMatrix = Matrix::CreateRotationY(XMConvertToRadians(180.f)) * Matrix::CreateTranslation({ 0.f, 0.5f, 0.125f });

	return S_OK;
}

void Em3001::On_Destroy()
{
	Em3000Parts::On_Destroy();
}

void Em3001::Priority_Update(Float timeDelta)
{
	
}

void Em3001::Update(Float timeDelta)
{
	
}

void Em3001::Late_Update(Float timeDelta)
{
	Em3000Parts::Late_Update(timeDelta);
}

void Em3001::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Em3001::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0);
		m_Model->Bind_Material(m_Shader, NormalMap, i, 6, 0);
		m_Model->Bind_BoneMatrices(m_Shader, BoneMatrices, i);

		if (FAILED(m_Shader->Begin(1)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void Em3001::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

Shared<Em3001> Em3001::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3001>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3001");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3001::Clone(void* arg)
{
	auto instance = make_shared<Em3001>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3001Part");
		return nullptr;
	}

	return instance;
}


