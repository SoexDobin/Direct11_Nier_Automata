#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND 			hWnd = {};
		WINMODE			winMode = {};
		std::wstring	windowTitle = {};
		unsigned int	viewportWidth = {};
		unsigned int	viewportHeight = {};
		unsigned int	levCount = {};
		bool            useOffscreenRendering {};
		uint32          renderTargetCount = {};
	} ENGINE_DESC;

	typedef struct tagVertexTexcoord
	{
		Vector3			position;
		Vector2			texCoord;
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
		Vector2			texCoord;
		static const uint32_t numElements = { 3 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elemnets[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	} VTXNORMTEX;

}

#endif // Engine_Struct_h__
