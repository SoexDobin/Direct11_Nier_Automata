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
	} VTXTEX;

}

#endif // Engine_Struct_h__
