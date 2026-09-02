#include "pch.h"
#include "Client_Function.h"

#include "Game.h"
#include "PartObject.h"
#include "UIObject.h"

#include "Bullet.h"
#include "CityOfRuinBridge.h"
#include "CityOfRuinEntry.h"
#include "Em0010.h"
#include "Em0010Body.h"
#include "Em0010Movement.h"
#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000Movement.h"
#include "Em3001.h"
#include "Em3002.h"
#include "Em3003.h"
#include "Em3100.h"
#include "Em3100Body.h"
#include "FireFlashEffect.h"
#include "FreeCamera.h"
#include "HpBarWorldUI.h"
#include "LoadingBackground.h"
#include "LoadingFadeIn.h"
#include "LoadingFadeOut.h"
#include "LoadingLogo.h"
#include "LoadingPixelPanel.h"
#include "MonsterAOE.h"
#include "MonsterSight.h"
#include "MonsterStateMachine.h"
#include "Pl0000.h"
#include "Pl0000Body.h"
#include "Pl0000EvadeChecker.h"
#include "Pl0000Input.h"
#include "Pl0000MonsterChecker.h"
#include "Pl0000Movement.h"
#include "Pl0000Shockwave.h"
#include "Pl0000StateMachine.h"
#include "SheathWP0070Body.h"
#include "SheathWP0220Body.h"
#include "SkyBox.h"
#include "SkySphere.h"
#include "SparkEffect.h"
#include "StaticCamera.h"
#include "Terrain.h"
#include "ThirdPersonCamera.h"
#include "TitleBackground.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "WP3000Body.h"

namespace
{
	template <typename T>
	ReflectedTypeDescriptor& Add_Type(ReflectionDescriptorBatch& batch, const char* registeredName,
		const char* baseRegisteredName, REFLECTED_OBJECT_KIND objectKind, uint32 level,
		HIERARCHY_AUTHORING_MODE authoringMode = HIERARCHY_AUTHORING_MODE::CODE_DEFINED)
	{
		ReflectedTypeDescriptor descriptor;
		descriptor.info.registeredName = registeredName;
		descriptor.info.runtimeTypeId = Make_ExternalRuntimeTypeId<T>();
		descriptor.info.baseRegisteredNames.emplace_back(baseRegisteredName);
		descriptor.info.objectKind = objectKind;
		if constexpr (std::is_base_of_v<PartObject, T>)
			descriptor.info.authoringMode = HIERARCHY_AUTHORING_MODE::LEAF;
		else
			descriptor.info.authoringMode = authoringMode;
		descriptor.info.level = level;
		descriptor.info.hasLevel = true;
		descriptor.createPrototype = []() -> Shared<Object> {
			return static_pointer_cast<Object>(T::Create(
				GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context()));
		};
		batch.types.push_back(std::move(descriptor));
		return batch.types.back();
	}

	ReflectedPropertyDescriptor Make_HpBarTarget_Property()
	{
		ReflectedPropertyDescriptor property;
		property.info.registeredName = "Target";
		property.info.valueType = REFLECTION_VALUE_TYPE::OBJECT_REF;
		property.info.dataTag = "ObjectRef";
		property.info.assetType = Asset_Type_Key::GameObject;
		property.info.saveDataKey = Save_Data_Key::ObjectReference;
		property.info.expectedBaseRegisteredName = "Entity";
		property.info.isWritable = true;
		property.info.isSerializable = true;
		property.read = [](Object& target, ReflectionValue& outValue) -> HRESULT {
			auto* hpBar = dynamic_cast<HpBarWorldUI*>(&target);
			if (!hpBar)
				return E_NOINTERFACE;
			outValue.data = hpBar->Get_TargetObjectGuid();
			return S_OK;
		};
		property.write = [](Object& target, const ReflectionValue& value) -> HRESULT {
			auto* hpBar = dynamic_cast<HpBarWorldUI*>(&target);
			const ObjectGuid* targetGuid = value.Try_Get<ObjectGuid>();
			return hpBar && targetGuid
				? hpBar->Set_TargetObjectGuid(*targetGuid)
				: E_INVALIDARG;
		};
		return property;
	}

