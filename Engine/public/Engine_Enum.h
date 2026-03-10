#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine {
enum class WINMODE { FULL, WIN };
enum class PROTOTYPE { OBJECT, GAMEOBJECT, COMPONENT, LEVEL, ENGINE_MANAGER };
enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };
enum class WORLD_STATE { RIGHT, UP, LOOK, POSITION, END };
enum class D3DTS { VIEW, PROJ, END };
enum class DIMB { LBUTTON, RBUTTON, WHEEL, END };
enum class DIMM { X, Y, WHEEL, END };
enum class LIGHT { DIRECTIONAL, POINT, END };
enum class MODEL { NONANIM, ANIM, END };
enum class COMPONENT_TYPE {
	SCRIPT,
	TRANSFORM,
	TEXTURE,
	VI_RECT_BUFFER,
	VI_TERRAIN_BUFFER,
	SHADER,
	MESH,
	MODEL, 
	MATERIAL
};

} // namespace Engine

namespace Engine {

enum class LAYER : unsigned __int32 {
  LAYER0 = 1u << 0,
  LAYER1 = 1u << 1,
  LAYER2 = 1u << 2,
  LAYER3 = 1u << 3,
  LAYER4 = 1u << 4,
  LAYER5 = 1u << 5,
  LAYER6 = 1u << 6,
  LAYER7 = 1u << 7,

  LAYER8 = 1u << 8,
  LAYER9 = 1u << 9,
  LAYER10 = 1u << 10,
  LAYER11 = 1u << 11,
  LAYER12 = 1u << 12,
  LAYER13 = 1u << 13,
  LAYER14 = 1u << 14,
  LAYER15 = 1u << 15,

  LAYER16 = 1u << 16,
  LAYER17 = 1u << 17,
  LAYER18 = 1u << 18,
  LAYER19 = 1u << 19,
  LAYER20 = 1u << 20,
  LAYER21 = 1u << 21,
  LAYER22 = 1u << 22,
  LAYER23 = 1u << 23,

  LAYER24 = 1u << 24,
  LAYER25 = 1u << 25,
  LAYER26 = 1u << 26,
  LAYER27 = 1u << 27,
  LAYER28 = 1u << 28,
  LAYER29 = 1u << 29,
  LAYER30 = 1u << 30,
  LAYER31 = 1u << 31,

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

enum class TAG : unsigned __int32 {
  TAG_0 = 1u << 0,
  TAG_1 = 1u << 1,
  TAG_2 = 1u << 2,
  TAG_3 = 1u << 3,
  TAG_4 = 1u << 4,
  TAG_5 = 1u << 5,
  TAG_6 = 1u << 6,
  TAG_7 = 1u << 7,

  TAG_8 = 1u << 8,
  TAG_9 = 1u << 9,
  TAG_10 = 1u << 10,
  TAG_11 = 1u << 11,
  TAG_12 = 1u << 12,
  TAG_13 = 1u << 13,
  TAG_14 = 1u << 14,
  TAG_15 = 1u << 15,

  TAG_16 = 1u << 16,
  TAG_17 = 1u << 17,
  TAG_18 = 1u << 18,
  TAG_19 = 1u << 19,
  TAG_20 = 1u << 20,
  TAG_21 = 1u << 21,
  TAG_22 = 1u << 22,
  TAG_23 = 1u << 23,

  TAG_24 = 1u << 24,
  TAG_25 = 1u << 25,
  TAG_26 = 1u << 26,
  TAG_27 = 1u << 27,
  TAG_28 = 1u << 28,
  TAG_29 = 1u << 29,
  TAG_30 = 1u << 30,
  TAG_31 = 1u << 31,

  END = 32,
  ALL_TAG = 0xFFFFFFFF,
};
} // namespace Engine

#endif // Engine_Enum_h__
