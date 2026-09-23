#include "InstanceMesh.h"
#include <rttr/registration>

using namespace rttr;
using namespace Engine;

// 생성자는 일부러 등록하지 않는다. 기본 생성자를 등록하면 TypeCatalog가
// 팩토리 없는 구체 Object 타입으로 거부한다.
RTTR_REGISTRATION_NAMED(InstanceMesh_RTTR)
{
	registration::class_<InstanceMesh>("InstanceMesh")
		.method("Clone", &InstanceMesh::Clone)
		;
}
