#ifndef Engine_ModelStruc_h__
#define Engine_ModelStruct_h__

#pragma pack(push, 1)
namespace Engine
{
	constexpr char MODEL_MAGIC[4] = { 'N','M','D','L' };
	constexpr uint32 MODEL_VERSION = 1;

	struct ModelFileHeader
	{
		Char   magic[4];       // "NMDL"포멧
		uint32 version;        // 1
		uint32 numBones;
		uint32 numMeshes;
		uint32 numMaterials;
	};

	struct MeshHeader
	{
		uint32 nameLength;     // 메시 이름 길이 (바이트)
		uint32 materialIndex;  // 대응 Material 인덱스
		uint32 numVertices;
		uint32 numIndices;
	};

	// aiTextureType 인덱스 + 상대 경로를 기록
	struct MatTexEntry
	{
		uint32_t typeIndex;      
		uint32_t pathLen;       
		// 뒤에 char[pathLen] 경로 문자열이 이어짐
	};

	struct MaterialHeader
	{
		uint32_t nameLen;        // 머티리얼 이름 길이
		float    ambient[4];
		float    diffuse[4];
		float    specular[4];
		float    emissive[4];
		uint32_t numTextures;    // MatTexEntry 개수
	};
}

#pragma pack(pop)

#endif