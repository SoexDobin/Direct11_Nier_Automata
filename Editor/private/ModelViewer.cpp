#include "pch.h"
#include "ModelViewer.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Model.h"

NS_BEGIN(Editor)

ModelViewer::ModelViewer() : EditorObject{} 
{
}

HRESULT ModelViewer::Initialize() 
{
	m_Enable = false; // Initially closed
	return EditorObject::Initialize();
}

void ModelViewer::Update(Bool isResize) 
{
	if (!m_Enable) return;

	Handle_KeyInput();
	EditorObject::Update(isResize);
}

void ModelViewer::Render(Bool isResize) 
{
	if (!m_Enable) return;

	if (ImGui::Begin("Model Viewer", &m_Enable))
	{
		ImGui::Columns(2, "ModelViewerColumns");
		
		// Left: Model List
		View_ModelList();

		ImGui::NextColumn();
		
		// Right: Animation List
		View_AnimationList();

		ImGui::Columns(1);
	}
	ImGui::End();
}

void ModelViewer::View_ModelList() 
{
	ImGui::Text("Models");
	ImGui::Separator();

	if (ImGui::BeginChild("ModelListChild", ImVec2(0, 0), true))
	{
		uint32 currentLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
		auto models = GAME_INSTANCE->Get_Models(currentLevel);

		for (auto &pModel : models)
		{
			if (!pModel)
				continue;

			const wstring &wName = pModel->Get_Name();
			string utf8Name = Helper::To_String(wName);
			string displayTag = utf8Name.empty() ? "(unnamed)" : utf8Name;

			bool bSelected = (m_SelectedModelTag == wName);
			if (ImGui::Selectable(displayTag.c_str(), bSelected))
			{
				m_SelectedModelTag = wName;
				m_pSelectedModel = pModel;
				m_SelectedAnimIndex = 0;
			}
		}
	}
	ImGui::EndChild();
}

void ModelViewer::View_AnimationList() 
{
	ImGui::Text("Animations");
	ImGui::Separator();

	if (!m_pSelectedModel)
	{
		ImGui::Text("No model selected.");
		return;
	}

	uint32 numAnims = m_pSelectedModel->Get_NumAnimations();
	if (numAnims == 0)
	{
		ImGui::Text("No animations found.");
		return;
	}

	// Show 15-20 per page logic (ImGui handles scrolling automatically if we set size)
	if (ImGui::BeginChild("AnimationListChild", ImVec2(0, 0), true))
	{
		for (uint32 i = 0; i < numAnims; ++i)
		{
			const wstring& animName = m_pSelectedModel->Get_AnimationNameByIndex(i);
			string utf8Name = Helper::To_String(animName);

			bool isSelected = (m_SelectedAnimIndex == static_cast<int32>(i));
			if (ImGui::Selectable(utf8Name.c_str(), isSelected))
			{
				m_SelectedAnimIndex = i;
				m_pSelectedModel->Set_AnimationIndex(i);
			}

			// Scroll to selected if it changed via keys
			if (isSelected && ImGui::IsWindowFocused())
				ImGui::SetScrollHereY();
		}
	}
	ImGui::EndChild();
}

void ModelViewer::Handle_KeyInput() 
{
	if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		return;

	if (!m_pSelectedModel) return;

	uint32 numAnims = m_pSelectedModel->Get_NumAnimations();
	if (numAnims == 0) return;

	if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
	{
		m_SelectedAnimIndex--;
		if (m_SelectedAnimIndex < 0) m_SelectedAnimIndex = numAnims - 1;
		m_pSelectedModel->Set_AnimationIndex(m_SelectedAnimIndex);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
	{
		m_SelectedAnimIndex++;
		if (m_SelectedAnimIndex >= static_cast<int32>(numAnims)) m_SelectedAnimIndex = 0;
		m_pSelectedModel->Set_AnimationIndex(m_SelectedAnimIndex);
	}
}

Shared<ModelViewer> ModelViewer::Create() 
{
	auto modelViewer = make_shared<ModelViewer>();
	if (FAILED(modelViewer->Initialize())) return nullptr;
	return modelViewer;
}

NS_END
