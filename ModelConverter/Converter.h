#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


NS_BEGIN(Engine)
struct BoneData;
struct MeshData;
struct MaterialData;
struct AnimationData;
NS_END

NS_BEGIN(Tool)
	class Converter final
{
public:
	explicit Converter() = default;
	~Converter() = default;

public:
	void Initialize();

public:
	uint32 GetMeshCount()     const { return static_cast<uint32>(m_Meshes.size()); }
	uint32 GetMaterialCount() const { return static_cast<uint32>(m_Material.size()); }
	uint32 GetBoneCount()     const { return static_cast<uint32>(m_Bones.size()); }
	uint32 GetAnimationCount()     const { return static_cast<uint32>(m_Animation.size()); }

public:
	Bool ReadAssetFile(const wstring& path);
	Bool ExportModel(const wstring& outPath);

private:
	void ReadBoneData(aiNode* node, int32_t parentIndex);
	void ReadMeshData();
	void ReadMaterialData();
	void ReadAnimation();

private:
	void WriteModelFile(const wstring& path);

private:
	int32 Get_BoneIndex(const Char* boneName);

private:
	Shared<Assimp::Importer>		m_Importer{nullptr};
	const aiScene*					m_AiScene{nullptr};

	Bool m_IsSkeletal{ false };
	vector<Shared<MODEL_BONE>>					m_Bones;
	vector<Shared<MODEL_MESH>>					m_Meshes;
	vector<Shared<MODEL_MATERIAL>>				m_Material;
	vector<Shared<MODEL_ANIMATION>>				m_Animation;
	vector<vector<Shared<MODEL_CHANNEL>>>		m_Channels;

public: /* static */
	static Vector4 ToFloat4(const aiColor4D& aiColor)
	{
		return { aiColor.r, aiColor.g, aiColor.b, aiColor.a };
	}
};

NS_END

/*
	typedef struct ChanelData
	{
		std::string name;
		uint32 numKeyFrames;
		std::vector<KEYFRAME> keyFrames;
	} MODEL_CHANNEL;

	typedef struct AnimationData
	{
		std::string name;
		Float duration;
		Float tickPerSecond;
		uint32 numChannel;
		std::vector<MODEL_CHANNEL> channels;
	} MODEL_ANIMATION;

 */