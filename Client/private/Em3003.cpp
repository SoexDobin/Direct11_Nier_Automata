#include "pch.h"
#include "Em3003.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Model.h"

Em3003::Em3003(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Em3000Parts{device, context} { }
Em3003::Em3003(const Em3003& rhs)
	: Em3000Parts{ rhs } { }

HRESULT Em3003::Initialize_Prototype()
{
	if (FAILED(Em3000Parts::Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed Initialize_Prototype Em3003");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em3003::Initialize(void* arg)
{
	if (FAILED(Em3000Parts::Initialize(arg)))
	{
		LOG_ERROR(L"Failed Initialize Em3003");
		return E_FAIL;
	}

	return S_OK;
}

void Em3003::On_Destroy()
{
	Em3000Parts::On_Destroy();
}

void Em3003::Priority_Update(Float timeDelta)
{
	
}

void Em3003::Update(Float timeDelta)
{
	m_Model->Update_ModelAnimation(timeDelta);
}

void Em3003::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(m_Transform->Get_WorldMatrix());
}

void Em3003::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Em3003::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (FAILED(m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0)))
		{
			LOG_ERROR(L"err 1");
		}
		if (FAILED(m_Model->Bind_Material(m_Shader, NormalMap, i, 6, 0)))
		{
			LOG_ERROR(L"err 2");
		}
		if (FAILED(m_Model->Bind_BoneMatrices(m_Shader, BoneMatrices, i)))
		{
			LOG_ERROR(L"err 3");
		}

		if (FAILED(m_Shader->Begin(1)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void Em3003::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

Shared<Em3003> Em3003::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3003>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3003");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3003::Clone(void* arg)
{
	auto instance = make_shared<Em3003>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3003");
		return nullptr;
	}

	return instance;
}


