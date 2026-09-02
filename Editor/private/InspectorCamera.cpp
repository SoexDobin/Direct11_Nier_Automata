#include "pch.h"
#include "InspectorCamera.h"
#include "Editor_Define.h"
#include "EditorManager.h"
#include "Game.h"
#include "GameObject.h"
#include "String_Helper.h"
#include "SpdLogger.h"

using namespace Editor;
using namespace Engine;

namespace
{
	template <typename T>
	Bool ReadCameraValue(Object& object, std::string_view propertyName, T& outValue)
	{
		ReflectionValue reflectedValue;
		if (FAILED(GAME_INSTANCE->Read_ReflectedProperty(object, propertyName, reflectedValue)))
			return false;
		const T* value = reflectedValue.Try_Get<T>();
		if (!value)
			return false;
		outValue = *value;
		return true;
	}

	template <typename T>
	Bool WriteCameraValue(Object& object, std::string_view propertyName, const T& value)
	{
		ReflectionValue reflectedValue;
		reflectedValue.data = value;
		return SUCCEEDED(GAME_INSTANCE->Write_ReflectedProperty(
			object, propertyName, reflectedValue));
	}
}

HRESULT InspectorCamera::Initialize()
{
    return S_OK;
}

void InspectorCamera::RenderCamera(const Shared<GameObject>& pObj)
{
	if (!pObj) return;

    if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 1. FovY
        Float fovY{};
        if (ReadCameraValue(*pObj, "FovY", fovY) &&
			ImGui::SliderAngle("FovY", &fovY, 1.0f, 179.0f)) {
            WriteCameraValue(*pObj, "FovY", fovY);
        }

        // 2. Aspect
        Float aspect{};
        if (ReadCameraValue(*pObj, "Aspect", aspect) &&
			ImGui::DragFloat("Aspect", &aspect, 0.01f, 0.1f, 10.0f)) {
            WriteCameraValue(*pObj, "Aspect", aspect);
        }

        // 3. Near / Far
        Float nearPlane{};
        if (ReadCameraValue(*pObj, "Near", nearPlane) &&
			ImGui::DragFloat("Near", &nearPlane, 0.01f, 0.001f, 1000.0f)) {
            WriteCameraValue(*pObj, "Near", nearPlane);
        }

        Float farPlane{};
        if (ReadCameraValue(*pObj, "Far", farPlane) &&
			ImGui::DragFloat("Far", &farPlane, 1.0f, 1.0f, 10000.0f)) {
            WriteCameraValue(*pObj, "Far", farPlane);
        }
        
	}
}

Shared<InspectorCamera> InspectorCamera::Create()
{
	auto instance = make_shared<InspectorCamera>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
