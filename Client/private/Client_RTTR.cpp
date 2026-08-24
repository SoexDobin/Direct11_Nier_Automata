#include "pch.h"
#include "Client_Function.h"
#include <rttr/registration>

#include "CityOfRuinEntry.h"
#include "LoadingBackground.h"
#include "LoadingLogo.h"
#include "LoadingPixelPanel.h"


#include "LoadingFadeIn.h"
#include "LoadingFadeOut.h"
#include "ThirdPersonCamera.h"
#include "StateMachine.h"
#include "Pl0000.h"
#include "Pl0000Body.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "StaticCamera.h"
#include "TitleBackground.h"
#include "Pl0000StateMachine.h"
#include "Pl0000Input.h"
#include "Pl0000Movement.h"
#include "Pl0000Shockwave.h"
#include "WP3000Body.h"
#include "Em3100.h"
#include "Em3100Body.h"
#include "SparkEffect.h"
#include "FireFlashEffect.h"
#include "Bullet.h"
#include "CityOfRuinBridge.h"
#include "HpBarWorldUI.h"
#include "Em0010.h"
#include "Em0010Body.h"
#include "Em0010Movement.h"
#include "MonsterSight.h"
#include "MonsterAOE.h"
#include "Pl0000EvadeChecker.h"
#include "Pl0000MonsterChecker.h"
#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000Movement.h"
#include "Em3001.h"
#include "Em3002.h"
#include "Em3003.h"
#include "SheathWP0070Body.h"
#include "SheathWP0220Body.h"

using rttr::registration;

// ==============================================================
// 샌드박싱 구역: 아래 태그 사이 공간은 파이썬 스크립트가
// 자동으로 Client/public 내부의 헤더를 스캔하여 포함시킵니다.
// ==============================================================
// <AUTO_GENERATED_INCLUDES>
#include "FreeCamera.h"
#include "MonsterStateMachine.h"
#include "SkyBox.h"
#include "SkySphere.h"
#include "Terrain.h"
// </AUTO_GENERATED_INCLUDES>

