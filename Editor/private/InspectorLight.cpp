#include "pch.h"
#include "InspectorLight.h"
#include "Editor_Define.h"
#include "EditorManager.h"
#include "LightGameObject.h" // Engine 헤더 위치 참조
#include "Game.h"
#include "GameObject.h"
using namespace Editor;
using namespace Engine;
HRESULT InspectorLight::Initialize()
{
    return S_OK;
}
void InspectorLight::RenderLight(const Shared<GameObject>& pObj)
{
    auto pLightObj = dynamic_pointer_cast<LightGameObject>(pObj);
    if (!pLightObj && pObj->Get_GameObjectType() != GAMEOBJECTTYPE::LIGHT) return;
    
    if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // pLightObj가 nullptr이면 원래 방식대로 캐스팅 (방어 코드)
        if (!pLightObj) pLightObj = static_pointer_cast<LightGameObject>(pObj);

        LIGHT_DESC& desc = pLightObj->Get_LightDesc_Ref();
        // 1. Light Type (ComboBox)
        int lightType = static_cast<int32>(desc.type);
        const Char* typeItems[] = { "DIRECTIONAL", "POINT", "SPOT" };
        if (ImGui::Combo("Light Type", &lightType, typeItems, IM_ARRAYSIZE(typeItems)))
        {
            desc.type = static_cast<LIGHT>(lightType);

            // Directional로 바뀌면 별도의 위치보단 방향만 쓴다는 제약 등이 있으니 UI 리프레시 용도
        }
        // 2. Range
        if (ImGui::DragFloat("Range", &desc.range, 0.1f, 0.1f, 1000.0f))
        {
            // 값이 드래그될 때 자동 반영 (참조 중이므로 렌더링에 이식됨)
        }
        // 3. Diffuse Color
        ImGui::ColorEdit4("Diffuse", reinterpret_cast<Float*>(&desc.diffuse));
        // 4. Ambient Color
        ImGui::ColorEdit4("Ambient", reinterpret_cast<Float*>(&desc.ambient));
        // 5. Specular Color
        ImGui::ColorEdit4("Specular", reinterpret_cast<Float*>(&desc.specular));
        // ※ Position 등은 LightGameObject 내부의 Late_Update()에서
        // pLightObj->Get_Transform()->Get_Position()값을 이용해 
        // 매 프레임 desc.position에 동기화 하므로 여기서는 에디팅 안 해도 됩니다.

        ImGui::Spacing();
    }
}
Shared<InspectorLight> InspectorLight::Create()
{
    auto instance = make_shared<InspectorLight>();
    if (FAILED(instance->Initialize())) return nullptr;
    return instance;
}