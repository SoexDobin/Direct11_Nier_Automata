#ifndef PCH_H
#define PCH_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "Engine_Define.h"
#include <DirectXMath.h>

#include "Assimp/scene.h"
#include "Assimp/Importer.hpp"
#include "Assimp/postprocess.h"

#define BIN(_DATA_) reinterpret_cast<const Char*>(_DATA_)

namespace Tool
{
}
using namespace Tool;

using namespace Engine;
using namespace Tool;

#endif //PCH_H
