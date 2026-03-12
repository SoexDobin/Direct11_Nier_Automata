#ifndef Engine_ModelStruc_h__
#define Engine_ModelStruct_h__

namespace Engine
{
	constexpr char MODEL_MAGIC[4] = { 'N','M','D','L' };
	constexpr uint32 MODEL_VERSION = 1;

	typedef struct MatTexEntry
	{
		uint32_t typeIndex;
		std::string path;
	} MODEL_ENTRY;

	typedef struct ModelFileHeader
	{
		Char   magic[4];       // "NMDL"포멧
		uint32 version;        // 1
		Bool   isAnim;
		uint32 numBones;
		uint32 numMeshes;
		uint32 numMaterials;
		uint32 numAnimations;
	} MODEL_HEADER;

	typedef struct BoneData
	{
		std::string name;
		int32_t index{ 0 };
		int32_t parent{ -1 };
		Matrix transform{};
	} MODEL_BONE;

	typedef struct MeshData
	{
		std::string name;
		int32 materialIndex{ 0 };
		uint32 numBones{ 0 };
		std::vector<uint32> boneIndices;
		std::vector<VTXMESH> vertices;       // Non-Anim Vertices
		std::vector<VTXANIMMESH> animVertices; // Anim Vertices
		std::vector<uint32> indices;
	} MODEL_MESH;

	typedef struct WeightData
	{
		uint32 vertexID;
		Float weight;
	} MODEL_WEIGHT;

	typedef struct MaterialData {
		std::string name;
		uint32 textureTypeMax;
		std::string directoryPath;
		std::vector<MODEL_ENTRY> textures;
	} MODEL_MATERIAL;

	typedef struct AnimationData
	{
		std::string name;
	} MODEL_ANIMATION;
}

#endif