	ReflectedPropertyDescriptor Make_HpBarWorldOffset_Property()
	{
		ReflectedPropertyDescriptor property;
		property.info.registeredName = "WorldOffset";
		property.info.valueType = REFLECTION_VALUE_TYPE::VECTOR3;
		property.info.dataTag = Data_Tag::Position;
		property.info.assetType = Asset_Type_Key::NoneAsset;
		property.info.saveDataKey = "WorldOffset";
		property.info.isWritable = true;
		property.info.isSerializable = true;
		property.read = [](Object& target, ReflectionValue& outValue) -> HRESULT {
			auto* hpBar = dynamic_cast<HpBarWorldUI*>(&target);
			if (!hpBar)
				return E_NOINTERFACE;
			outValue.data = hpBar->Get_WorldOffset();
			return S_OK;
		};
		property.write = [](Object& target, const ReflectionValue& value) -> HRESULT {
			auto* hpBar = dynamic_cast<HpBarWorldUI*>(&target);
			const Vector3* worldOffset = value.Try_Get<Vector3>();
			if (!hpBar || !worldOffset)
				return E_INVALIDARG;
			hpBar->Set_WorldOffset(*worldOffset);
			return S_OK;
		};
		return property;
	}

	template <typename Enum>
	ReflectedEnumValue Enum_Value(const char* name, Enum value)
	{
		return { name, static_cast<std::int64_t>(value) };
	}

#define ENUM_VALUE(type, name) Enum_Value(#name, type)

