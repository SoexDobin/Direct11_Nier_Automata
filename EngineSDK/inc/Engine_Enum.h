#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN };
	enum class PROTOTYPE { OBJECT, GAMEOBJECT, COMPONENT, LEVEL, ENGINE_MANAGER };
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };
	enum class WORLD_STATE { RIGHT, UP, LOOK, POSITION, END};
	enum class D3DTS { VIEW, PROJ, END };
	enum class DIMB { LBUTTON, RBUTTON, WHEEL, END };
	enum class DIMM { X, Y, WHEEL, END };
	enum class COMPONENT_TYPE
	{
		SCRIPT,
		TRANSFORM,
		TEXTURE,
		VI_RECT_BUFFER,
		VI_TERRAIN_BUFFER,
		SHADER
	};
	

}


namespace Engine
{

	enum class LAYER : unsigned __int32
	{
		LAYER0 = 0 << 0,
		LAYER1 = 1 << 0,
		LAYER2 = 1 << 1,
		LAYER3 = 1 << 2,
		LAYER4 = 1 << 3,
		LAYER5 = 1 << 4,
		LAYER6 = 1 << 5,
		LAYER7 = 1 << 6,

		LAYER8 = 1 << 7,
		LAYER9 = 1 << 8,
		LAYER10 = 1 << 9,
		LAYER11 = 1 << 10,
		LAYER12 = 1 << 11,
		LAYER13 = 1 << 12,
		LAYER14 = 1 << 13,
		LAYER15 = 1 << 14,

		LAYER16 = 1 << 15,
		LAYER17 = 1 << 16,
		LAYER18 = 1 << 17,
		LAYER19 = 1 << 18,
		LAYER20 = 1 << 19,
		LAYER21 = 1 << 20,
		LAYER22 = 1 << 21,
		LAYER23 = 1 << 22,

		LAYER24 = 1 << 23,
		LAYER25 = 1 << 24,
		LAYER26 = 1 << 25,
		LAYER27 = 1 << 26,
		LAYER28 = 1 << 27,
		LAYER29 = 1 << 28,
		LAYER30 = 1 << 29,
		LAYER31 = 1 << 30,

		END = 32,

		L0 = 0,
		L1 = 1,
		L2 = 2,
		L3 = 3,
		L4 = 4,
		L5 = 5,
		L6 = 6,
		L7 = 7,
		L8 = 8,
		L9 = 9,
		L10 = 10,
		L11 = 11,
		L12 = 12,
		L13 = 13,
		L14 = 14,
		L15 = 15,
		L16 = 16,
		L17 = 17,
		L18 = 18,
		L19 = 19,
		L20 = 20,
		L21 = 21,
		L22 = 22,
		L23 = 23,
		L24 = 24,
		L25 = 25,
		L26 = 26,
		L27 = 27,
		L28 = 28,
		L29 = 29,
		L30 = 30,
		L31 = 31,

	
		ALL_LAYER = 0xFFFFFFFF,
	};


	enum class TAG : unsigned __int32
	{
		TAG_0 = 0 << 0,
		TAG_1 = 1 << 0,
		TAG_2 = 1 << 1,
		TAG_3 = 1 << 2,
		TAG_4 = 1 << 3,
		TAG_5 = 1 << 4,
		TAG_6 = 1 << 5,
		TAG_7 = 1 << 6,

		TAG_8 = 1 << 7,
		TAG_9 = 1 << 8,
		TAG_10 = 1 << 9,
		TAG_11 = 1 << 10,
		TAG_12 = 1 << 11,
		TAG_13 = 1 << 12,
		TAG_14 = 1 << 13,
		TAG_15 = 1 << 14,

		TAG_16 = 1 << 15,
		TAG_17 = 1 << 16,
		TAG_18 = 1 << 17,
		TAG_19 = 1 << 18,
		TAG_20 = 1 << 19,
		TAG_21 = 1 << 20,
		TAG_22 = 1 << 21,
		TAG_23 = 1 << 22,

		TAG_24 = 1 << 23,
		TAG_25 = 1 << 24,
		TAG_26 = 1 << 25,
		TAG_27 = 1 << 26,
		TAG_28 = 1 << 27,
		TAG_29 = 1 << 28,
		TAG_30 = 1 << 29,
		TAG_31 = 1 << 30,

		END = 32,
		ALL_TAG = 0xFFFFFFFF,
	};
}


#endif // Engine_Enum_h__

