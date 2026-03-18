#include "pch.h"
#include "AssetBrowser.h"
#include "EditorManager.h"
#include "Game.h"
#include "GameObject.h"
#include "String_Helper.h"

using namespace Engine;

AssetBrowser::AssetBrowser() {}

HRESULT AssetBrowser::Initialize()
{
	return EditorObject::Initialize();
}

void AssetBrowser::Update(Bool isResize)
{
	EditorObject::Update(isResize);
}

void AssetBrowser::Render(Bool isResize)
{
	if (!m_Enable)
		return;

	ImGui::Begin("Assets");
	Render_PrototypeList();
	ImGui::End();
}

void AssetBrowser::Render_PrototypeList()
{
	uint32 levIndex = GAME_INSTANCE->Get_CurrentLevelIndex();
	
	// ── 1. Global Prototypes (Level 0) ──
	if (ImGui::CollapsingHeader("Global Prototypes (Level 0)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const auto &staticMap = GAME_INSTANCE->Get_Prototypes(0);
		for (auto &[objectID, staticProto] : staticMap)
		{
			if (!staticProto)
			{
				continue;
			}

			Render_PrototypeItem(staticProto, objectID, 0);
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// ── 2. Current Level Prototypes ──
	string currentHeader = "Current Level Prototypes (Level " + std::to_string(levIndex) + ")";
	if (ImGui::CollapsingHeader(currentHeader.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (levIndex == 0)
		{
			ImGui::TextDisabled("  (Same as Global)");
		}
		else
		{
			const auto &protoMap = GAME_INSTANCE->Get_Prototypes(levIndex);
			for (auto &[objectID, proto] : protoMap)
			{
				if (!proto)
				{
					continue;
				}

				Render_PrototypeItem(proto, objectID, levIndex);
			}
		}
	}
}

void AssetBrowser::Render_PrototypeItem(const Shared<GameObject> &pProto, uint32 objectID, uint32 levIndex)
{
	const wstring &wName = pProto->Get_Name();
	wstring protoTag = GAME_INSTANCE->Get_PrototypeTagFromObjectID(objectID, levIndex);

	// 태그가 없으면 이름을 대신 사용하되, 앞에 표시하여 구분
	wstring displayLabel = L"[Proto] " + protoTag + L"_" + std::to_wstring(objectID);

	Bool bSelected = false;
	ImGui::Selectable(To_String(displayLabel).c_str(), &bSelected, ImGuiSelectableFlags_None, ImVec2(0.f, 24.f));

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{

		size_t byteSize = (wName.size() + 1) * sizeof(wchar_t);
		ImGui::SetDragDropPayload(Drag_PayLoadKey.c_str(), protoTag.c_str(), static_cast<int32>(byteSize));

		ImGui::Text("Dragging: %s", protoTag.c_str());
		if (protoTag.empty())
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "(No Prototype Tag!)");
		}
		
		ImGui::EndDragDropSource();
	}
}

Shared<AssetBrowser> AssetBrowser::Create()
{
	auto instance = make_shared<AssetBrowser>();

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed To Create AssetBrowser");
		return nullptr;
	}

	return instance;
}
