#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>
#include <list>
#include <map>
#include <string>
#include <memory>
#include <algorithm>

// Engine Headers
#include "Engine_Define.h"
#include "Game.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

// ImGui Headers
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Editor Headers
#include "Editor_Define.h"

#include "ID_Helper.h"
#include "String_Helper.h"

using namespace Engine;
using namespace Helper;
using namespace std;
