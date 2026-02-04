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

	typedef struct tagGuidDesc
	{
		uint32 typeID = {};
		uint32 uniqueID = {};
	} ID_DESC;

}

#endif // Engine_Struct_h__
