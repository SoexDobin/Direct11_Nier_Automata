#ifndef Engine_Define_h__
#define Engine_Define_h__

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <format>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <concepts>


#include <d3d11.h>
#define DIRECTX_TOOLKIT_IMPORT
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <directxtk/DDSTextureLoader.h>
#include <directxtk/SimpleMath.h>
#include <directxtk/WICTextureLoader.h>
#include <wrl.h>

#include <Effects11/d3dx11effect.h>


#include "fmod.hpp"
#include "fmod_common.h"
#include "fmod_errors.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include "magic_enum/magic_enum.hpp"

#pragma warning(push)
#pragma warning(disable: 26819) 
#include <nlohmann/json.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 26495) // Code Analysis: Variable is uninitialized
#pragma warning(disable : 26439) // Code Analysis: Function may not throw
#pragma warning(disable : 26819) // Code Analysis: Unannotated fallthrough
#include <rttr/registration>
#include <rttr/type>
#pragma warning(pop)

#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;
using namespace rttr;
using namespace std;
using namespace Engine;

//#ifdef _DEBUG
//#pragma comment(lib, "rttr_core_d.lib")
//#else
//#pragma comment(lib, "rttr_core.lib")
//#endif

#pragma warning(disable : 4251)

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>

#ifndef DBG_NEW
#define DBG_NEW NEW(_NORMAL_BLOCK, __FILE__, __LINE__)
#define NEW DBG_NEW
#endif

#endif

#endif // Engine_Define_h__
