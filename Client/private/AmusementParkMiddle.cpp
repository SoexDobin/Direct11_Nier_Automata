#include "pch.h"
#include "AmusementParkMiddle.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>

#include "AmusementParkLight.h"

AmusementParkMiddle::AmusementParkMiddle(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {
}

AmusementParkMiddle::AmusementParkMiddle(const AmusementParkMiddle& rhs)
	: WorldObject{ rhs } {
}

void AmusementParkMiddle::SetUp_EntryLight()
{
	if (m_IsEntry) return;

	uint32 levIndex = ETOI(LEVEL::GAMEPLAY2);
	wstring protoTag = L"AmusementParkLight";

	AmusementParkLight::AMUSEMENT_LIGHT_DESC desc{};
	auto& lDesc = desc.lightDesc;
	lDesc.type = LIGHT::POINT;
	lDesc.range = 12.f;
	lDesc.ambient = Vector4{ 55.f, 55.f, 55.f, 255.f };
	lDesc.specular = Vector4{ 25.f, 25.f, 25.f, 255.f };
	lDesc.direction = Vector4::Zero;

	Float xValues[] = { 80.f, 90.f, 97.f, 86.f, 100.f, 121.f, 142.f, 163 };
	Float zValues[] = { 42.f, 62.f };
	Vector4 colors[] = {
		Vector4(230.f, 30.f, 40.f, 255.f),  // Red
		Vector4(0.f, 150.f, 70.f, 255.f),   // Green
		Vector4(255.f, 200.f, 0.f, 255.f)   // Yellow
	};

	int colorIndex = 0;
	for (auto x : xValues)
	{
		for (auto z : zValues)
		{
			lDesc.position = Vector4(x, 10.f, z, 1.f);
			lDesc.diffuse = colors[colorIndex % 3];
			m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
			colorIndex++;
		}
	}

	lDesc.range = 30.f;
	lDesc.diffuse = Vector4{ 255.f, 255.f, 255.f, 255.f };
	lDesc.ambient = Vector4{ 55.f, 55.f, 55.f, 255.f };
	lDesc.specular = Vector4{ 25.f, 25.f, 25.f, 255.f };
	lDesc.position = Vector4{ 200.f, 20.f, 52.f, 1.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));

	m_IsEntry = true;
}

HRESULT AmusementParkMiddle::Initialize_Prototype()
{
	return WorldObject::Initialize_Prototype();
}

HRESULT AmusementParkMiddle::Initialize(void* arg)
{
	// 151 0 0
	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"AmusementParkMiddle";
	desc.navTag = L"AmusementParkMiddle";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void AmusementParkMiddle::On_Destroy()
{
	for (auto light : m_Lights)
	{
		Destroy(light);
	}
	m_Lights.clear();

	WorldObject::On_Destroy();
}

void AmusementParkMiddle::On_Enable()
{
	WorldObject::On_Enable();
}

void AmusementParkMiddle::On_Disable()
{
	WorldObject::On_Disable();
}

void AmusementParkMiddle::Update(Float timeDelta)
{
	SetUp_EntryLight();
	m_Transform->Update_WorldMatrix();
}

HRESULT AmusementParkMiddle::Render()
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

void AmusementParkMiddle::Submit_RenderGroup()
{
	WorldObject::Submit_RenderGroup();
}

Shared<AmusementParkMiddle> AmusementParkMiddle::Create(const ComPtr<ID3D11Device>& device,
                                                        const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<AmusementParkMiddle>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AmusementParkMiddle");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> AmusementParkMiddle::Clone(void* arg)
{
	auto instance = make_shared<AmusementParkMiddle>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : AmusementParkMiddle");
		return nullptr;
	}

	return instance;
}


