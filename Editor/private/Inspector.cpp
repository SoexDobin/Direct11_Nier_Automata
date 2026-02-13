#include "pch.h"
#include "Inspector.h"

Inspector::Inspector() {}
Inspector::~Inspector() {}

HRESULT Inspector::Initialize()
{
	m_layerJsonPath = L"../Data/LayerSettings.json";
	m_tagJsonPath = L"../Data/TagSettings.json";

	auto layerRegistry = GAME->Get_LayerRegister();
	auto tagRegistry = GAME->Get_TagRegister();

	layerRegistry->LoadFromFile(m_layerJsonPath);
	tagRegistry->LoadFromFile(m_tagJsonPath);

	return EditorObject::Initialize();
}

void Inspector::Update()
{

}

void Inspector::Render()
{
	ImGui::Begin("Inspector");
	{
		ImGui::Text("Inspector Panel");
		ImGui::Separator();
		ImGui::TextDisabled("Select a GameObject to view properties");
	}
	ImGui::End();
	Inspector::LayerTagGUI();
}

void Inspector::LayerTagGUI()
{
	ImGui::Begin("Layer & Tag Settings");
	{
		auto layerRegistry = GAME->Get_LayerRegister();
		auto tagRegistry = GAME->Get_TagRegister();

		if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen)) {
			const auto& layers = layerRegistry->Get_AllLayers();

			for (const auto& [layer, name] : layers) {
				Char buffer[256] = {};
				wcstombs_s(nullptr, buffer, name.c_str(), sizeof(buffer));

				if (ImGui::InputText(std::to_string(ETOI(layer)).c_str(), buffer, sizeof(buffer))) {
					tChar wBuffer[256] = {};
					mbstowcs_s(nullptr, wBuffer, buffer, sizeof(wBuffer));
					layerRegistry->Set_LayerName(layer, wBuffer);
				}
			}
		}

		if (ImGui::CollapsingHeader("Tags", ImGuiTreeNodeFlags_DefaultOpen)) {
			const auto& tags = tagRegistry->Get_AllTags();
			for (const auto& [tag, name] : tags) {
				Char buffer[256] = {};
				wcstombs_s(nullptr, buffer, name.c_str(), sizeof(buffer));

				if (ImGui::InputText(std::to_string(ETOI(tag)).c_str(), buffer, sizeof(buffer))) {
					tChar wBuffer[256] = {};
					mbstowcs_s(nullptr, wBuffer, buffer, sizeof(wBuffer));
					tagRegistry->Set_TagName(tag, wBuffer);
				}
			}
		}
	}
	ImGui::End();
}

Shared<Inspector> Inspector::Create()
{
	auto inspector = make_shared<Inspector>();

	if (FAILED(inspector->Initialize()))
	{
		MSG_BOX("Failed To Create Inspector");
	}

	return inspector;
}
