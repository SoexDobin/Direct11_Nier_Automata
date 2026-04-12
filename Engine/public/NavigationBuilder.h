#pragma once
#include "EngineManager.h"
#include "NavCell.h"

struct rcPolyMesh;
struct rcPolyMeshDetail;

NS_BEGIN(Engine)

class NavigationBuilder final : public EngineManager
{
public:
	typedef struct tagNavBuildParamsDesc
	{
		Float cellSize				{ 0.2f };
		Float cellHeight			{ 0.2f };
		Float moveableSlopeAngle	{ 45.f };
		Float agentHeight			{ 2.f };
		Float agentRadius			{ 0.6f };
		Float agentMaxClimb			{ 0.8f };
		
		int32 maxEdgeLen			{ 12 };
		int32 minRegionArea			{ 8 };
		int32 mergeRegionArea		{ 20 };
		int32 maxVertexPerPoly		{ 6 };

		Float detailSampleDist		{ 6.f };
		Float detailSampleMaxError	{ 1.f };
		Float maxSimplificationError{ 1.3f };
	} NAV_BUILD_PARAMS_DESC;

	struct NAV_BUILD_RESULT
	{
		vector<NavCell> navCells;
		Bool isSuccess{ false };
	};

private:
	NO_COPY(NavigationBuilder)
public:
	explicit NavigationBuilder();
	~NavigationBuilder() override = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	HRESULT Begin() override;

public:
	/// 소스 메시(position + index 배열)로부터 NavCell 목록을 빌드
	NAV_BUILD_RESULT Build(
		const Float* vertices, int32 numVertices,
		const int32* triangles, int32 numTriangles,
		const NAV_BUILD_PARAMS_DESC&params = {}
	);
	/// rcPolyMeshDetail 결과로부터 NavCell 생성 + 이웃 자동 연결
	void ExtractCells(
		const rcPolyMesh& mesh,
		const rcPolyMeshDetail& detail,
		_Out_ vector<NavCell>& outCells
	);
	/// NavCell 목록에서 공유 엣지를 찾아 이웃을 자동 연결
	void SetUp_Neighbors(vector<NavCell>& cells);

public:
	static Unique<NavigationBuilder> Create();

};

NS_END