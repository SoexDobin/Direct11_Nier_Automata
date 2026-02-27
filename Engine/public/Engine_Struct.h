#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND 			hWnd = {};
		HINSTANCE 		hInst = {};
		WINMODE			winMode = {};
		std::wstring	windowTitle = {};
		uint32_t		viewportWidth = {};
		uint32_t		viewportHeight = {};
		uint32_t		startLevel = {};
		uint32_t		levelCount = {};
		bool            useOffscreenRendering {};
		uint32_t         renderTargetCount = {};
	} ENGINE_DESC;

	typedef struct tagGuidDesc {
		uint32_t m_typeID = {};
		uint32_t m_objectID = {};
	} ID_DESC, ObjectID;

	typedef struct tagLightDesc
	{
		LIGHT type = {};
		Vector4 direction = {};
		Vector4 position = {};
		float range = {};

		Vector4 diffuse = {};
		Vector4 Ambient = {};
		Vector4 specular = {};
	} LIGHT_DESC;

	typedef struct tagVertexTexcoord
	{
		Vector3			position;
		Vector2			texcoord;
		static const uint32_t numElements = { 2 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXTEX;

	typedef struct tagVertexNormalTexcoord
	{
		Vector3			position;
		Vector3			normal;
		Vector2			texcoord;

		static const uint32_t numElements = { 3 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elemnets[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXNORMTEX;

	typedef struct tagVertexMesh
	{
		Vector3			position;
		Vector3			normal;
		Vector3			tangent;
		Vector2			texcoord;

		static const uint32_t numElements = { 4 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elemnets[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXMESH;

}

#endif // Engine_Struct_h__
