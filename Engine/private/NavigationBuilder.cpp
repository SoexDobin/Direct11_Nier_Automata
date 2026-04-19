#include "NavigationBuilder.h"

#include <fstream>

#include "SpdLogger.h"
#include <recastnavigation/Recast.h>
#include <recastnavigation/RecastAlloc.h>
#include "Model.h"
#include "String_Helper.h"

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

vector<NavCellBinary> NavigationBuilder::Bake_Navigation(Shared<Model> model, const Matrix& worldMatrix, const rcConfig& config)
{
	return Bake_Internal(model, worldMatrix, config, false);
}

HRESULT NavigationBuilder::Export_Binary(const string& fileName, Shared<Model> model, const Matrix& worldMatrix, const rcConfig& config)
{
	vector<NavCellBinary> bakedData = Bake_Internal(model, worldMatrix, config, true);
	if (bakedData.empty())
	{
		MSG_BOX("[ NavMesh Builder Export ] There is no vertex or just failed");
		return E_FAIL;
	}

	NavMeshHeader header{};
	memcpy(header.magic, "NNAV", 4);
	header.version = 1;
	header.numCells = static_cast<uint32>(bakedData.size());

	// AABB 계산
	Vector3 bmin{ FLT_MAX, FLT_MAX, FLT_MAX };
	Vector3 bmax{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
	for (auto& cell : bakedData)
	{
		for (int32 j = 0; j < 3; ++j)
		{
			bmin = Vector3::Min(bmin, cell.vertices[j]);
			bmax = Vector3::Max(bmax, cell.vertices[j]);
		}
	}
	header.bmin[0] = bmin.x; header.bmin[1] = bmin.y; header.bmin[2] = bmin.z;
	header.bmax[0] = bmax.x; header.bmax[1] = bmax.y; header.bmax[2] = bmax.z;

	std::ofstream fout(fileName, std::ios::binary);
	if (!fout.is_open()) return E_FAIL;

	fout.write(reinterpret_cast<Char*>(&header), sizeof(NavMeshHeader));
	fout.write(reinterpret_cast<Char*>(bakedData.data()), sizeof(NavCellBinary) * bakedData.size());
	fout.close();

	return S_OK;
}

vector<NavCellBinary> NavigationBuilder::Bake_Internal(Shared<Model> model, const Matrix& worldMatrix, rcConfig config, Bool computeNeighbors)
{
	vector<NavCellBinary> resultData;
	if (model == nullptr) return resultData;
	vector<Float> rawPos;
	vector<int32_t> rawIndices;
	model->Extract_RawMeshData(rawPos, rawIndices);
	int32 numVertices = static_cast<int32>(rawPos.size() / 3);
	int32 numTris = static_cast<int32>(rawIndices.size() / 3);
	if (numVertices == 0 || numTris == 0) return resultData;
	vector<Float> worldVertices(rawPos.size());
	for (int32 i = 0; i < numVertices; ++i)
	{
		Vector3 localPos(rawPos[i * 3], rawPos[i * 3 + 1], rawPos[i * 3 + 2]);
		Vector3 worldPos = Vector3::Transform(localPos, worldMatrix);
		worldVertices[i * 3] = worldPos.x;
		worldVertices[i * 3 + 1] = worldPos.y;
		worldVertices[i * 3 + 2] = worldPos.z;
	}
	rcContext ctx;
	config.maxVertsPerPoly = 3;
	rcCalcBounds(worldVertices.data(), numVertices, config.bmin, config.bmax);
	rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);
	rcHeightfield* solid = nullptr;
	rcCompactHeightfield* chf = nullptr;
	rcContourSet* contour = nullptr;
	rcPolyMesh* polyMesh = nullptr;
	rcPolyMeshDetail* detailMesh = nullptr;
	uByte* triAreas = nullptr;
	Bool isBuildSuccess = false;
	solid = rcAllocHeightfield();
	if (solid && rcCreateHeightfield(&ctx, *solid, config.width, config.height,
		config.bmin, config.bmax, config.cs, config.ch))
	{
		triAreas = new unsigned char[numTris];
		memset(triAreas, 0, numTris);
		rcMarkWalkableTriangles(&ctx, config.walkableSlopeAngle,
			worldVertices.data(), numVertices, rawIndices.data(), numTris, triAreas);
		if (!rcRasterizeTriangles(&ctx, worldVertices.data(), numVertices,
			rawIndices.data(), triAreas, numTris, *solid, config.walkableClimb))
		{
			delete[] triAreas;
			rcFreeHeightField(solid);
			return resultData;
		}
		rcFilterLowHangingWalkableObstacles(&ctx, config.walkableClimb, *solid);
		rcFilterLedgeSpans(&ctx, config.walkableHeight, config.walkableClimb, *solid);
		rcFilterWalkableLowHeightSpans(&ctx, config.walkableHeight, *solid);
		chf = rcAllocCompactHeightfield();
		if (!chf || !rcBuildCompactHeightfield(&ctx, config.walkableHeight,
			config.walkableClimb, *solid, *chf))
		{
			rcFreeHeightField(solid);
			delete[] triAreas;
			return resultData;
		}
		if (!rcErodeWalkableArea(&ctx, config.walkableRadius, *chf))
		{
			rcFreeCompactHeightfield(chf);
			rcFreeHeightField(solid);
			delete[] triAreas;
			return resultData;
		}
		if (!rcBuildDistanceField(&ctx, *chf) ||
			!rcBuildRegions(&ctx, *chf, 0, config.minRegionArea, config.mergeRegionArea))
		{
			rcFreeCompactHeightfield(chf);
			rcFreeHeightField(solid);
			delete[] triAreas;
			return resultData;
		}
		contour = rcAllocContourSet();
		if (!contour || !rcBuildContours(&ctx, *chf,
			config.maxSimplificationError, config.maxEdgeLen, *contour))
		{
			rcFreeCompactHeightfield(chf);
			rcFreeHeightField(solid);
			delete[] triAreas;
			return resultData;
		}
		polyMesh = rcAllocPolyMesh();
		if (polyMesh && rcBuildPolyMesh(&ctx, *contour, config.maxVertsPerPoly, *polyMesh))
		{
			// ★ DetailMesh 생성: 통 삼각형 내부에 보간용 작은 삼각형을 만들어줌
			detailMesh = rcAllocPolyMeshDetail();
			if (detailMesh)
			{
				if (!rcBuildPolyMeshDetail(&ctx, *polyMesh, *chf,
					config.detailSampleDist, config.detailSampleMaxError, *detailMesh))
				{
					rcFreePolyMeshDetail(detailMesh);
					detailMesh = nullptr;
				}
			}
			isBuildSuccess = true;
		}
	}
	if (isBuildSuccess && polyMesh)
	{
		if (detailMesh)
		{
			// ★ DetailMesh의 서브 삼각형 전체 추출 로직 ★
			int32 totalDetailTris = 0;
			for (int32 i = 0; i < polyMesh->npolys; ++i)
				totalDetailTris += detailMesh->meshes[i * 4 + 3];
			resultData.reserve(totalDetailTris);
			for (int32 polyIdx = 0; polyIdx < polyMesh->npolys; ++polyIdx)
			{
				const uint32 baseVert = detailMesh->meshes[polyIdx * 4 + 0];
				const uint32 baseTri = detailMesh->meshes[polyIdx * 4 + 2];
				const uint32 triCount = detailMesh->meshes[polyIdx * 4 + 3];
				for (uint32 t = 0; t < triCount; ++t)
				{
					NavCellBinary cell;
					const uByte* dtri = &detailMesh->tris[(baseTri + t) * 4];
					for (int32 j = 0; j < 3; ++j)
					{
						const float* dv = &detailMesh->verts[(baseVert + dtri[j]) * 3];
						cell.vertices[j].x = dv[0];
						cell.vertices[j].y = dv[1]; // float precision
						cell.vertices[j].z = dv[2];
					}
					// 서브 삼각형 초기화
					cell.neighborIndices[0] = -1;
					cell.neighborIndices[1] = -1;
					cell.neighborIndices[2] = -1;
					resultData.push_back(cell);
				}
			}

			// ★ 이웃 다시 엮어주기 (O(N^2)) - NavHelper에서 초록색 선(연결)으로 보이게 하기 위함
			int32 numCells = static_cast<int32>(resultData.size());
			constexpr float epsilon = 0.01f;
			auto isEqual = [epsilon](const Vector3& a, const Vector3& b) -> bool {
				return (fabsf(a.x - b.x) < epsilon &&
						fabsf(a.y - b.y) < epsilon &&
						fabsf(a.z - b.z) < epsilon);
			};

			for (int32 i = 0; i < numCells; ++i)
			{
				for (int32 ei = 0; ei < 3; ++ei)
				{
					if (resultData[i].neighborIndices[ei] != -1) continue;

					Vector3 p0 = resultData[i].vertices[ei];
					Vector3 p1 = resultData[i].vertices[(ei + 1) % 3];

					for (int32 j = i + 1; j < numCells; ++j)
					{
						for (int32 ej = 0; ej < 3; ++ej)
						{
							if (resultData[j].neighborIndices[ej] != -1) continue;

							Vector3 q0 = resultData[j].vertices[ej];
							Vector3 q1 = resultData[j].vertices[(ej + 1) % 3];

							if ((isEqual(p0, q0) && isEqual(p1, q1)) ||
								(isEqual(p0, q1) && isEqual(p1, q0)))
							{
								resultData[i].neighborIndices[ei] = j;
								resultData[j].neighborIndices[ej] = i;
								break;
							}
						}
						if (resultData[i].neighborIndices[ei] != -1) break;
					}
				}
			}
		}
		else
		{
			// 혹시라도 DetailMesh 생성 실패시 기존 1단계 양자화 방식(통 삼각형) 폴백
			int32 nvp = polyMesh->nvp;
			resultData.reserve(polyMesh->npolys);
			for (int i = 0; i < polyMesh->npolys; ++i)
			{
				NavCellBinary cell;
				const uShort* p = &polyMesh->polys[i * nvp * 2];
				for (int j = 0; j < 3; ++j)
				{
					if (p[j] == RC_MESH_NULL_IDX) continue;
					const uShort* v = &polyMesh->verts[p[j] * 3];
					cell.vertices[j].x = config.bmin[0] + v[0] * config.cs;
					cell.vertices[j].y = config.bmin[1] + v[1] * config.ch;
					cell.vertices[j].z = config.bmin[2] + v[2] * config.cs;
				}
				for (int32 j = 0; j < 3; ++j)
				{
					unsigned short neighbor = p[nvp + j];
					cell.neighborIndices[j] = (neighbor == RC_MESH_NULL_IDX || neighbor >= polyMesh->npolys) ? -1 : static_cast<int32_t>(neighbor);
				}
				resultData.push_back(cell);
			}
		}
	}
	delete[] triAreas;
	if (solid)      rcFreeHeightField(solid);
	if (chf)        rcFreeCompactHeightfield(chf);
	if (contour)    rcFreeContourSet(contour);
	if (detailMesh) rcFreePolyMeshDetail(detailMesh);
	if (polyMesh)   rcFreePolyMesh(polyMesh);
	return resultData;
}

