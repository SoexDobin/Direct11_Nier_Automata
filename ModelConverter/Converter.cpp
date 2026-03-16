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
	m_Animation.clear();
	m_Channels.clear();

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
	std::cout << "  mNumMeshes=" << m_AiScene->mNumMeshes << " mNumMaterials=" << m_AiScene->mNumMaterials << "\n";

	m_IsSkeletal = false;
	m_IsSkeletal = m_AiScene->mNumAnimations > 0;

	ReadBoneData(m_AiScene->mRootNode, -1);
	ReadMeshData();
	ReadMaterialData();
	ReadAnimation();

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

void Tool::Converter::ReadBoneData(aiNode* node, int32_t parentIndex)
{
	auto bone = make_shared<MODEL_BONE>();
	bone->name = node->mName.C_Str();
	bone->parentIndex = parentIndex;

	auto m = node->mTransformation;
	Matrix matrix( // Transpose() 재배열
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
	);
	bone->transform = matrix;

	m_Bones.push_back(bone);

	size_t boneIndex = m_Bones.size() - 1;
	for (uint32 i = 0; i < node->mNumChildren; ++i) {
		ReadBoneData(node->mChildren[i], static_cast<int32_t>(boneIndex));
	}
}

void Tool::Converter::ReadMeshData()
{
	for (uint32 i = 0; i < m_AiScene->mNumMeshes; ++i)
	{
		auto aiMesh = m_AiScene->mMeshes[i];
		auto mesh = make_shared<MODEL_MESH>();
		
		mesh->name = aiMesh->mName.C_Str();
		mesh->materialIndex = aiMesh->mMaterialIndex;

		/* Animation / Bone Info */
		if (m_IsSkeletal)
		{
			mesh->animVertices.resize(aiMesh->mNumVertices);
			memset(mesh->animVertices.data(), 0, sizeof(VTXANIMMESH) * aiMesh->mNumVertices);

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
			}

			for (uint32 j = 0; j < aiMesh->mNumBones; ++j)
			{
				aiBone* aiBone = aiMesh->mBones[j];
				
				// 1. 글로벌 본 리스트에서 해당 본의 인덱스 찾기
				int32 globalBoneIndex = Get_BoneIndex(aiBone->mName.C_Str());
				mesh->boneIndices.push_back(static_cast<uint32>(globalBoneIndex));
				if (-1 == globalBoneIndex)
				{
					std::cerr << "[Assimp] Error: Bone Index Error index was -1" << "\n";
					return;
				}

				// 2. 본인덱스 차례의 오프셋 매트릭스 
				Matrix offsetMatrix;
				memcpy(&offsetMatrix, &aiBone->mOffsetMatrix, sizeof(Matrix));
				mesh->offsetMatrices.push_back(offsetMatrix.Transpose());

				// 3. Blend인덱스와 가중치 업데이트
				for (uint32 k = 0; k < aiBone->mNumWeights; ++k)
				{
					aiVertexWeight vertexWeight = aiBone->mWeights[k];
					uint32 weightIndex = vertexWeight.mVertexId;

					if (mesh->animVertices[weightIndex].blendWeight.x == 0.f) {
						mesh->animVertices[weightIndex].blendIndex.x = j;
						mesh->animVertices[weightIndex].blendWeight.x = vertexWeight.mWeight;
					}
					else if (mesh->animVertices[weightIndex].blendWeight.y == 0.f) {
						mesh->animVertices[weightIndex].blendIndex.y = j;
						mesh->animVertices[weightIndex].blendWeight.y = vertexWeight.mWeight;
					}
					else if (mesh->animVertices[weightIndex].blendWeight.z == 0.f) {
						mesh->animVertices[weightIndex].blendIndex.z = j;
						mesh->animVertices[weightIndex].blendWeight.z = vertexWeight.mWeight;
					}
					else if (mesh->animVertices[weightIndex].blendWeight.w == 0.f) {
						mesh->animVertices[weightIndex].blendIndex.w = j;
						mesh->animVertices[weightIndex].blendWeight.w = vertexWeight.mWeight;
					}
				}
			}
		}
		else
		{
			mesh->vertices.resize(aiMesh->mNumVertices);
			memset(mesh->vertices.data(), 0, sizeof(VTXMESH) * aiMesh->mNumVertices);

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

		/* indices */
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

void Converter::ReadAnimation()
{
	if (false == m_IsSkeletal) return;

	m_Channels.resize(m_AiScene->mNumAnimations);
	for (uint32 i = 0; i < m_AiScene->mNumAnimations; ++i)
	{
		const aiAnimation* aiAnim = m_AiScene->mAnimations[i];
		auto anim = make_shared<MODEL_ANIMATION>();

		anim->name = aiAnim->mName.C_Str();
		anim->duration = static_cast<Float>(aiAnim->mDuration);
		anim->tickPerSecond = static_cast<Float>(aiAnim->mTicksPerSecond);
		anim->numChannel = static_cast<uint32>(aiAnim->mNumChannels);

		for (uint32 j = 0; j < aiAnim->mNumChannels; ++j)
		{
			const aiNodeAnim* aiChannel = aiAnim->mChannels[j];
			auto channel = make_shared<MODEL_CHANNEL>();

			channel->name = aiChannel->mNodeName.C_Str();
			channel->boneIndex = Get_BoneIndex(channel->name.c_str());
			uint32 numKeys = std::max(aiChannel->mNumPositionKeys, aiChannel->mNumRotationKeys);
			numKeys = std::max(numKeys, aiChannel->mNumScalingKeys);
			channel->numKeyFrames = numKeys;

			Float3 scale{};
			Float4 rotation{};
			Float3 translation{};
			for (uint32 k = 0; k < numKeys; ++k)
			{
				KEYFRAME keyFrame{}; 
				if (aiChannel->mNumScalingKeys > k)
				{
					memcpy(&scale, &aiChannel->mScalingKeys[k].mValue, sizeof(Float3));
					keyFrame.trackPosition = static_cast<Float>(aiChannel->mScalingKeys[k].mTime);
				}
				if (aiChannel->mNumRotationKeys > k)
				{
					rotation.x = aiChannel->mRotationKeys[k].mValue.x;
					rotation.y = aiChannel->mRotationKeys[k].mValue.y;
					rotation.z = aiChannel->mRotationKeys[k].mValue.z;
					rotation.w = aiChannel->mRotationKeys[k].mValue.w;
					keyFrame.trackPosition = static_cast<Float>(aiChannel->mRotationKeys[k].mTime);
				}
				if (aiChannel->mNumPositionKeys > k)
				{
					memcpy(&translation, &aiChannel->mPositionKeys[k].mValue, sizeof(Float3));
					keyFrame.trackPosition = static_cast<Float>(aiChannel->mPositionKeys[k].mTime);
				}

				keyFrame.scale = scale;
				keyFrame.rotation = rotation;
				keyFrame.position = translation;

				channel->keyFrames.push_back(keyFrame);
			}
			m_Channels[i].push_back(channel);
		}
		m_Animation.push_back(anim);
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
	header.isAnim = m_IsSkeletal;
	header.numBones = static_cast<uint32>(m_Bones.size());
	header.numMeshes = static_cast<uint32>(m_Meshes.size());
	header.numMaterials = static_cast<uint32>(m_Material.size());
	header.numAnimations = static_cast<uint32>(m_Animation.size());
	out.write(BIN(&header), sizeof(header));

	/* Bones */
	for (auto& bone : m_Bones)
	{
		uint32 length = static_cast<uint32>(bone->name.size());
		out.write(BIN(&length), sizeof(uint32));
		out.write(bone->name.data(), length);
		out.write(BIN(&bone->parentIndex), sizeof(int32));
		out.write(BIN(&bone->transform), sizeof(Matrix));
	}

	/* Meshes */
	for (auto& mesh : m_Meshes)
	{
		uint32 nameLength = static_cast<uint32>(mesh->name.size());
		out.write(BIN(&nameLength), sizeof(uint32));
		out.write(mesh->name.data(), nameLength);
		out.write(BIN(&mesh->materialIndex), sizeof(uint32));

		uint32 numBoneIndex = static_cast<uint32>(mesh->boneIndices.size());
		out.write(BIN(&numBoneIndex), sizeof(uint32));
		if (numBoneIndex > 0)
		{
			out.write(BIN(mesh->boneIndices.data()), numBoneIndex * sizeof(uint32));
			uint32 numOffsetIndex = static_cast<uint32>(mesh->offsetMatrices.size());
			out.write(BIN(&numOffsetIndex), sizeof(uint32));
			out.write(BIN(mesh->offsetMatrices.data()), numOffsetIndex * sizeof(Matrix));
		}
		
		uint32 numVertex = (header.isAnim) ? static_cast<uint32>(mesh->animVertices.size()) : static_cast<uint32>(mesh->vertices.size());
		out.write(BIN(&numVertex), sizeof(uint32));
		if (header.isAnim)
			out.write(BIN(mesh->animVertices.data()), numVertex * sizeof(VTXANIMMESH));
		else
			out.write(BIN(mesh->vertices.data()), numVertex * sizeof(VTXMESH));
		
		uint32 numIndex = static_cast<uint32>(mesh->indices.size());
		out.write(BIN(&numIndex), sizeof(uint32));
		out.write(BIN(mesh->indices.data()), numIndex * sizeof(uint32));
	}

	/* Materials */
	uint32 maxTextureType = AI_TEXTURE_TYPE_MAX;
	for (auto& mat : m_Material)
	{
		uint32 nameLength = static_cast<uint32>(mat->name.size());
		out.write(BIN(&nameLength), sizeof(uint32));
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

	/* Animation */
	for (uint32 i = 0; i < m_Animation.size(); ++i)
	{
		auto& anim = m_Animation[i];

		uint32 animNameLength = static_cast<uint32>(anim->name.size());
		out.write(BIN(&animNameLength), sizeof(uint32));
		out.write(anim->name.data(), animNameLength);
		out.write(BIN(&anim->duration), sizeof(Float));
		out.write(BIN(&anim->tickPerSecond), sizeof(Float));
		out.write(BIN(&anim->numChannel), sizeof(uint32));

		for (uint32 j = 0; j < m_Channels[i].size(); ++j)
		{
			auto& channel = m_Channels[i][j];

			uint32 channelNameLength = static_cast<uint32>(channel->name.size());
			out.write(BIN(&channelNameLength), sizeof(uint32));
			out.write(channel->name.data(), channelNameLength);
			out.write(BIN(&channel->boneIndex), sizeof(int32));
			out.write(BIN(&channel->numKeyFrames), sizeof(uint32));
			
			if (channel->numKeyFrames > 0)
			{
				out.write(BIN(channel->keyFrames.data()), channel->numKeyFrames * sizeof(Engine::KEYFRAME));
			}
		}
	}

	out.close();
}

int32 Converter::Get_BoneIndex(const Char* boneName)
{
	int32 boneIndex{ -1 };
	// 인자 타입을 const MODEL_BONE& 에서 auto& 또는 const shared_ptr<MODEL_BONE>& 로 수정
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](const Shared<MODEL_BONE>& bone) -> Bool
		{
			++boneIndex;
			return !strcmp(boneName, bone->name.c_str());
		});
	if (iter == m_Bones.end())
		return -1;
	return boneIndex;
}
