#include "pch.h"
#include "CityOfRuinEntry.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>


CityOfRuinEntry::CityOfRuinEntry(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {}
CityOfRuinEntry::CityOfRuinEntry(const CityOfRuinEntry& rhs)
	: WorldObject{ rhs } {}

HRESULT CityOfRuinEntry::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT CityOfRuinEntry::Initialize(void* arg)
{
	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXMESH::Tag;
	desc.modelTag = L"CityOfRuinEntry";
	desc.navTag = L"CityOfRuinEntry";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void CityOfRuinEntry::On_Destroy()
{
	GameObject::On_Destroy();
}

void CityOfRuinEntry::On_Enable()
{
	GameObject::On_Enable();
}

void CityOfRuinEntry::On_Disable()
{
	GameObject::On_Disable();
}

HRESULT CityOfRuinEntry::Render()
{
	
#ifdef _DEBUG
	m_Navigation->Render_Debug();
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0);

		if (FAILED(m_Shader->Begin(0)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void CityOfRuinEntry::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

Shared<CityOfRuinEntry> CityOfRuinEntry::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<CityOfRuinEntry>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : CityOfRuinEntry");
		MSG_BOX("Failed to Created : CityOfRuinEntry");
	}
	return prototype;
}

Shared<GameObject> CityOfRuinEntry::Clone(void* arg)
{
	auto instance = make_shared<CityOfRuinEntry>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : CityOfRuinEntry");
		MSG_BOX("Failed to Cloned : CityOfRuinEntry");
	}
	return instance;
}
