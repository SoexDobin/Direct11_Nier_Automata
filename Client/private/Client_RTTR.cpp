#include "pch.h"
#include "Client_Function.h"
#include <rttr/registration>

#include "CityOfRuins.h"
#include "LoadingBackground.h"
#include "LoadingLogo.h"
#include "LoadingPixelPanel.h"


#include "LoadingFadeIn.h"
#include "LoadingFadeOut.h"
#include "ThirdPersonCamera.h"
#include "StateMachine.h"
#include "P10000.h"
#include "P10000Body.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "StaticCamera.h"
#include "TitleBackground.h"
#include "P10000StateMachine.h"
#include "P10000Input.h"

using rttr::registration;

// ==============================================================
// 샌드박싱 구역: 아래 태그 사이 공간은 파이썬 스크립트가
// 자동으로 Client/public 내부의 헤더를 스캔하여 포함시킵니다.
// ==============================================================
// <AUTO_GENERATED_INCLUDES>
#include "FreeCamera.h"
#include "Monster.h"
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



	rttr::registration::class_<P10000>("P10000")
		.constructor<>()
        .method("Clone", &P10000::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return P10000::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<P10000Body>("P10000Body")
        .constructor<>()
        .method("Clone", &P10000Body::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return P10000Body::Create(device, context); })
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


    rttr::registration::class_<ThirdPersonCamera>("ThirdPersonCamera")
        .constructor<>()
        .method("Clone", &ThirdPersonCamera::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return ThirdPersonCamera::Create(device, context); })
        .property("TargetID", &ThirdPersonCamera::Get_TargetID, &ThirdPersonCamera::Set_TargetID)
        (rttr::metadata(Meta_Key_Type::DataType, Data_Type_Key::uint32),
        rttr::metadata(Meta_Key_Type::TypeTag, Data_Tag::ObjectID),
        rttr::metadata(Meta_Key_Type::SaveData, Save_Data_Key::TargetObjectID))
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<P10000StateMachine>("P10000StateMachine")
        .constructor<>()
        .method("Clone", &P10000StateMachine::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> { return P10000StateMachine::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<P10000Input>("P10000Input")
        .constructor<>()
        .method("Clone", &P10000Input::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<Component> { return P10000Input::Create(device, context); })
        (rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));
    rttr::registration::class_<CityOfRuins>("CityOfRuins")
        .constructor<>()
        .method("Clone", &CityOfRuins::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return CityOfRuins::Create(device, context); })
		(rttr::metadata("Level", ETOI(LEVEL::GAMEPLAY)));

  // ==============================================================
  // 샌드박싱 구역: 아래 태그 사이 공간은 파이썬 스크립트가
  // 자동으로 GameObject 및 Script 파생 객체의 RTTR 블록을 채웁니다.
  // ==============================================================
  // <AUTO_GENERATED_RTTR>
    rttr::registration::class_<FreeCamera>("FreeCamera")
        .constructor<>()
        .method("Clone", &FreeCamera::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return FreeCamera::Create(device, context); })(rttr::metadata("Level", 0));

    rttr::registration::class_<Monster>("Monster")
        .constructor<>()
        .method("Clone", &Monster::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Monster::Create(device, context); })(rttr::metadata("Level", 0));

    rttr::registration::class_<SkyBox>("SkyBox")
        .constructor<>()
        .method("Clone", &SkyBox::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return SkyBox::Create(device, context); })(rttr::metadata("Level", 0));

    rttr::registration::class_<SkySphere>("SkySphere")
        .constructor<>()
        .method("Clone", &SkySphere::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return SkySphere::Create(device, context); })(rttr::metadata("Level", 0));

    rttr::registration::class_<Terrain>("Terrain")
        .constructor<>()
        .method("Clone", &Terrain::Clone)
        .method("Create", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return Terrain::Create(device, context); })(rttr::metadata("Level", 0));


// </AUTO_GENERATED_RTTR>
}
NS_END
