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
	m_Bones.clear();
	m_Meshes.clear();
	m_Material.clear();

	string modelFilePath(path.begin(), path.end());

	m_AiScene = m_Importer->ReadFile(
		modelFilePath,
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices
	);

	if (!m_AiScene || m_AiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cerr << "[Assimp] Error: " << m_Importer->GetErrorString() << "\n";
		return false;
	}
	std::cout << "  mNumMeshes=" << m_AiScene->mNumMeshes
		<< " mNumMaterials=" << m_AiScene->mNumMaterials << "\n";

	// TODO : Anim뜨면 Bone 적용
	

	ReadMeshData();
	ReadMaterialData();
	ReadBoneData(m_AiScene->mRootNode, 0, -1);

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

void Tool::Converter::ReadBoneData(aiNode* node, int32_t index, int32_t parent)
{
	auto bone = make_shared<MODEL_BONE>();
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

	


	size_t boneIndex = m_Bones.size() - 1;
	for (uint32 i = 0; i < node->mNumChildren; ++i) {
		ReadBoneData(node->mChildren[i], static_cast<int32_t>(boneIndex), static_cast<int32_t>(boneIndex));
	}
}

void Tool::Converter::ReadMeshData()
{
	Bool isAnim = m_AiScene->HasAnimations();

	for (uint32 i = 0; i < m_AiScene->mNumMeshes; ++i)
	{
		auto aiMesh = m_AiScene->mMeshes[i];
		auto mesh = make_shared<MODEL_MESH>();
		
		mesh->name = aiMesh->mName.C_Str();
		if (mesh->name.empty())
			mesh->name = "Mesh_" + std::to_string(i);
		mesh->materialIndex = aiMesh->mMaterialIndex;

		/* vertices */
		if (isAnim)
		{
			mesh->animVertices.resize(aiMesh->mNumVertices);
			for (uint32 j = 0; j < aiMesh->mNumVertices; ++j)
			{
				VTXANIMMESH& vtxMesh = mesh->animVertices[j];
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
					vtxMesh.texcoord = Vector2{ aiTexcoord.x, aiTexcoord.y };
				}

				vtxMesh.blendIndex = Vector4{ 0.f, 0.f, 0.f, 0.f };
				vtxMesh.blendWeight = Vector4{ 0.f, 0.f, 0.f, 0.f };
			}
		}
		else
		{
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
					vtxMesh.texcoord = Vector2{ aiTexcoord.x, aiTexcoord.y };
				}
			}
		}

		/* Animation / Bone Info */
		if (isAnim)
		{
			mesh->numBones = aiMesh->mNumBones;
			for (uint32 b = 0; b < aiMesh->mNumBones; ++b)
			{
				aiBone* pAIBone = aiMesh->mBones[b];
				
				// 글로벌 본 리스트에서 해당 본의 인덱스 찾기
				uint32 globalBoneIndex = 0;
				for (uint32 g = 0; g < m_Bones.size(); ++g)
				{
					if (m_Bones[g]->name == pAIBone->mName.C_Str())
					{
						globalBoneIndex = g;
						break;
					}
				}
				mesh->boneIndices.push_back(globalBoneIndex);

				for (uint32 w = 0; w < pAIBone->mNumWeights; ++w)
				{
					aiVertexWeight vertexWeight = pAIBone->mWeights[w];
					uint32 vId = vertexWeight.mVertexId;

					if (mesh->animVertices[vId].blendWeight.x == 0.f) {
						mesh->animVertices[vId].blendIndex.x = static_cast<Float>(b);
						mesh->animVertices[vId].blendWeight.x = vertexWeight.mWeight;
					}
					else if (mesh->animVertices[vId].blendWeight.y == 0.f) {
						mesh->animVertices[vId].blendIndex.y = static_cast<Float>(b);
						mesh->animVertices[vId].blendWeight.y = vertexWeight.mWeight;
					}
					else if (mesh->animVertices[vId].blendWeight.z == 0.f) {
						mesh->animVertices[vId].blendIndex.z = static_cast<Float>(b);
						mesh->animVertices[vId].blendWeight.z = vertexWeight.mWeight;
					}
					else if (mesh->animVertices[vId].blendWeight.w == 0.f) {
						mesh->animVertices[vId].blendIndex.w = static_cast<Float>(b);
						mesh->animVertices[vId].blendWeight.w = vertexWeight.mWeight;
					}
				}
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
}

void Tool::Converter::ReadMaterialData()
{
	for (uint32 i = 0; i < m_AiScene->mNumMaterials; ++i)
	{
		const aiMaterial* aiMat = m_AiScene->mMaterials[i];
		auto mat = make_shared<MODEL_MATERIAL>();

		aiString name;
		aiMat->Get(AI_MATKEY_NAME, name);
		mat->name = name.C_Str();

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

				MODEL_ENTRY entry;
				entry.typeIndex = t;

				// 절대경로 혼재 방지: 순수 파일명(ex: "diffuse.dds")만 파싱해서 저장
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
	MODEL_HEADER header{};
	memcpy(header.magic, "NMDL", 4);
	header.version = 1;
	header.isAnim = m_AiScene->HasAnimations();
	header.numBones = static_cast<uint32>(m_Bones.size());
	header.numMeshes = static_cast<uint32>(m_Meshes.size());
	header.numMaterials = static_cast<uint32>(m_Material.size());
	out.write(BIN(&header), sizeof(header));

	/* Meshes */
	for (auto& mesh : m_Meshes)
	{
		uint32 nameLength = static_cast<uint32>(mesh->name.size());
		out.write(BIN(&nameLength), sizeof(uint32));
		out.write(mesh->name.data(), nameLength);
		out.write(BIN(&mesh->materialIndex), sizeof(uint32));

		// numBones 및 boneIndices 기록
		out.write(BIN(&mesh->numBones), sizeof(uint32));
		if (mesh->numBones > 0)
		{
			out.write(BIN(mesh->boneIndices.data()), mesh->numBones * sizeof(uint32));
		}

		uint32 numVertex = (header.isAnim) ? static_cast<uint32>(mesh->animVertices.size()) : static_cast<uint32>(mesh->vertices.size());
		uint32 numIndex = static_cast<uint32>(mesh->indices.size());
		out.write(BIN(&numVertex), sizeof(uint32));
		out.write(BIN(&numIndex), sizeof(uint32));

		if (header.isAnim)
		{
			out.write(BIN(mesh->animVertices.data()), numVertex * sizeof(Engine::VTXANIMMESH));
		}
		else
		{
			out.write(BIN(mesh->vertices.data()), numVertex * sizeof(Engine::VTXMESH));
		}
		
		out.write(BIN(mesh->indices.data()), numIndex * sizeof(uint32));
	}

	/* Materials */
	uint32 maxTextureType = AI_TEXTURE_TYPE_MAX;
	for (auto& mat : m_Material)
	{
		uint32 nameLength = static_cast<uint32>(mat->name.size());
		out.write(reinterpret_cast<const Char*>(&nameLength), sizeof(uint32));
		out.write(mat->name.data(), nameLength);
		out.write(BIN(&maxTextureType), sizeof(uint32));
		
		uint32 numTex = static_cast<uint32>(mat->textures.size());
		out.write(BIN(&numTex), sizeof(uint32));
		for (auto& texture : mat->textures)
		{
			uint32 pathLength = static_cast<uint32>(texture.path.size());
			out.write(BIN(&texture.typeIndex), sizeof(uint32));
			out.write(BIN(&pathLength), sizeof(uint32));
			out.write(texture.path.data(), pathLength);
		}
	}

	/* Bones */
	for (auto& bone : m_Bones)
	{
		uint32 length = static_cast<uint32>(bone->name.size());
		out.write(BIN(&length), sizeof(uint32));
		out.write(bone->name.data(), length);
		out.write(BIN(&bone->index), sizeof(int32));
		out.write(BIN(&bone->parent), sizeof(int32));
		out.write(BIN(&bone->transform), sizeof(Matrix));
	}

	out.close();
}