namespace Client {
void Register_Client_Reflection() {
  static bool bRegistered = false;
  if (bRegistered) return;
  bRegistered = true;
  // ==============================================================
  // 샌드박싱 구역: 아래 태그 사이 공간은 파이썬 스크립트가
  // 자동으로 GameObject 및 Script 파생 객체의 RTTR 블록을 채웁니다.
  // ==============================================================
	rttr::registration::class_<LoadingFadeIn>("LoadingFadeIn")
      .constructor<>()
      .method("Clone", &LoadingFadeIn::Clone)
      .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return LoadingFadeIn::Create(device, context); })
      (rttr::metadata("Level", ETOI(LEVEL::STATIC)));
	rttr::registration::class_<LoadingFadeOut>("LoadingFadeOut")
      .constructor<>()
      .method("Clone", &LoadingFadeOut::Clone)
      .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return LoadingFadeOut::Create(device, context); })
      (rttr::metadata("Level", ETOI(LEVEL::STATIC)));

	rttr::registration::class_<LoadingBackground>("LoadingBackground")
      .constructor<>()
      .method("Clone", &LoadingBackground::Clone)
      .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return LoadingBackground::Create(device, context); })
      (rttr::metadata("Level", ETOI(LEVEL::LOADING)));
	rttr::registration::class_<LoadingLogo>("LoadingLogo")
      .constructor<>()
      .method("Clone", &LoadingLogo::Clone)
      .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return LoadingLogo::Create(device, context); })
      (rttr::metadata("Level", ETOI(LEVEL::LOADING)));
	rttr::registration::class_<LoadingPixelPanel>("LoadingPixelPanel")
      .constructor<>()
      .method("Clone", &LoadingPixelPanel::Clone)
      .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return LoadingPixelPanel::Create(device, context); })
      (rttr::metadata("Level", ETOI(LEVEL::LOADING)));
	rttr::registration::class_<TitleBackground>("TitleBackground")
      .constructor<>()
      .method("Clone", &TitleBackground::Clone)
      .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return TitleBackground::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::TITLE)));
	rttr::registration::class_<StaticCamera>("StaticCamera")
      .constructor<>()
      .method("Clone", &StaticCamera::Clone)
      .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return StaticCamera::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::STATIC)));



	rttr::registration::class_<Pl0000>("Pl0000")
		.constructor<>()
        .method("Clone", &Pl0000::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Pl0000::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Pl0000Body>("Pl0000Body")
        .constructor<>()
        .method("Clone", &Pl0000Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Pl0000Body::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<WP0070Body>("WP0070Body")
        .constructor<>()
        .method("Clone", &WP0070Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return WP0070Body::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<WP0220Body>("WP0220Body")
        .constructor<>()
        .method("Clone", &WP0220Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return WP0220Body::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<WP3000Body>("WP3000Body")
        .constructor<>()
        .method("Clone", &WP3000Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return WP3000Body::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Pl0000EvadeChecker>("Pl0000EvadeChecker")
        .constructor<>()
        .method("Clone", &Pl0000EvadeChecker::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Pl0000EvadeChecker::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));


    rttr::registration::class_<ThirdPersonCamera>("ThirdPersonCamera")
        .constructor<>()
        .method("Clone", &ThirdPersonCamera::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return ThirdPersonCamera::Create(device, context); })
        .property("TargetID", &ThirdPersonCamera::Get_TargetID, &ThirdPersonCamera::Set_TargetID)
        (rttr::metadata(Meta_Key_Type::DataType, Data_Type_Key::uint32),
        rttr::metadata(Meta_Key_Type::TypeTag, Data_Tag::ObjectID),
        rttr::metadata(Meta_Key_Type::SaveData, Save_Data_Key::TargetObjectID))
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Pl0000StateMachine>("Pl0000StateMachine")
        .constructor<>()
        .method("Clone", &Pl0000StateMachine::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> { return Pl0000StateMachine::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Pl0000Input>("Pl0000Input")
        .constructor<>()
        .method("Clone", &Pl0000Input::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> { return Pl0000Input::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Pl0000Movement>("Pl0000Movement")
        .constructor<>()
        .method("Clone", &Pl0000Movement::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> { return Pl0000Movement::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Pl0000Shockwave>("Pl0000Shockwave")
        .constructor<>()
        .method("Clone", &Pl0000Shockwave::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Pl0000Shockwave::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Pl0000MonsterChecker>("Pl0000MonsterChecker")
        .constructor<>()
        .method("Clone", &Pl0000MonsterChecker::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Pl0000MonsterChecker::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<SheathWP0070Body>("SheathWP0070Body")
        .constructor<>()
        .method("Clone", &SheathWP0070Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return SheathWP0070Body::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<SheathWP0220Body>("SheathWP0220Body")
        .constructor<>()
        .method("Clone", &SheathWP0220Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return SheathWP0220Body::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));



    rttr::registration::class_<CityOfRuinEntry>("CityOfRuinEntry")
        .constructor<>()
        .method("Clone", &CityOfRuinEntry::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return CityOfRuinEntry::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<CityOfRuinBridge>("CityOfRuinBridge")
        .constructor<>()
        .method("Clone", &CityOfRuinBridge::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return CityOfRuinBridge::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    // MONSTER
    rttr::registration::class_<Em3100>("Em3100")
        .constructor<>()
        .method("Clone", &Em3100::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em3100::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<Em3100Body>("Em3100Body")
        .constructor<>()
        .method("Clone", &Em3100Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em3100Body::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em0010>("Em0010")
        .constructor<>()
        .method("Clone", &Em0010::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em0010::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em0010Body>("Em0010Body")
        .constructor<>()
        .method("Clone", &Em0010Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em0010Body::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<MonsterSight>("MonsterSight")
        .constructor<>()
        .method("Clone", &MonsterSight::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return MonsterSight::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<MonsterAOE>("MonsterAOE")
        .constructor<>()
        .method("Clone", &MonsterAOE::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return MonsterAOE::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em0010Movement>("Em0010Movement")
        .constructor<>()
        .method("Clone", &Em0010Movement::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> { return Em0010Movement::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em3000>("Em3000")
        .constructor<>()
        .method("Clone", &Em3000::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em3000::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em3000Body>("Em3000Body")
        .constructor<>()
        .method("Clone", &Em3000Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em3000Body::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em3000Movement>("Em3000Movement")
        .constructor<>()
        .method("Clone", &Em3000Movement::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> { return Em3000Movement::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em3001>("Em3001")
        .constructor<>()
        .method("Clone", &Em3001::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em3001::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em3002>("Em3002")
        .constructor<>()
        .method("Clone", &Em3002::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em3002::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    rttr::registration::class_<Em3003>("Em3003")
        .constructor<>()
        .method("Clone", &Em3003::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Em3003::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    // Projectile
    rttr::registration::class_<Bullet>("Bullet")
        .constructor<>()
        .method("Clone", &Bullet::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Bullet::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    // Effect
    rttr::registration::class_<FireFlashEffect>("FireFlashEffect")
        .constructor<>()
        .method("Clone", &FireFlashEffect::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return FireFlashEffect::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<SparkEffect>("SparkEffect")
        .constructor<>()
        .method("Clone", &SparkEffect::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return SparkEffect::Create(device, context); })
	(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

    // IN GAME UI
    rttr::registration::class_<HpBarWorldUI>("HpBarWorldUI")
        .constructor<>()
        .method("Clone", &HpBarWorldUI::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return HpBarWorldUI::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

  // ==============================================================
  // 샌드박싱 구역: 아래 태그 사이 공간은 파이썬 스크립트가
  // 자동으로 GameObject 및 Script 파생 객체의 RTTR 블록을 채웁니다.
  // ==============================================================
  // <AUTO_GENERATED_RTTR>
    rttr::registration::class_<FreeCamera>("FreeCamera")
        .constructor<>()
        .method("Clone", &FreeCamera::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> {
            return FreeCamera::Create(device, context);
        })(rttr::metadata("Level", 0));

    rttr::registration::class_<MonsterStateMachine>("MonsterStateMachine")
        .constructor<>()
        .method("Clone", &MonsterStateMachine::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> {
            return MonsterStateMachine::Create(device, context);
        })(rttr::metadata("Level", 0));

    rttr::registration::class_<SkyBox>("SkyBox")
        .constructor<>()
        .method("Clone", &SkyBox::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> {
            return SkyBox::Create(device, context);
        })(rttr::metadata("Level", 0));

    rttr::registration::class_<SkySphere>("SkySphere")
        .constructor<>()
        .method("Clone", &SkySphere::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> {
            return SkySphere::Create(device, context);
        })(rttr::metadata("Level", 0));

    rttr::registration::class_<Terrain>("Terrain")
        .constructor<>()
        .method("Clone", &Terrain::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> {
            return Terrain::Create(device, context);
        })(rttr::metadata("Level", 0));

// </AUTO_GENERATED_RTTR>
}
NS_END
