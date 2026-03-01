#include "pch.h"
#include "Client_Function.h"
#include <rttr/registration>
using rttr::registration;

// ==============================================================
// 샌드박싱 구역: 아래 태그 사이 공간은 파이썬 스크립트가
// 자동으로 Client/public 내부의 헤더를 스캔하여 포함시킵니다.
// ==============================================================
// <AUTO_GENERATED_INCLUDES>
// </AUTO_GENERATED_INCLUDES>

namespace Client {
void Register_Client_Reflection() {
  // ==============================================================
  // [수동 등록 구역 예제]
  // 파이썬 자동화 스크립트는 이 주석 바깥의 코드를 절대 삭제하거나 수정하지
  // 않습니다! 특별한 메타데이터나 수동 설정이 필요한 객체의 경우, 이곳에
  // 작성하세요.
  // ==============================================================
  /*
  rttr::registration::class_<SpecialTerrain>(L"SpecialTerrain")
      .constructor<>()
      .property("HiddenHeight", &SpecialTerrain::m_Height)
      .method("Clone", &SpecialTerrain::Clone)
      .method("Create", &SpecialTerrain::Create);
  */

  // ==============================================================
  // 샌드박싱 구역: 아래 태그 사이 공간은 파이썬 스크립트가
  // 자동으로 GameObject 및 Script 파생 객체의 RTTR 블록을 채웁니다.
  // ==============================================================
  // <AUTO_GENERATED_RTTR>
  // </AUTO_GENERATED_RTTR>
}
} // namespace Client
