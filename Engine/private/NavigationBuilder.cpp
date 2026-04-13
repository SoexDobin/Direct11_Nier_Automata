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
	NAV_BUILD_RESULT build{};
	rcContext ctx{};
	rcConfig cfg{};

	// 1. rcConfig구성
	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = params.cellSize;
	cfg.ch = params.cellHeight;
	cfg.walkableSlopeAngle = params.moveableSlopeAngle;
	cfg.walkableHeight = static_cast<int32>(ceilf(params.agentHeight / cfg.ch));
	cfg.walkableClimb = static_cast<int32>(floorf(params.agentMaxClimb / cfg.ch));
	cfg.walkableRadius = static_cast<int32>(ceilf(params.agentRadius / cfg.ch));
	cfg.maxEdgeLen = static_cast<int32>(params.maxEdgeLen / cfg.cs);
	cfg.minRegionArea = params.minRegionArea * params.minRegionArea;
	cfg.mergeRegionArea = params.mergeRegionArea * params.mergeRegionArea;
	cfg.maxVertsPerPoly = params.maxVertexPerPoly;
	cfg.detailSampleDist = params.detailSampleDist < 0.9f ? 0.f : cfg.cs * params.detailSampleDist;
	cfg.detailSampleMaxError = params.detailSampleMaxError;
	cfg.maxSimplificationError = params.maxSimplificationError;

	// 2. AABB & Grid
	rcCalcBounds(vertices, numVertices, cfg.bmin, cfg.bmax);
	rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

#ifdef _DEBUG
	LOG_INFO(L"[ NavMeshBuilder ] Grid: {}x{}, Cells: {}", cfg.width, cfg.height, cfg.width * cfg.height);
#endif

	// 3. HeightField 
	rcHeightfield* solid = rcAllocHeightfield();
	if (!solid)
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to ALLOC rcAllocHeightfield");
		return build;
	}

	if (!rcCreateHeightfield(&ctx, *solid, cfg.width, cfg.height, 
		cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to CREATE rcCreateHeightfield");
		return build;
	}

	// 4. Walkable
	uByte* triangleAreas = new uByte[numTriangles];
	memset(triangleAreas, 0, numTriangles);
	rcMarkWalkableTriangles(&ctx, cfg.walkableSlopeAngle, vertices, numVertices, triangles, numTriangles, triangleAreas);

	// 5. Rasterize
	if (!rcRasterizeTriangles(&ctx, vertices, numVertices,
		triangles, triangleAreas, numTriangles, *solid))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to RASTERIZE rcRasterizeTriangles");
		delete[] triangleAreas;
		rcFreeHeightField(solid);
		return build;
	}
	delete[] triangleAreas;

	// 6. Filtering
	rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *solid);
	rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid);
	rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *solid);

	// 7. Compact(Press)
	rcCompactHeightfield* chf = rcAllocCompactHeightfield();
	if (!chf)
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to ALLOC COMPACT rcAllocCompactHeightfield");
		rcFreeHeightField(solid);
		return build;
	}

	if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid, *chf))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD COMPACT rcBuildCompactHeightfield");
		rcFreeCompactHeightfield(chf);
		rcFreeHeightField(solid);
		return build;
	}
	rcFreeHeightField(solid);
	solid = nullptr;

	// 8. Agent Field 
	if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *chf))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to AGENT ERODE WALKABLE rcErodeWalkableArea");
		rcFreeCompactHeightfield(chf);
		return build;
	}

	// 9. Distance Field
	if (!rcBuildDistanceField(&ctx, *chf))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD DISTANCE rcBuildDistanceField");
		rcFreeCompactHeightfield(chf);
		return build;
	}

	// 10. Region Field
	if (!rcBuildRegions(&ctx, *chf, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD REGION rcBuildRegions");
		rcFreeCompactHeightfield(chf);
		return build;
	}

	// 11. Contour Field
	rcContourSet* contourSet = rcAllocContourSet();
	if (!contourSet)
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to ALLOC CONTOUR rcAllocContourSet");
		rcFreeCompactHeightfield(chf);
		return build;
	}

	if (!rcBuildContours(&ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *contourSet))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD CONTOUR rcAllocContourSet");
		rcFreeContourSet(contourSet);
		rcFreeCompactHeightfield(chf);
		return build;
	}

	// 12. Polygon Mesh
	rcPolyMesh* polyMesh = rcAllocPolyMesh();
	if (!polyMesh)
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to ALLOC POLYGON MESH rcAllocPolyMesh");
		rcFreeContourSet(contourSet);
		rcFreeCompactHeightfield(chf);
		return build;
	}
	if (!rcBuildPolyMesh(&ctx, *contourSet, cfg.maxVertsPerPoly, *polyMesh))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD POLYGON MESH rcBuildPolyMesh");
		rcFreePolyMesh(polyMesh);
		rcFreeContourSet(contourSet);
		rcFreeCompactHeightfield(chf);
		return build;
	}
	rcFreeContourSet(contourSet);
	contourSet = nullptr;

	// 13. Detail Mesh
	rcPolyMeshDetail* detailMesh = rcAllocPolyMeshDetail();
	if (!detailMesh)
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to ALLOC DETAIL MESH rcAllocPolyMeshDetail");
		rcFreePolyMesh(polyMesh);
		rcFreeCompactHeightfield(chf);
		return build;
	}
	if (!rcBuildPolyMeshDetail(&ctx, *polyMesh, *chf,
		cfg.detailSampleDist, cfg.detailSampleMaxError, *detailMesh))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD DETAIL MESH rcBuildPolyMeshDetail");
		rcFreePolyMeshDetail(detailMesh);
		rcFreePolyMesh(polyMesh);
		rcFreeCompactHeightfield(chf);
		return build;
	}
	rcFreeCompactHeightfield(chf);
	chf = nullptr;

	// 14. Get NAV CELL
	ExtractCells(*polyMesh, *detailMesh, build.navCells);
	SetUp_Neighbors(build.navCells);

