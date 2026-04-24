#include "pch.h"
#include "AmusementParkDome.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>

#include "AmusementParkLight.h"

AmusementParkDome::AmusementParkDome(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {
}

AmusementParkDome::AmusementParkDome(const AmusementParkDome& rhs)
	: WorldObject{ rhs } {
}

void AmusementParkDome::SetUp_Light()
{
	if (m_IsEntry) return;

	uint32 levIndex = ETOI(LEVEL::GAMEPLAY2);
	wstring protoTag = L"AmusementParkLight";

	AmusementParkLight::AMUSEMENT_LIGHT_DESC desc{};
	auto& lDesc = desc.lightDesc;
	lDesc.type = LIGHT::POINT;
	lDesc.range = 10.f;
	lDesc.diffuse = Vector4{ 255.f, 255.f, 255.f, 255.f };
	lDesc.ambient = Vector4{ 100.f, 100.f, 100.f, 255.f };
	lDesc.specular = Vector4{ 25.f, 25.f, 25.f, 255.f };
	lDesc.direction = Vector4::Zero;

	lDesc.position = { 400.f, 40.f, 52.f };
	lDesc.range = 50.f;
	GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc);
	

	m_IsEntry = true;
}

HRESULT AmusementParkDome::Initialize_Prototype()
{
	return WorldObject::Initialize_Prototype();
}

HRESULT AmusementParkDome::Initialize(void* arg)
{
	// 301 0 86.5

	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"AmusementParkDome";
	desc.navTag = L"AmusementParkDome";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void AmusementParkDome::On_Destroy()
{
	WorldObject::On_Destroy();
}

void AmusementParkDome::On_Enable()
{
	WorldObject::On_Enable();
}

void AmusementParkDome::On_Disable()
{
	WorldObject::On_Disable();
}

void AmusementParkDome::Update(Float timeDelta)
{
	SetUp_Light();
}

HRESULT AmusementParkDome::Render()
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

void AmusementParkDome::Submit_RenderGroup()
{
	WorldObject::Submit_RenderGroup();
}

Shared<AmusementParkDome> AmusementParkDome::Create(const ComPtr<ID3D11Device>& device,
                                                    const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<AmusementParkDome>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AmusementParkDome");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> AmusementParkDome::Clone(void* arg)
{
	auto instance = make_shared<AmusementParkDome>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : AmusementParkDome");
		return nullptr;
	}

	return instance;
}


