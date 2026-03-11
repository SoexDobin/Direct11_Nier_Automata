#pragma once

using namespace DirectX;

NS_BEGIN(Tool)
typedef struct asTexEntry
{
	uint32_t typeIndex;
	string path;
} AS_TEX_ENTRY;

typedef struct asBone
{
	string name;
	int32_t index{ -1 };
	int32_t parent{ -1 };
	Matrix transform{};
} AS_BONE;

typedef struct asMesh
{
	string name;
	int32_t boneIndex{ -1 };
	int32_t materialIndex{ 0 };
	vector<VTXMESH> vertices;
	vector<uint32> indices;
} AS_MESH;

typedef struct asMaterial
{
	string name;
	Vector4 diffuse{ 0.f, 0.f, 0.f, 1.f };
	Vector4 specular{ 1.f, 1.f, 1.f, 1.f };
	Vector4 ambient{ 0.f, 0.f, 0.f, 1.f };
	Vector4 emissive{ 0.f, 0.f, 0.f, 0.f };
	string diffuseFile;
	string specularFile;
	string normalFile;
	vector<AS_TEX_ENTRY> textures;
} AS_MATERIAL;

typedef struct asAnimation
{
	string name;
} AS_ANIM;

using namespace Tool;

NS_END