#ifdef _DEBUG
	LOG_INFO(L"[NavMeshBuilder] NavMesh 빌드 완료: {} cells", build.navCells.size());
#endif

	// Clear
	rcFreePolyMeshDetail(detailMesh);
	rcFreePolyMesh(polyMesh);

	build.isSuccess = true;

	return build;
}

void NavigationBuilder::ExtractCells(const rcPolyMesh& mesh, const rcPolyMeshDetail& detail, vector<NavCell>& outCells) const
{
	outCells.clear();

	int32 cellIndex = 0;

	for (int32 i = 0; i < detail.nmeshes; ++i)
	{
		const uint32* meshDef = &detail.meshes[i * 4];
		uint32 vertexBase = meshDef[0];
		uint32 triangleBase = meshDef[2];
		uint32 numTriangles = meshDef[3];

		for (uint32 j = 0; j < numTriangles; ++j)
		{
			const uByte* triangle = &detail.tris[(triangleBase + j) * 4];

			const Float* v0 = &detail.verts[(vertexBase + triangle[0]) * 3];
			const Float* v1 = &detail.verts[(vertexBase + triangle[1]) * 3];
			const Float* v2 = &detail.verts[(vertexBase + triangle[2]) * 3];

			Vector3 a(v0[0], v0[1], v0[2]);
			Vector3 b(v1[0], v1[1], v1[2]);
			Vector3 c(v2[0], v2[1], v2[2]);

			outCells.emplace_back(a, b, c, cellIndex++);
		}
	}
}

void NavigationBuilder::SetUp_Neighbors(vector<NavCell>& cells) const
{
	for (size_t i = 0; i < cells.size(); ++i)
	{
		for (size_t j = 0; j < cells.size(); ++j)
		{
			if (i == j) continue;

			auto& src = cells[i];
			auto& dst = cells[j];

			if (dst.Compare_Points(src.Get_Point(NavCell::CELL_POINT::A), src.Get_Point(NavCell::CELL_POINT::B)))
			{
				src.Set_Neighbor(NavCell::CELL_LINE::AB, static_cast<int32>(j));
			}

			if (dst.Compare_Points(src.Get_Point(NavCell::CELL_POINT::B), src.Get_Point(NavCell::CELL_POINT::C)))
			{
				src.Set_Neighbor(NavCell::CELL_LINE::BC, static_cast<int32>(j));
			}

			if (dst.Compare_Points(src.Get_Point(NavCell::CELL_POINT::C), src.Get_Point(NavCell::CELL_POINT::A)))
			{
				src.Set_Neighbor(NavCell::CELL_LINE::CA, static_cast<int32>(j));
			}
		}
	}
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

