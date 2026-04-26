#include "pch.h"
#include "Pl0000EvadeGhost.h"

#include <SpdLogger.h>
#include <Game.h>
#include <Shader.h>
#include <Model.h>

Pl0000EvadeGhost::Pl0000EvadeGhost() : GameObject{} {}
Pl0000EvadeGhost::Pl0000EvadeGhost(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
	: GameObject{device, context} {}
Pl0000EvadeGhost::Pl0000EvadeGhost(const Pl0000EvadeGhost &rhs)
	: GameObject{rhs} {}

HRESULT Pl0000EvadeGhost::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT Pl0000EvadeGhost::Initialize(void *arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"Failed Pl0000EvadeGhost Initialize");
		return E_FAIL;
	}

	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;

	EVADE_GHOST_DESC* desc = static_cast<EVADE_GHOST_DESC*>(arg);
	m_SnapShots = desc->snapShots;
	m_SpawnWorldMatrix = desc->worldMatrix;
	m_LifeTime = desc->lifeTime;
	m_GhostColor = desc->ghostColor;
	m_RefModel = desc->model;
	m_RefShader = desc->shader;

	if (!m_RefModel || !m_RefShader)
	{
		LOG_ERROR(L"EvadeGhost Requires pModel and pShader in DESC");
		return E_FAIL;
	}

	m_Transform->Set_WorldMatrix(m_SpawnWorldMatrix);
	m_Transform->Update_WorldMatrix();

	return S_OK;
}

void Pl0000EvadeGhost::On_Destroy()
{
	GameObject::On_Destroy();
}

void Pl0000EvadeGhost::Priority_Update(Float timeDelta)
{
}

void Pl0000EvadeGhost::Update(Float timeDelta)
{
	m_Elapsed += timeDelta;

	if (m_Elapsed >= m_LifeTime)
	{
		Destroy(shared_from_this());
		return;
	}

	m_Alpha = 1.0f - (m_Elapsed / m_LifeTime);
	m_Alpha = max(0.f, m_Alpha);
}

void Pl0000EvadeGhost::Late_Update(Float timeDelta)
{
	
}

void Pl0000EvadeGhost::Fixed_Update(Float fixedDelta)
{
}

void Pl0000EvadeGhost::Submit_RenderGroup()
{
	if (Is_Active())
	{
		GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::BLEND, shared_from_this());
	}
}

HRESULT Pl0000EvadeGhost::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_RefModel->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (i < m_SnapShots.size())
			m_RefShader->Bind_Matrices(BoneMatrices, m_SnapShots[i].matrices, m_SnapShots[i].numBones);
		

		if (FAILED(m_RefShader->Begin(3)))
			return E_FAIL;

		m_RefModel->Render(i);
	}

	return S_OK;
}

HRESULT Pl0000EvadeGhost::Bind_ShaderResources()
{
	if (FAILED(m_RefShader->Bind_Matrix(WorldMatrix, &m_SpawnWorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_RefShader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_RefShader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_RefShader, CameraPosition)))
		return E_FAIL;

	m_RefShader->Bind_RawValue("g_GhostAlpha", &m_Alpha, sizeof(Float));
	m_RefShader->Bind_RawValue("g_GhostColor", &m_GhostColor, sizeof(Vector4));

	return S_OK;
}

Shared<Pl0000EvadeGhost> Pl0000EvadeGhost::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
{
	auto prototype = make_shared<Pl0000EvadeGhost>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000EvadeGhost");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Pl0000EvadeGhost::Clone(void *arg)
{
	auto instance = make_shared<Pl0000EvadeGhost>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000EvadeGhost");
		return nullptr;
	}

	return instance;
}
