#include "pch.h"
#include "Converter.h"

#include <fstream>
#include <iostream>

using namespace Tool;

void Tool::Converter::Initialize()
{
	if (m_Importer) return;
	m_Importer = make_shared<Assimp::Importer>();
}

Bool Tool::Converter::ReadAssetFile(const wstring& path)
{
	string modelFilePath(path.begin(), path.end());

	m_AiScene = m_Importer->ReadFile(
		modelFilePath,
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (!m_AiScene || m_AiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cerr << "[Assimp] Error: " << m_Importer->GetErrorString() << "\n";
		return false;
	}
	std::cout << "  mNumMeshes=" << m_AiScene->mNumMeshes
		<< " mNumMaterials=" << m_AiScene->mNumMaterials << "\n";

	// TODO : Anim뜨면 Bone 적용
	//ReadModelData(m_AiScene->mRootNode, 0, -1);

	ReadMeshData(m_AiScene->mRootNode, 0);
	ReadMaterialData();

	return true;
}

Bool Tool::Converter::ExportModel(const wstring& outPath)
{
	if (m_Meshes.empty())
	{
		std::cerr << "  [Warning] Mesh 없음, 스킵\n";
		return false;
	}

	WriteModelFile(outPath);
	return true;
}

void Tool::Converter::ExportMaterialData(const wstring& outPath)
{
}

void Tool::Converter::ReadModelData(aiNode* node, int32_t index, int32_t parent)
{
	auto bone = make_shared<AS_BONE>();
	bone->name = node->mName.C_Str();
	bone->index = index;
	bone->parent = parent;

	auto m = node->mTransformation;
	Matrix matrix(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
	);
	XMStoreFloat4x4(&bone->transform, matrix);

	m_Bones.push_back(bone);

	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		ReadModelData(node->mChildren[i], static_cast<int32_t>(m_Bones.size()), index);
	}
}

void Tool::Converter::ReadMeshData(aiNode* node, int32_t boneIndex)
{
	for (uint32 i = 0; i < node->mNumMeshes; ++i)
	{
		auto aiMesh = m_AiScene->mMeshes[node->mMeshes[i]];
		auto mesh = make_shared<AS_MESH>();

		mesh->name = aiMesh->mName.C_Str();
		mesh->boneIndex = boneIndex;
		mesh->materialIndex = aiMesh->mMaterialIndex;

		/* vertices */
		mesh->vertices.resize(aiMesh->mNumVertices);
		for (uint32 j = 0; j < aiMesh->mNumVertices; ++j)
		{
			VTXMESH& vtxMesh = mesh->vertices[j];
			auto aiPos = aiMesh->mVertices[j];
			vtxMesh.position = Vector3{ aiPos.x, aiPos.y, aiPos.z };

			if (aiMesh->HasNormals()) {
				auto aiNorm = aiMesh->mNormals[j];
				vtxMesh.normal = Vector3{ aiNorm.x, aiNorm.y, aiNorm.z };
			}
				
			if (aiMesh->HasTangentsAndBitangents()) {
				auto aiTangent = aiMesh->mTangents[j];
				vtxMesh.tangent = Vector3{ aiTangent.x, aiTangent.y, aiTangent.z };
			}
				
			if (aiMesh->HasTextureCoords(0)) {
				auto aiTexcoord = aiMesh->mTextureCoords[0][j];
				vtxMesh.texcoord = Vector2{ aiTexcoord.x, aiTexcoord.y };  // z 제거
			}
		}

		for (uint32 j = 0; j < aiMesh->mNumFaces; ++j)
		{
			const aiFace& face = aiMesh->mFaces[j];
			for (uint32 k = 0; k < face.mNumIndices; ++k)
			{
				mesh->indices.push_back(face.mIndices[k]);
			}
		}

		m_Meshes.push_back(mesh);
	}

	for (uint32 i = 0; i < node->mNumChildren; ++i)
		ReadMeshData(node->mChildren[i], boneIndex);
}

