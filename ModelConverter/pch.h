#ifndef PCH_H
#define PCH_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "Engine_Define.h"
#include <DirectXMath.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define BIN(_DATA_) reinterpret_cast<const Char*>(_DATA_)

namespace Tool
{
}
using namespace Tool;

using namespace Engine;
using namespace Tool;

#endif //PCH_H
