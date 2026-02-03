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

namespace Engine
{
	struct ENGINE_DLL LayerMask final
	{
		using Mask = uint32;
		LayerMask() = default;

		uint32 Get_Layer() const { return m_layer; }
		void Set_Layer(const uint32 layer) { m_layer = layer; }
		uint32 Get_Mask() const { return m_mask; }
		void Set_Mask(const uint32 mask) { m_mask = mask; }

		void Add(LAYER layer) { m_mask |= ETOI(layer); }
		void Add(uint32 layer) { m_mask |= layer; }
		void Remove(LAYER layer) { m_mask &= ~ETOI(layer); }
		void Remove(uint32 layer) { m_mask &= ~layer; }
		Bool Has(LAYER layer) const { return (m_mask & ETOI(layer)) != 0; }
		Bool Has(uint32 layer) const { return (m_mask & layer) != 0; }

	private:
		Mask m_layer = { 0x00000000 };
		Mask m_mask = { 0xFFFFFFFF };
	};
}

#endif // Engine_Struct_h__
