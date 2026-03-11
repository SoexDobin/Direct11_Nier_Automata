#pragma once
#include <assimp/Importer.hpp>
#include "ConverterStructs.h"
#include <assimp/postprocess.h>

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

public:
	Bool ReadAssetFile(const wstring& path);
	Bool ExportModel(const wstring& outPath);
	void ExportMaterialData(const wstring& outPath);

private:
	void ReadModelData(aiNode* node, int32_t index, int32_t parent);
	void ReadMeshData(aiNode* node, int32_t boneIndex);
	void ReadMaterialData();

private:
	void WriteModelFile(const wstring& path);
	void WriteMaterialFile(const wstring& path);

private:
	Shared<Assimp::Importer>	m_Importer{nullptr};
	const aiScene*				m_AiScene{nullptr};
	vector<Shared<AS_BONE>>		m_Bones;
	vector<Shared<AS_MESH>>		m_Meshes;
	vector<Shared<AS_MATERIAL>> m_Material;

public: /* static */
	static Vector4 ToFloat4(const aiColor4D& aiColor)
	{
		return { aiColor.r, aiColor.g, aiColor.b, aiColor.a };
	}
};

NS_END