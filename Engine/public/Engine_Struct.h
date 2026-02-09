#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND 			hWnd = {};
		WINMODE			winMode = {};
		unsigned int	viewportWidth = {};
		unsigned int	viewportHeight = {};
		unsigned int	levCount = {};
	} ENGINE_DESC;

	typedef struct tagVertexTexcoord
	{
		Vector3			position;
		Vector2			texCoord;
	} VTXTEX;

}

#endif // Engine_Struct_h__
