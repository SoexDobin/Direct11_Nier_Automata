#include "pch.h"
#include "EditorManager.h"
#include "Game.h"

#include "Inspector.h"
#include "EditorView.h"

IMPLEMENT_SINGLETON(EditorManager)

EditorManager::EditorManager()
	: m_Inspector { nullptr },
	m_EditorView { nullptr }
{
	
}

EditorManager::~EditorManager()
{
	m_Inspector = nullptr;
	m_EditorView = nullptr;
}

HRESULT EditorManager::Initialize()
{
	if (nullptr == (m_Inspector = Inspector::Create()))
		return E_FAIL;
	if (nullptr == (m_EditorView = EditorView::Create()))
		return E_FAIL;

	return S_OK;
}

void EditorManager::Update()
{
	m_Inspector->Update();
	m_EditorView->Update();

}

void EditorManager::Render() 
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

	m_Inspector->Render();
	m_EditorView->Render();
	
	ImGui::Render();

	if (FAILED(GAME->Clear_BackBufferView(make_shared<Float4>(0.1f, 0.1f, 0.1f, 1.f))))
	{
		LOG_CRITICAL(L"Failed To Clear Back Buffer");
		return;
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	if (FAILED(GAME->Present()))
	{
		LOG_CRITICAL(L"Failed To Present SwapChain");
		return;
	}
}
