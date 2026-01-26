#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Define.h"
#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND 			hWnd;
		WINMODE			winMode;
		unsigned int	viewportWidth;
		unsigned int	viewportHeight;
	} ENGINE_DESC;
}


#endif // Engine_Struct_h__