	void Add_Animation_Enums(ReflectionDescriptorBatch& batch)
	{
		batch.enums.push_back({ "Pl0000.AnimationState", "AnimationState", {
			ENUM_VALUE(Pl0000::PL0000_STATE::IDLE_Neutral, IDLE_Neutral),
			ENUM_VALUE(Pl0000::PL0000_STATE::IDLE_STAND_TO_Neutral, IDLE_STAND_TO_Neutral),
			ENUM_VALUE(Pl0000::PL0000_STATE::WALK_TO_RUN, WALK_TO_RUN),
			ENUM_VALUE(Pl0000::PL0000_STATE::RUN_CYCLE, RUN_CYCLE),
			ENUM_VALUE(Pl0000::PL0000_STATE::RUN_STOP_L, RUN_STOP_L),
			ENUM_VALUE(Pl0000::PL0000_STATE::RUN_STOP_R, RUN_STOP_R),
			ENUM_VALUE(Pl0000::PL0000_STATE::RUN_LIGHT_CYCLE, RUN_LIGHT_CYCLE),
			ENUM_VALUE(Pl0000::PL0000_STATE::RUN_HEAVY_CYCLE, RUN_HEAVY_CYCLE),
			ENUM_VALUE(Pl0000::PL0000_STATE::SPRINT_CYCLE, SPRINT_CYCLE),
			ENUM_VALUE(Pl0000::PL0000_STATE::SPRINT_STOP_R, SPRINT_STOP_R),
			ENUM_VALUE(Pl0000::PL0000_STATE::STAND_TO_DASH_F, STAND_TO_DASH_F),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_F, DASH_F),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_TO_STAND_F, DASH_TO_STAND_F),
			ENUM_VALUE(Pl0000::PL0000_STATE::STAND_TO_DASH_B, STAND_TO_DASH_B),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_B, DASH_B),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_TO_STAND_B, DASH_TO_STAND_B),
			ENUM_VALUE(Pl0000::PL0000_STATE::STAND_TO_DASH_R, STAND_TO_DASH_R),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_R, DASH_R),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_TO_STAND_R, DASH_TO_STAND_R),
			ENUM_VALUE(Pl0000::PL0000_STATE::STAND_TO_DASH_L, STAND_TO_DASH_L),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_L, DASH_L),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_TO_STAND_L, DASH_TO_STAND_L),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_F_TO_SPRINT, DASH_F_TO_SPRINT),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_B_TO_SPRINT, DASH_B_TO_SPRINT),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_R_TO_SPRINT, DASH_R_TO_SPRINT),
			ENUM_VALUE(Pl0000::PL0000_STATE::DASH_L_TO_SPRINT, DASH_L_TO_SPRINT),
			ENUM_VALUE(Pl0000::PL0000_STATE::EVADE_FRONT, EVADE_FRONT),
			ENUM_VALUE(Pl0000::PL0000_STATE::EVADE_BACKWARD, EVADE_BACKWARD),
			ENUM_VALUE(Pl0000::PL0000_STATE::EVADE_RIGHT, EVADE_RIGHT),
			ENUM_VALUE(Pl0000::PL0000_STATE::EVADE_LEFT, EVADE_LEFT),
			ENUM_VALUE(Pl0000::PL0000_STATE::JUMP_ENTER, JUMP_ENTER),
			ENUM_VALUE(Pl0000::PL0000_STATE::JUMP_HOLD, JUMP_HOLD),
			ENUM_VALUE(Pl0000::PL0000_STATE::DOUBLE_JUMP1, DOUBLE_JUMP1),
			ENUM_VALUE(Pl0000::PL0000_STATE::DOUBLE_JUMP2, DOUBLE_JUMP2),
			ENUM_VALUE(Pl0000::PL0000_STATE::DOUBLE_JUMP3, DOUBLE_JUMP3),
			ENUM_VALUE(Pl0000::PL0000_STATE::DOUBLE_JUMP4, DOUBLE_JUMP4),
			ENUM_VALUE(Pl0000::PL0000_STATE::STAND_TO_JUMP, STAND_TO_JUMP),
			ENUM_VALUE(Pl0000::PL0000_STATE::RUN_TO_JUMP, RUN_TO_JUMP),
			ENUM_VALUE(Pl0000::PL0000_STATE::SPRINT_TO_JUMP, SPRINT_TO_JUMP),
			ENUM_VALUE(Pl0000::PL0000_STATE::JUMP_TO_RUN, JUMP_TO_RUN),
			ENUM_VALUE(Pl0000::PL0000_STATE::JUMP_TO_SPRINT, JUMP_TO_SPRINT),
			ENUM_VALUE(Pl0000::PL0000_STATE::JUMP_TO_STAND, JUMP_TO_STAND),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND1, LIGHT_GROUND1),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND2, LIGHT_GROUND2),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND3, LIGHT_GROUND3),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND4, LIGHT_GROUND4),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND5, LIGHT_GROUND5),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND6, LIGHT_GROUND6),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND7, LIGHT_GROUND7),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND_HOLD, LIGHT_GROUND_HOLD),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_GROUND_RUN, LIGHT_GROUND_RUN),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR1, LIGHT_AIR1),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR2, LIGHT_AIR2),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR3, LIGHT_AIR3),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR4, LIGHT_AIR4),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR5, LIGHT_AIR5),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_ENTER, LIGHT_AIR_DOWN_ENTER),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_HOLD, LIGHT_AIR_DOWN_HOLD),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_END, LIGHT_AIR_DOWN_END),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND1, HEAVY_GROUND1),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND2, HEAVY_GROUND2),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND3, HEAVY_GROUND3),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_UNFULL, HEAVY_GROUND_HOLD_UNFULL),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_NO_CONTACT, HEAVY_GROUND_HOLD_NO_CONTACT),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_CYCLE, HEAVY_GROUND_HOLD_CYCLE),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_FULL, HEAVY_GROUND_HOLD_FULL),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_GROUND_SPRINT, HEAVY_GROUND_SPRINT),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_ENTER, HEAVY_AIR_DOWN_ENTER),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_HOLD, HEAVY_AIR_DOWN_HOLD),
			ENUM_VALUE(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_END, HEAVY_AIR_DOWN_END),
			ENUM_VALUE(Pl0000::PL0000_STATE::LIGHT_HEAVY_COMBO, LIGHT_HEAVY_COMBO),
		} });

		batch.enums.push_back({ "WP0070.AnimationState", "AnimationState", {
			ENUM_VALUE(WP0070Body::WP0070_STATE::SHEATHE_LIGHT, SHEATHE_LIGHT),
			ENUM_VALUE(WP0070Body::WP0070_STATE::SHEATHE_HEAVY, SHEATHE_HEAVY),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND1, LIGHT_GROUND1),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND2, LIGHT_GROUND2),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND3, LIGHT_GROUND3),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND4, LIGHT_GROUND4),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND5, LIGHT_GROUND5),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND6, LIGHT_GROUND6),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND7, LIGHT_GROUND7),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND_RUN, LIGHT_GROUND_RUN),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_GROUND_HOLD, LIGHT_GROUND_HOLD),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_AIR1, LIGHT_AIR1),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_AIR2, LIGHT_AIR2),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_AIR3, LIGHT_AIR3),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_AIR_DOWN_ENTER, LIGHT_AIR_DOWN_ENTER),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_AIR_DOWN_HOLD, LIGHT_AIR_DOWN_HOLD),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_AIR_DOWN_END, LIGHT_AIR_DOWN_END),
			ENUM_VALUE(WP0070Body::WP0070_STATE::LIGHT_COMBO, LIGHT_COMBO),
		} });

		batch.enums.push_back({ "WP0220.AnimationState", "AnimationState", {
			ENUM_VALUE(WP0220Body::WP0220_STATE::SHEATHE_LIGHT, SHEATHE_LIGHT),
			ENUM_VALUE(WP0220Body::WP0220_STATE::SHEATHE_HEAVY, SHEATHE_HEAVY),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND1, HEAVY_GROUND1),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND2, HEAVY_GROUND2),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND3, HEAVY_GROUND3),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND_HOLD_UNFULL, HEAVY_GROUND_HOLD_UNFULL),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND_HOLD_CYCLE, HEAVY_GROUND_HOLD_CYCLE),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND_HOLD_NO_CONTACT, HEAVY_GROUND_HOLD_NO_CONTACT),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND_HOLD_FULL, HEAVY_GROUND_HOLD_FULL),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_GROUND_SPRINT, HEAVY_GROUND_SPRINT),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_AIR_DOWN_ENTER, HEAVY_AIR_DOWN_ENTER),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_AIR_DOWN_HOLD, HEAVY_AIR_DOWN_HOLD),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_AIR_DOWN_END, HEAVY_AIR_DOWN_END),
			ENUM_VALUE(WP0220Body::WP0220_STATE::HEAVY_COMBO, HEAVY_COMBO),
		} });

		batch.enums.push_back({ "WP3000.AnimationState", "AnimationState", {
			ENUM_VALUE(WP3000Body::POD_STATE::IDLE, IDLE),
			ENUM_VALUE(WP3000Body::POD_STATE::SHOOT_START, SHOOT_START),
			ENUM_VALUE(WP3000Body::POD_STATE::SHOOT_LOOP, SHOOT_LOOP),
			ENUM_VALUE(WP3000Body::POD_STATE::SHOOT_END, SHOOT_END),
		} });
	}

