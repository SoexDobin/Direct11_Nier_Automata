#include "pch.h"
#include "AmusementParkEntry.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>

#include "AmusementParkLight.h"

AmusementParkEntry::AmusementParkEntry(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {
}

AmusementParkEntry::AmusementParkEntry(const AmusementParkEntry& rhs)
	: WorldObject{ rhs } {
}

void AmusementParkEntry::SetUp_EntryLight()
{
	if (m_IsEntry) return;

	uint32 levIndex = ETOI(LEVEL::GAMEPLAY2);
	wstring protoTag = L"AmusementParkLight";

	AmusementParkLight::AMUSEMENT_LIGHT_DESC desc{};
	auto& lDesc = desc.lightDesc;
	lDesc.type = LIGHT::POINT;
	lDesc.range = 10.f;
	lDesc.diffuse = Vector4{ 255.f, 255.f, 255.f, 255.f };
	lDesc.ambient = Vector4{55.f, 55.f, 55.f, 55.f };
	lDesc.specular = Vector4{ 25.f, 25.f, 25.f, 255.f };
	lDesc.direction = Vector4::Zero;

	lDesc.position = {-15.f, 6.f, 52.f };
	lDesc.diffuse = Vector4{ 255.f, 255.f, 255.f, 255.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { -15.f, 6.f, 45.f};
	lDesc.diffuse = Vector4{ 70.f, 230.f, 170.f, 255.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { -15.f, 6.f, 59.f, };
	lDesc.diffuse = Vector4{ 50.f, 255.f, 255.f, 255.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));


	lDesc.diffuse = Vector4{ 255.f, 255.f, 255.f, 255.f };
	lDesc.position = { 25.f, 0.f, 52.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { 40.f, 0.f, 52.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { 30.f, 0.f, 45.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { 30.f, 0.f, 55.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { 36.5f, 0.f, 45.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { 36.5f, 0.f, 55.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));

	lDesc.range = 30.f;
	lDesc.diffuse = Vector4{ 200.f, 125.f, 234.f, 255.f };
	lDesc.position = { 25.f, 15.f, 90.f};
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { 25.f, 15.f, 15.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));

	lDesc.diffuse = Vector4{ 255.f, 100.f, 0.f, 255.f };
	lDesc.position = { 50.f, 15.f, 90.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));
	lDesc.position = { 50.f, 15.f, 15.f};
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));

	lDesc.diffuse = Vector4{ 255.f, 255.f, 255.f, 255.f };
	lDesc.position = { 70.f, 12.f, 52.f };
	m_Lights.push_back(GAME_INSTANCE->Instantiate<AmusementParkLight>(protoTag, levIndex, &desc));

	m_IsEntry = true;
}


HRESULT AmusementParkEntry::Initialize_Prototype()
{
	return WorldObject::Initialize_Prototype();
}

HRESULT AmusementParkEntry::Initialize(void* arg)
{
	// 0 0 86.5
	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"AmusementParkEntry";
	desc.navTag = L"AmusementParkEntry";
	
	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void AmusementParkEntry::On_Destroy()
{
	for (auto light : m_Lights)
	{
		Destroy(light);
	}
	m_Lights.clear();

	WorldObject::On_Destroy();
}

void AmusementParkEntry::On_Enable()
{
	WorldObject::On_Enable();
}

void AmusementParkEntry::On_Disable()
{
	WorldObject::On_Disable();
}

HRESULT AmusementParkEntry::Render()
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

		if (i == 10 || i == 12 || i == 39)
		{
			auto w = Vector4{ 0.20f, 0.4f, 0.4f, 1.f };
			m_Shader->Bind_RawValue(BlendWeight, &w, sizeof(Vector4));
		}
		else
		{
			auto w = Vector4{ 0.33f, 0.33f, 0.34f, 1.f };
			m_Shader->Bind_RawValue(BlendWeight, &w, sizeof(Vector4));
		}
	
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

void AmusementParkEntry::Update(Float timeDelta)
{
	SetUp_EntryLight();
	m_Transform->Update_WorldMatrix();
}

void AmusementParkEntry::Submit_RenderGroup()
{
	WorldObject::Submit_RenderGroup();
}

Shared<AmusementParkEntry> AmusementParkEntry::Create(const ComPtr<ID3D11Device>& device,
	const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<AmusementParkEntry>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AmusementParkEntry");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> AmusementParkEntry::Clone(void* arg)
{
	auto instance = make_shared<AmusementParkEntry>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : AmusementParkEntry");
		return nullptr;
	}

	return instance;
}