#ifndef Engine_NavStruc_h__
#define Engine_NavStruct_h__

namespace Engine
{
	struct NavMeshHeader
	{
		char     magic[4]{ 'N','N','A','V' };   // 매직 넘버
		uint32_t version{ 1 };
		uint32_t numCells{ 0 };
		float    bmin[3]{};                      // 월드 AABB min (디버그/검증용)
		float    bmax[3]{};                      // 월드 AABB max
	};

	struct NavCellBinary
	{
		Vector3   vertices[3];
		int32_t   neighborIndices[3]{ -1, -1, -1 };  // Recast에서 직접 추출한 이웃
	};
}

#endif
