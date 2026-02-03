#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Define.h"

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

	typedef struct tagLayerMask
	{
		std::wstring layerName = {};
		Engine::LAYER layer;
		Engine::COMPARE_LAYER compareLayer;

	} LAYER_MASK;

	typedef struct tagTagMask
	{
		std::wstring tagName = {};
		Engine::TAG tag;
		Engine::COMPARE_TAG compareTag;
	} TAG_MASK;

	typedef struct tagGuidDesc
	{
		uint32 typeID = {};
		uint32 uniqueID = {};
	} ID_DESC;
}


#endif // Engine_Struct_h__
