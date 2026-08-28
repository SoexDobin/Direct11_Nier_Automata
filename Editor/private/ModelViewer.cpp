#include "pch.h"
#include "ModelViewer.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Model.h"
#include "Engine_ID.h"
#include "PathManager.h"

#include <fstream>
#include <nlohmann/json.hpp>

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
		ImGui::Separator();
		View_AnimationPreset();
	}
	ImGui::End();
}

void ModelViewer::View_AnimationPreset()
{
	if (!ImGui::CollapsingHeader("Animation Preset", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	ImGui::InputText("Preset Name", m_PresetName, sizeof(m_PresetName));
	ImGui::InputText("AssetGuid", m_PresetGuid, sizeof(m_PresetGuid), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::Button("New Preset"))
		New_AnimationPreset();
	ImGui::SameLine();
	if (ImGui::Button("Save Preset"))
		Save_AnimationPreset();
	ImGui::SameLine();
	if (ImGui::Button("Apply To Selected Model"))
		Apply_AnimationPreset();

	if (!m_PresetStatus.empty())
		ImGui::TextWrapped("%s", m_PresetStatus.c_str());

	ImGui::Columns(3, "AnimationPresetColumns");
	ImGui::Text("Available .anim");
	ImGui::Separator();
	const filesystem::path resourceDirectory = PATH.GetResourceDir();
	if (filesystem::exists(resourceDirectory))
	{
		for (const auto& entry : filesystem::recursive_directory_iterator(resourceDirectory))
		{
			if (!entry.is_regular_file() || entry.path().extension() != L".anim")
				continue;
			const filesystem::path relativePath = filesystem::relative(entry.path(), resourceDirectory);
			const string label = Helper::To_String(relativePath.generic_wstring());
			if (ImGui::Selectable(label.c_str(), false))
			{
				const wstring normalizedPath = relativePath.generic_wstring();
				if (find(m_PresetAnimationPaths.begin(), m_PresetAnimationPaths.end(), normalizedPath) ==
					m_PresetAnimationPaths.end())
					m_PresetAnimationPaths.push_back(normalizedPath);
			}
		}
	}
	else
	{
		ImGui::TextDisabled("Resource directory not found");
	}

	ImGui::NextColumn();
	ImGui::Text("Preset Clips");
	ImGui::Separator();
	for (size_t i = 0; i < m_PresetAnimationPaths.size();)
	{
		ImGui::PushID(static_cast<int32>(i));
		ImGui::TextWrapped("%s", Helper::To_String(m_PresetAnimationPaths[i]).c_str());
		if (ImGui::SmallButton("Up") && i > 0)
			std::swap(m_PresetAnimationPaths[i], m_PresetAnimationPaths[i - 1]);
		ImGui::SameLine();
		if (ImGui::SmallButton("Down") && i + 1 < m_PresetAnimationPaths.size())
			std::swap(m_PresetAnimationPaths[i], m_PresetAnimationPaths[i + 1]);
		ImGui::SameLine();
		if (ImGui::SmallButton("Remove"))
		{
			m_PresetAnimationPaths.erase(m_PresetAnimationPaths.begin() + i);
			ImGui::PopID();
			continue;
		}
		ImGui::Separator();
		ImGui::PopID();
		++i;
	}

	ImGui::NextColumn();
	ImGui::Text("Saved Presets");
	ImGui::Separator();
	const filesystem::path presetDirectory = PATH.GetAnimationPresetSettingsDir();
	if (filesystem::exists(presetDirectory))
	{
		for (const auto& entry : filesystem::directory_iterator(presetDirectory))
		{
			if (!entry.is_regular_file() || entry.path().extension() != L".json")
				continue;
			const string label = Helper::To_String(entry.path().stem().wstring());
			if (ImGui::Selectable(label.c_str(), false))
				Load_AnimationPreset(entry.path());
		}
	}
	ImGui::Columns(1);
}

void ModelViewer::New_AnimationPreset()
{
	const AssetGuid assetGuid = Create_AssetGuid();
	const string guidText = To_String(assetGuid);
	strncpy_s(m_PresetGuid, guidText.c_str(), _TRUNCATE);
	m_PresetName[0] = '\0';
	m_PresetAnimationPaths.clear();
	m_PresetStatus = assetGuid.Is_Valid() ? "Created a new preset identity." : "Failed to create AssetGuid.";
}

void ModelViewer::Save_AnimationPreset()
{
	AssetGuid assetGuid{};
	if (m_PresetName[0] == '\0' || !Try_Parse_AssetGuid(m_PresetGuid, assetGuid) ||
		m_PresetAnimationPaths.empty())
	{
		m_PresetStatus = "Preset name, valid AssetGuid, and at least one clip are required.";
		return;
	}

	string fileName = m_PresetName;
	for (Char& ch : fileName)
	{
		if (static_cast<unsigned char>(ch) < 0x20 || ch == '<' || ch == '>' || ch == ':' ||
			ch == '"' || ch == '/' || ch == '\\' || ch == '|' || ch == '?' || ch == '*')
			ch = '_';
	}
	if (fileName.empty())
	{
		m_PresetStatus = "Preset file name is invalid.";
		return;
	}

	nlohmann::json document;
	document["schemaVersion"] = 1;
	document["assetGuid"] = To_String(assetGuid);
	document["name"] = string{ m_PresetName };
	document["animations"] = nlohmann::json::array();
	for (const wstring& path : m_PresetAnimationPaths)
		document["animations"].push_back({ {"path", Helper::To_String(path)} });

	const filesystem::path directory = PATH.GetAnimationPresetSettingsDir();
	std::error_code errorCode;
	filesystem::create_directories(directory, errorCode);
	if (errorCode)
	{
		m_PresetStatus = "Failed to create AnimationPreset directory.";
		return;
	}

	ofstream out(directory / filesystem::path(fileName + ".json"));
	if (!out.is_open())
	{
		m_PresetStatus = "Failed to open AnimationPreset output file.";
		return;
	}
	out << document.dump(4);
	m_PresetStatus = out.good() ? "AnimationPreset saved." : "Failed while writing AnimationPreset.";
}

void ModelViewer::Load_AnimationPreset(const filesystem::path& presetPath)
{
	try
	{
		ifstream in(presetPath);
		nlohmann::json document;
		in >> document;
		AssetGuid assetGuid{};
		const string guidText = document.value("assetGuid", string{});
		if (document.value("schemaVersion", 0) != 1 || !Try_Parse_AssetGuid(guidText, assetGuid) ||
			!document.contains("animations") || !document["animations"].is_array())
			throw std::runtime_error("invalid preset schema");

		const string presetName = document.value("name", presetPath.stem().string());
		strncpy_s(m_PresetName, presetName.c_str(), _TRUNCATE);
		strncpy_s(m_PresetGuid, guidText.c_str(), _TRUNCATE);
		m_PresetAnimationPaths.clear();
		for (const auto& animation : document["animations"])
		{
			const string path = animation.is_string() ? animation.get<string>() : animation.value("path", string{});
			if (!path.empty())
				m_PresetAnimationPaths.push_back(Helper::To_wString(path));
		}
		m_PresetStatus = "AnimationPreset loaded.";
	}
	catch (const std::exception& exception)
	{
		m_PresetStatus = string{ "Failed to load AnimationPreset: " } + exception.what();
	}
}

void ModelViewer::Apply_AnimationPreset()
{
	if (!m_pSelectedModel || m_PresetAnimationPaths.empty())
	{
		m_PresetStatus = "Select a model and add at least one clip.";
		return;
	}

	vector<wstring> fullPaths;
	for (const wstring& relativePath : m_PresetAnimationPaths)
	{
		const filesystem::path fullPath = filesystem::path(PATH.GetResourceDir()) / relativePath;
		if (!filesystem::exists(fullPath))
		{
			m_PresetStatus = "Animation clip not found: " + Helper::To_String(fullPath.wstring());
			return;
		}
		fullPaths.push_back(fullPath.wstring());
	}

	m_PresetStatus = SUCCEEDED(m_pSelectedModel->Load_Animations(fullPaths))
		? "AnimationPreset applied to selected model."
		: "AnimationPreset is incompatible with the selected model.";
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