void Tool::Converter::ReadMaterialData()
{
	for (uint32 i = 0; i < m_AiScene->mNumMaterials; ++i)
	{
		const aiMaterial* aiMat = m_AiScene->mMaterials[i];
		auto mat = make_shared<AS_MATERIAL>();
		aiString name;
		aiMat->Get(AI_MATKEY_NAME, name);
		mat->name = name.C_Str();
		aiColor4D color;
		if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color)) mat->ambient = ToFloat4(color);
		if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color)) mat->diffuse = ToFloat4(color);
		if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color)) mat->specular = ToFloat4(color);
		if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color)) mat->emissive = ToFloat4(color);
		// AI_TEXTURE_TYPE_MAX 전체 순회
		for (uint32 t = 0; t < AI_TEXTURE_TYPE_MAX; ++t)
		{
			const aiTextureType texType = static_cast<aiTextureType>(t);
			const uint32        numTex = aiMat->GetTextureCount(texType);
			if (0 == numTex) continue;
			for (uint32 j = 0; j < numTex; ++j)
			{
				aiString texPath;
				if (AI_SUCCESS != aiMat->GetTexture(texType, j, &texPath))
					continue;
				AS_TEX_ENTRY entry;
				entry.typeIndex = t;
				// 절대경로 혼재 방지: 파일명만 저장
				entry.path = filesystem::path(texPath.C_Str()).filename().string();
				mat->textures.push_back(entry);
			}
		}
		m_Material.push_back(mat);
	}
}

void Tool::Converter::WriteModelFile(const wstring& path)
{
	filesystem::create_directories(filesystem::path(path).parent_path());
	ofstream out(path, std::ios::binary);

	/* Header */
	ModelFileHeader header{};
	memcpy(header.magic, "NMDL", 4);
	header.version = 1;
	header.numBones = static_cast<uint32>(m_Bones.size());
	header.numMeshes = static_cast<uint32>(m_Meshes.size());
	header.numMaterials = static_cast<uint32>(m_Material.size());
	out.write(reinterpret_cast<const Char*>(&header), sizeof(header));

	/* Bones */
	for (auto& bone : m_Bones)
	{
		uint32 length = static_cast<uint32>(bone->name.size());
		out.write(reinterpret_cast<const Char*>(&length), sizeof(uint32));
		out.write(bone->name.data(), length);
		out.write(reinterpret_cast<const Char*>(&bone->parent), sizeof(int32));
		out.write(reinterpret_cast<const Char*>(&bone->transform), sizeof(Matrix));
	}

	/* Meshes */
	for (auto& mesh : m_Meshes)
	{
		uint32 nameLength = static_cast<uint32>(mesh->name.size());
		out.write(reinterpret_cast<const Char*>(&nameLength), sizeof(uint32));
		out.write(mesh->name.data(), nameLength);
		out.write(reinterpret_cast<const Char*>(&mesh->boneIndex), sizeof(int32));
		out.write(reinterpret_cast<const Char*>(&mesh->materialIndex), sizeof(uint32));

		uint32 numVertex = static_cast<uint32>(mesh->vertices.size());
		uint32 numIndex = static_cast<uint32>(mesh->indices.size());
		out.write(reinterpret_cast<const Char*>(&numVertex), sizeof(uint32));
		out.write(reinterpret_cast<const Char*>(&numIndex), sizeof(uint32));
		out.write(reinterpret_cast<const Char*>(mesh->vertices.data()), numVertex * sizeof(Engine::VTXMESH));
		out.write(reinterpret_cast<const Char*>(mesh->indices.data()),numIndex * sizeof(uint32));
	}

	/* Materials */
	for (auto& mat : m_Material)
	{
		uint32 nameLen = static_cast<uint32>(mat->name.size());
		Float  amb[4] = { mat->ambient.x,  mat->ambient.y,  mat->ambient.z,  mat->ambient.w };
		Float  dif[4] = { mat->diffuse.x,  mat->diffuse.y,  mat->diffuse.z,  mat->diffuse.w };
		Float  spc[4] = { mat->specular.x, mat->specular.y, mat->specular.z, mat->specular.w };
		Float  emi[4] = { mat->emissive.x, mat->emissive.y, mat->emissive.z, mat->emissive.w };
		uint32 numTex = static_cast<uint32>(mat->textures.size());
		out.write(reinterpret_cast<const Char*>(&nameLen), sizeof(uint32));
		out.write(mat->name.data(), nameLen);
		out.write(reinterpret_cast<const Char*>(amb), sizeof(float) * 4);
		out.write(reinterpret_cast<const Char*>(dif), sizeof(float) * 4);
		out.write(reinterpret_cast<const Char*>(spc), sizeof(float) * 4);
		out.write(reinterpret_cast<const Char*>(emi), sizeof(float) * 4);
		out.write(reinterpret_cast<const Char*>(&numTex), sizeof(uint32));
		for (auto& tex : mat->textures)
		{
			uint32 pathLen = static_cast<uint32>(tex.path.size());
			out.write(reinterpret_cast<const Char*>(&tex.typeIndex), sizeof(uint32));
			out.write(reinterpret_cast<const Char*>(&pathLen), sizeof(uint32));
			out.write(tex.path.data(), pathLen);
		}
	}

	out.close();
}

void Tool::Converter::WriteMaterialFile(const wstring& path)
{
	
}
