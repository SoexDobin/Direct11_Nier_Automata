#ifndef Engine_NavStruc_h__
#define Engine_NavStruct_h__

namespace Engine
{
	/// .nnav 헤더. bakeWorldMatrix는 bake 당시 anchor GameObject의 월드 행렬이며,
	/// 셀이 어느 공간에 있는지를 파일 스스로 증명한다. identity면 오프셋 없는 로컬 공간이라는 뜻이다.
	/// 레이아웃에 패딩이 없어야 하므로(디스크 이미지와 1:1) 아래 static_assert로 고정한다.
	struct NavMeshHeader
	{
		char     magic[4]{ 'N','N','A','V' };   // 매직 넘버
		uint32_t numCells{ 0 };
		float    bmin[3]{};                      // 셀 AABB min (디버그/검증용)
		float    bmax[3]{};                      // 셀 AABB max

		float    bakeWorldMatrix[16]{ 1.f, 0.f, 0.f, 0.f,
									  0.f, 1.f, 0.f, 0.f,
									  0.f, 0.f, 1.f, 0.f,
									  0.f, 0.f, 0.f, 1.f };
		char     anchorObjectGuid[40]{};        // Engine::To_String(ObjectGuid), 비어 있으면 anchor 미지정
		char     anchorObjectName[64]{};
		char     sourceModelTag[64]{};
	};

	static_assert(sizeof(NavMeshHeader) == 264, "NNAV header layout must stay packed at 264 bytes");

	struct NavCellBinary
	{
		Vector3   vertices[3];
		int32_t   neighborIndices[3]{ -1, -1, -1 };  // Recast에서 직접 추출한 이웃
	};
}

#endif