#undef ENUM_VALUE
}

HRESULT Client::Register_Client_Reflection()
{
	static Bool registered = false;
	if (registered)
		return S_OK;

	ReflectionDescriptorBatch batch;
	Add_Animation_Enums(batch);

	constexpr auto gameObject = REFLECTED_OBJECT_KIND::GAMEOBJECT;
	constexpr auto component = REFLECTED_OBJECT_KIND::COMPONENT;
	const uint32 staticLevel = ETOI(LEVEL::STATIC);
	const uint32 loadingLevel = ETOI(LEVEL::LOADING);
	const uint32 titleLevel = ETOI(LEVEL::TITLE);
	const uint32 gameplayLevel = ETOI(LEVEL::GAMEPLAY);

	Add_Type<LoadingFadeIn>(batch, "LoadingFadeIn", "UIObject", gameObject, staticLevel,
		HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED);
	Add_Type<LoadingFadeOut>(batch, "LoadingFadeOut", "UIObject", gameObject, staticLevel,
		HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED);
	Add_Type<LoadingBackground>(batch, "LoadingBackground", "UIObject", gameObject, loadingLevel,
		HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED);
	Add_Type<LoadingLogo>(batch, "LoadingLogo", "UIObject", gameObject, loadingLevel,
		HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED);
	Add_Type<LoadingPixelPanel>(batch, "LoadingPixelPanel", "UIObject", gameObject, loadingLevel,
		HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED);
	Add_Type<TitleBackground>(batch, "TitleBackground", "UIObject", gameObject, titleLevel,
		HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED);
	Add_Type<StaticCamera>(batch, "StaticCamera", "Camera", gameObject, staticLevel);
	Add_Type<FreeCamera>(batch, "FreeCamera", "Camera", gameObject, staticLevel);
	Add_Type<SkyBox>(batch, "SkyBox", "GameObject", gameObject, staticLevel);
	Add_Type<SkySphere>(batch, "SkySphere", "GameObject", gameObject, staticLevel);
	Add_Type<Terrain>(batch, "Terrain", "GameObject", gameObject, staticLevel);

	Add_Type<Pl0000>(batch, "Pl0000", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::CODE_DEFINED);
	Add_Type<Pl0000Body>(batch, "Pl0000Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<WP0070Body>(batch, "WP0070Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<WP0220Body>(batch, "WP0220Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<WP3000Body>(batch, "WP3000Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<Pl0000EvadeChecker>(batch, "Pl0000EvadeChecker", "GameObject", gameObject, gameplayLevel);
	Add_Type<ThirdPersonCamera>(batch, "ThirdPersonCamera", "Camera", gameObject, gameplayLevel);
	Add_Type<Pl0000StateMachine>(batch, "Pl0000StateMachine", "Component", component, gameplayLevel);
	Add_Type<Pl0000Input>(batch, "Pl0000Input", "Component", component, gameplayLevel);
	Add_Type<Pl0000Movement>(batch, "Pl0000Movement", "Component", component, gameplayLevel);
	Add_Type<Pl0000Shockwave>(batch, "Pl0000Shockwave", "GameObject", gameObject, gameplayLevel);
	Add_Type<Pl0000MonsterChecker>(batch, "Pl0000MonsterChecker", "GameObject", gameObject, gameplayLevel);
	Add_Type<SheathWP0070Body>(batch, "SheathWP0070Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<SheathWP0220Body>(batch, "SheathWP0220Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<CityOfRuinEntry>(batch, "CityOfRuinEntry", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::CODE_DEFINED);
	Add_Type<CityOfRuinBridge>(batch, "CityOfRuinBridge", "GameObject", gameObject, gameplayLevel);
	Add_Type<Em3100>(batch, "Em3100", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::CODE_DEFINED);
	Add_Type<Em3100Body>(batch, "Em3100Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<Em0010>(batch, "Em0010", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::CODE_DEFINED);
	Add_Type<Em0010Body>(batch, "Em0010Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<MonsterSight>(batch, "MonsterSight", "GameObject", gameObject, gameplayLevel);
	Add_Type<MonsterAOE>(batch, "MonsterAOE", "GameObject", gameObject, gameplayLevel);
	Add_Type<Em0010Movement>(batch, "Em0010Movement", "Component", component, gameplayLevel);
	Add_Type<Em3000>(batch, "Em3000", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::CODE_DEFINED);
	Add_Type<Em3000Body>(batch, "Em3000Body", "GameObject", gameObject, gameplayLevel);
	Add_Type<Em3000Movement>(batch, "Em3000Movement", "Component", component, gameplayLevel);
	Add_Type<Em3001>(batch, "Em3001", "GameObject", gameObject, gameplayLevel);
	Add_Type<Em3002>(batch, "Em3002", "GameObject", gameObject, gameplayLevel);
	Add_Type<Em3003>(batch, "Em3003", "GameObject", gameObject, gameplayLevel);
	Add_Type<Bullet>(batch, "Bullet", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::TRANSIENT);
	Add_Type<FireFlashEffect>(batch, "FireFlashEffect", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::TRANSIENT);
	Add_Type<SparkEffect>(batch, "SparkEffect", "GameObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::TRANSIENT);
	ReflectedTypeDescriptor& hpBar = Add_Type<HpBarWorldUI>(batch, "HpBarWorldUI",
		"WorldUIObject", gameObject, gameplayLevel,
		HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED);
	hpBar.properties.push_back(Make_HpBarTarget_Property());
	hpBar.properties.push_back(Make_HpBarWorldOffset_Property());
	Add_Type<MonsterStateMachine>(batch, "MonsterStateMachine", "Component", component, staticLevel);

	const HRESULT result = GAME_INSTANCE->Register_ReflectionDescriptors(batch);
	if (SUCCEEDED(result))
		registered = true;
	return result;
}