vector<NavCell> NavigationBuilder::Import_Binary(const string& filePath)
{
	vector<NavCell> cells;
	std::ifstream fin(filePath, std::ios::binary);
	if (!fin.is_open()) return cells;
	NavMeshHeader header{};
	fin.read(reinterpret_cast<char*>(&header), sizeof(NavMeshHeader));
	if (memcmp(header.magic, "NNAV", 4) != 0) return cells;
	cells.reserve(header.numCells);
	for (uint32 i = 0; i < header.numCells; ++i)
	{
		NavCellBinary bin{};
		fin.read(reinterpret_cast<char*>(&bin), sizeof(NavCellBinary));
		NavCell cell(bin.vertices[0], bin.vertices[1], bin.vertices[2], static_cast<int32>(i));
		cell.Set_Neighbor(NavCell::CELL_LINE::AB, bin.neighborIndices[0]);
		cell.Set_Neighbor(NavCell::CELL_LINE::BC, bin.neighborIndices[1]);
		cell.Set_Neighbor(NavCell::CELL_LINE::CA, bin.neighborIndices[2]);
		cells.push_back(std::move(cell));
	}
	return cells;
}

// ─── Build (런타임용) ── rcPolyMesh 기반, Recast 이웃 직접 사용 ───
NavigationBuilder::NAV_BUILD_RESULT NavigationBuilder::Build(const Float* vertices, int32 numVertices,
                                                             const int32* triangles, int32 numTriangles, 
                                                             const NAV_BUILD_PARAMS_DESC& params)
{
	NAV_BUILD_RESULT build{};
	rcContext ctx{};
	rcConfig cfg{};
	// 1. rcConfig 구성
	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = params.cellSize;
	cfg.ch = params.cellHeight;
	cfg.walkableSlopeAngle = params.moveableSlopeAngle;
	cfg.walkableHeight = static_cast<int32>(ceilf(params.agentHeight / cfg.ch));
	cfg.walkableClimb = static_cast<int32>(floorf(params.agentMaxClimb / cfg.ch));
	cfg.walkableRadius = static_cast<int32>(ceilf(params.agentRadius / cfg.cs));
	cfg.maxEdgeLen = static_cast<int32>(params.maxEdgeLen / cfg.cs);
	cfg.minRegionArea = params.minRegionArea * params.minRegionArea;
	cfg.mergeRegionArea = params.mergeRegionArea * params.mergeRegionArea;
	cfg.maxVertsPerPoly = 3;
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
		rcFreeHeightField(solid);
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
	// 7. Compact
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
	// 8. Agent Erode
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
	// 10. Region
	if (!rcBuildRegions(&ctx, *chf, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD REGION rcBuildRegions");
		rcFreeCompactHeightfield(chf);
		return build;
	}
	// 11. Contour
	rcContourSet* contourSet = rcAllocContourSet();
	if (!contourSet)
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to ALLOC CONTOUR rcAllocContourSet");
		rcFreeCompactHeightfield(chf);
		return build;
	}
	if (!rcBuildContours(&ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *contourSet))
	{
		LOG_ERROR(L"[NavMeshBuilder] failed to BUILD CONTOUR rcBuildContours");
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
	// ★ 13. Detail Mesh 빌드 (chf 해제 전!)
	rcPolyMeshDetail* detailMesh = rcAllocPolyMeshDetail();
	if (detailMesh)
	{
		if (!rcBuildPolyMeshDetail(&ctx, *polyMesh, *chf,
			cfg.detailSampleDist, cfg.detailSampleMaxError, *detailMesh))
		{
			rcFreePolyMeshDetail(detailMesh);
			detailMesh = nullptr;
		}
	}
	// ★ chf 해제를 DetailMesh 빌드 이후로 이동
	rcFreeCompactHeightfield(chf);
	// 14. NavCell 추출 (DetailMesh 전달)
	ExtractCells_FromPolyMesh(*polyMesh, cfg, detailMesh, build.navCells);
#ifdef _DEBUG
	LOG_INFO(L"[NavMeshBuilder] NavMesh 빌드 완료: {} cells", build.navCells.size());
#endif
	if (detailMesh) rcFreePolyMeshDetail(detailMesh);
	rcFreePolyMesh(polyMesh);
	build.isSuccess = true;
	return build;
}

void NavigationBuilder::ExtractCells_FromPolyMesh(
	const rcPolyMesh& mesh,
	const rcConfig& cfg,
	const rcPolyMeshDetail* detailMesh,
	vector<NavCell>& outCells)
{
	outCells.clear();
	outCells.clear();

	if (detailMesh)
	{
		// DetailMesh 서브 삼각형 추출
		int32 totalDetailTris = 0;
		for (int32 i = 0; i < mesh.npolys; ++i)
			totalDetailTris += detailMesh->meshes[i * 4 + 3];
		outCells.reserve(totalDetailTris);
		int32 globalIdx = 0;
		for (int32 polyIdx = 0; polyIdx < mesh.npolys; ++polyIdx)
		{
			const uint32 baseVert = detailMesh->meshes[polyIdx * 4 + 0];
			const uint32 baseTri = detailMesh->meshes[polyIdx * 4 + 2];
			const uint32 triCount = detailMesh->meshes[polyIdx * 4 + 3];
			for (uint32 t = 0; t < triCount; ++t)
			{
				const uByte* dtri = &detailMesh->tris[(baseTri + t) * 4];
				Vector3 verts[3];
				for (int32 j = 0; j < 3; ++j)
				{
					const float* dv = &detailMesh->verts[(baseVert + dtri[j]) * 3];
					verts[j].x = dv[0];
					verts[j].y = dv[1];
					verts[j].z = dv[2];
				}
				NavCell cell(verts[0], verts[1], verts[2], globalIdx++);
				// 이웃 정보는 전부 꺼두고 바깥쪽에서 SetUp_Neighbors로 세팅 필요함
				cell.Set_Neighbor(NavCell::CELL_LINE::AB, -1);
				cell.Set_Neighbor(NavCell::CELL_LINE::BC, -1);
				cell.Set_Neighbor(NavCell::CELL_LINE::CA, -1);
				outCells.push_back(std::move(cell));
			}
		}

		// ★ 이웃 다시 엮어주기 (O(N^2))
		int32 numCells = static_cast<int32>(outCells.size());
		constexpr float epsilon = 0.01f;
		auto isEqual = [epsilon](const Vector3& a, const Vector3& b) -> bool {
			return (fabsf(a.x - b.x) < epsilon &&
					fabsf(a.y - b.y) < epsilon &&
					fabsf(a.z - b.z) < epsilon);
		};

		for (int32 i = 0; i < numCells; ++i)
		{
			for (int32 ei = 0; ei < 3; ++ei)
			{
				if (outCells[i].Get_Neighbor(static_cast<NavCell::CELL_LINE>(ei)) != -1) continue;

				Vector3 p0 = outCells[i].Get_Point(static_cast<NavCell::CELL_POINT>(ei));
				Vector3 p1 = outCells[i].Get_Point(static_cast<NavCell::CELL_POINT>((ei + 1) % 3));

				for (int32 j = i + 1; j < numCells; ++j)
				{
					for (int32 ej = 0; ej < 3; ++ej)
					{
						if (outCells[j].Get_Neighbor(static_cast<NavCell::CELL_LINE>(ej)) != -1) continue;

						Vector3 q0 = outCells[j].Get_Point(static_cast<NavCell::CELL_POINT>(ej));
						Vector3 q1 = outCells[j].Get_Point(static_cast<NavCell::CELL_POINT>((ej + 1) % 3));

						if ((isEqual(p0, q0) && isEqual(p1, q1)) ||
							(isEqual(p0, q1) && isEqual(p1, q0)))
						{
							outCells[i].Set_Neighbor(static_cast<NavCell::CELL_LINE>(ei), j);
							outCells[j].Set_Neighbor(static_cast<NavCell::CELL_LINE>(ej), i);
							break;
						}
					}
					if (outCells[i].Get_Neighbor(static_cast<NavCell::CELL_LINE>(ei)) != -1) break;
				}
			}
		}
	}
	else
	{
		const int32 nvp = mesh.nvp;
		outCells.reserve(mesh.npolys);
		for (int32 i = 0; i < mesh.npolys; ++i)
		{
			const uShort* p = &mesh.polys[i * nvp * 2];
			Vector3 verts[3];
			for (int32 j = 0; j < 3; ++j)
			{
				if (p[j] == RC_MESH_NULL_IDX) continue;
				const uShort* v = &mesh.verts[p[j] * 3];
				verts[j].x = cfg.bmin[0] + v[0] * cfg.cs;
				verts[j].y = cfg.bmin[1] + v[1] * cfg.ch;
				verts[j].z = cfg.bmin[2] + v[2] * cfg.cs;
			}
			NavCell cell(verts[0], verts[1], verts[2], i);
			for (int32 j = 0; j < 3; ++j)
			{
				uShort neighbor = p[nvp + j];
				cell.Set_Neighbor(static_cast<NavCell::CELL_LINE>(j),
					(neighbor == RC_MESH_NULL_IDX || neighbor >= mesh.npolys) ? -1 : static_cast<int32>(neighbor));
			}
			outCells.push_back(std::move(cell));
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
