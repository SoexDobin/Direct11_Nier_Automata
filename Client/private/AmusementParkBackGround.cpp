#include "pch.h"
#include "AmusementParkBackGround.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>

AmusementParkBackGround::AmusementParkBackGround(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {
}

AmusementParkBackGround::AmusementParkBackGround(const AmusementParkBackGround& rhs)
	: WorldObject{ rhs } {
}

HRESULT AmusementParkBackGround::Initialize_Prototype()
{
	return WorldObject::Initialize_Prototype();
}

HRESULT AmusementParkBackGround::Initialize(void* arg)
{

	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"AmusementParkBackGround";
	desc.navTag = L"AmusementParkBackGround";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void AmusementParkBackGround::On_Destroy()
{
	WorldObject::On_Destroy();
}

void AmusementParkBackGround::On_Enable()
{
	WorldObject::On_Enable();
}

void AmusementParkBackGround::On_Disable()
{
	WorldObject::On_Disable();
}

HRESULT AmusementParkBackGround::Render()
{
#ifdef _DEBUG
	m_Navigation->Render_Debug();
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	Matrix worldMat = m_Transform->Get_WorldMatrix();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (!m_Model->IsInFrustum_PreMesh(i, worldMat))
			continue;

		if (FAILED(m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0)))
			continue;

		HRESULT h1 = m_Model->Bind_Material(m_Shader, DiffuseMap1, i, 1, 1);
		HRESULT h2 = m_Model->Bind_Material(m_Shader, DiffuseMap2, i, 1, 2);
		Bool isBlend = SUCCEEDED(h1) && SUCCEEDED(h2);
		uint32 passIndex = isBlend ? 0 : 1;

		m_Model->Bind_Material(m_Shader, NormalMap, i, 6, 0);
		if (isBlend)
		{
			m_Model->Bind_Material(m_Shader, NormalMap1, i, 6, 1);
			m_Model->Bind_Material(m_Shader, NormalMap2, i, 6, 2);
		}


		if (FAILED(m_Shader->Begin(passIndex)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void AmusementParkBackGround::Submit_RenderGroup()
{
	WorldObject::Submit_RenderGroup();
}

Shared<AmusementParkBackGround> AmusementParkBackGround::Create(const ComPtr<ID3D11Device>& device,
                                                                const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<AmusementParkBackGround>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AmusementParkBackGround");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> AmusementParkBackGround::Clone(void* arg)
{
	auto instance = make_shared<AmusementParkBackGround>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : AmusementParkBackGround");
		return nullptr;
	}

	return instance;
}




