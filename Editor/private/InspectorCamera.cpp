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
	void QueueCameraValue(const Shared<GameObject>& owner, Object& object,
		std::string_view propertyName, const T& before, const T& after)
	{
		ReflectionValue beforeValue;
		ReflectionValue afterValue;
		beforeValue.data = before;
		afterValue.data = after;
		EDITOR->Queue_PropertyWrite(owner, object, propertyName,
			beforeValue, afterValue,
			ImGui::IsItemActivated() || !ImGui::IsItemActive());
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
		if (ReadCameraValue(*pObj, "FovY", fovY)) {
			const Float beforeFovY = fovY;
			if (ImGui::SliderAngle("FovY", &fovY, 1.0f, 179.0f))
				QueueCameraValue(pObj, *pObj, "FovY", beforeFovY, fovY);
        }

        // 2. Aspect
        Float aspect{};
		if (ReadCameraValue(*pObj, "Aspect", aspect)) {
			const Float beforeAspect = aspect;
			if (ImGui::DragFloat("Aspect", &aspect, 0.01f, 0.1f, 10.0f))
				QueueCameraValue(pObj, *pObj, "Aspect", beforeAspect, aspect);
        }

        // 3. Near / Far
        Float nearPlane{};
		if (ReadCameraValue(*pObj, "Near", nearPlane)) {
			const Float beforeNear = nearPlane;
			if (ImGui::DragFloat("Near", &nearPlane, 0.01f, 0.001f, 1000.0f))
				QueueCameraValue(pObj, *pObj, "Near", beforeNear, nearPlane);
        }

        Float farPlane{};
		if (ReadCameraValue(*pObj, "Far", farPlane)) {
			const Float beforeFar = farPlane;
			if (ImGui::DragFloat("Far", &farPlane, 1.0f, 1.0f, 10000.0f))
				QueueCameraValue(pObj, *pObj, "Far", beforeFar, farPlane);
        }
        
	}
}

Shared<InspectorCamera> InspectorCamera::Create()
{
	auto instance = make_shared<InspectorCamera>();
	if (FAILED(instance->Initialize())) return nullptr;
	return instance;
}
