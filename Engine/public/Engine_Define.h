#ifndef Engine_Define_h__
#define Engine_Define_h__

#include <d3d11.h>
#include <DirectXMath.h>
#include "directxtk/SimpleMath.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <wrl.h>

#include "spdlog/spdlog.h"
#include "magic_enum/magic_enum.hpp"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

#include <algorithm>
#include <ctime>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"

#include "fmod.hpp"
#include "fmod_common.h"
#include "fmod_errors.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#ifndef DBG_NEW

#define DBG_NEW NEW(_NORMAL_BLOCK, __FILE__, __LINE__)
#define NEW DBG_NEW

#endif
#endif

using namespace std;
using namespace Engine;

#endif // Engine_Define_h__
