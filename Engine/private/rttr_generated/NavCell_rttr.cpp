#include "NavCell.h"
// NavCell::Create가 Shared<Navigation>을 받아서, RTTR 등록에는 완전한 타입이 필요하다.
// 이 include가 없으면 유니티 빌드에서 같은 묶음에 Navigation.h가 들어올 때만 우연히 컴파일된다.
#include "Navigation.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

RTTR_REGISTRATION_NAMED(NavCell_RTTR)
{
	registration::class_<NavCell>("NavCell")
		.constructor<>()
		.method("Create", &NavCell::Create)
		;
}
