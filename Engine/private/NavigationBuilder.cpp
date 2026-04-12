#include "NavigationBuilder.h"
#include "SpdLogger.h"
#include <recastnavigation/Recast.h>
#include <recastnavigation/RecastAlloc.h>

NavigationBuilder::NavigationBuilder() : EngineManager{} {}

HRESULT NavigationBuilder::Initialize_Prototype()
{
	return EngineManager::Initialize_Prototype();
}

HRESULT NavigationBuilder::Initialize(void* arg)
{
	return EngineManager::Initialize(arg);
}

HRESULT NavigationBuilder::Begin()
{
	return EngineManager::Begin();
}

NavigationBuilder::NAV_BUILD_RESULT NavigationBuilder::Build(const Float* vertices, int32 numVertices,
															 const int32* triangles, int32 numTriangles, 
															 const NAV_BUILD_PARAMS_DESC& params)
{

	return {};
}

void NavigationBuilder::ExtractCells(const rcPolyMesh& mesh, const rcPolyMeshDetail& detail, vector<NavCell>& outCells)
{
}

void NavigationBuilder::SetUp_Neighbors(vector<NavCell>& cells)
{
}

Unique<NavigationBuilder> NavigationBuilder::Create()
{
	auto instance = make_unique<NavigationBuilder>();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : NavigationBuilder");
		return nullptr;
	}

	return instance;
}

