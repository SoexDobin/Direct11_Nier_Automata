#include "pch.h"
#include "ModelViewer.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Model.h"
#include "Shader.h"
#include "PathManager.h"

#include <imgui_stdlib.h>
#include <nlohmann/json.hpp>

NS_BEGIN(Editor)

namespace
{
	string To_Utf8Path(const filesystem::path& path)
	{
		const std::u8string value = path.generic_u8string();
		return string{ value.begin(), value.end() };
	}

	const Char* TextureSlotForAssimpType(uint32 typeIndex)
	{
		switch (typeIndex)
		{
		case 1:
		case 12: return "g_AlbedoMap";
		case 4:
		case 14: return "g_EmissiveMap";
		case 6:
		case 13: return "g_NormalMap";
		case 8: return "g_OpacityMap";
		default: return nullptr;
		}
	}

	Bool JsonStringArrayContains(const nlohmann::json& values, const string& target)
	{
		if (!values.is_array()) return false;
		return std::ranges::any_of(values, [&](const nlohmann::json& value) {
			return value.is_string() && value.get<string>() == target;
		});
	}

	Shared<Engine::Shader> MaterialShader(const Shared<Engine::Model>& model)
	{
		if (!model) return nullptr;
		return GAME_INSTANCE->Get_Shader(0,
			(model->Is_Skeletal() ? Engine::VTXANIMMESH::Tag : Engine::VTXMESH::Tag).c_str());
	}
}

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
		
		// Right: animation and ModelTag-owned material authoring.
		if (ImGui::BeginTabBar("ModelViewerTabs"))
		{
			if (ImGui::BeginTabItem("Animations"))
			{
				View_AnimationList();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Materials"))
			{
				View_MaterialSettings();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

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

			const wstring &wName = pModel->Get_ModelTag();
			string utf8Name = Helper::To_String(wName);
			string displayTag = utf8Name.empty() ? "(unnamed)" : utf8Name;

			bool bSelected = (m_SelectedModelTag == wName);
			if (ImGui::Selectable(displayTag.c_str(), bSelected))
			{
				m_SelectedModelTag = wName;
				m_pSelectedModel = pModel;
				m_SelectedAnimIndex = 0;
				m_PreviewMeshIndex = -1;
				Load_MaterialDraft();
			}
		}
	}
	ImGui::EndChild();
}

filesystem::path ModelViewer::Get_MaterialSettingsPath() const
{
	if (!m_pSelectedModel || m_pSelectedModel->Get_ModelTag().empty())
		return {};
	return filesystem::path(PATH.GetMaterialSettingsDir()) /
		Engine::Model::Make_MaterialSettingsFileName(m_pSelectedModel->Get_ModelTag());
}

void ModelViewer::Load_MaterialDraft()
{
	m_HasMaterialDraft = false;
	m_MaterialDraft = {};
	if (!m_pSelectedModel)
	{
		m_MaterialStatus = "Select a model first.";
		return;
	}

	const filesystem::path settingsPath = Get_MaterialSettingsPath();
	if (filesystem::is_regular_file(settingsPath))
	{
		if (FAILED(m_pSelectedModel->Load_MaterialSettings(
			settingsPath, m_pSelectedModel->Get_ModelTag(), MaterialShader(m_pSelectedModel))))
		{
			m_MaterialStatus = "Material settings are invalid; the previous in-memory snapshot was preserved.";
			return;
		}
	}

	const Shared<const Engine::MODEL_MATERIAL_SETTINGS> settings =
		m_pSelectedModel->Get_MaterialSettings();
	if (!settings)
	{
		m_MaterialStatus = "No authored settings. Generate a draft from the Converter inspection JSON.";
		return;
	}

	m_MaterialDraft = *settings;
	m_HasMaterialDraft = true;
	m_MaterialStatus = "Loaded ModelTag material settings.";
}

void ModelViewer::Generate_MaterialDraft()
{
	m_HasMaterialDraft = false;
	m_MaterialDraft = {};
	if (!m_pSelectedModel)
	{
		m_MaterialStatus = "Select a model first.";
		return;
	}
	if (filesystem::is_regular_file(Get_MaterialSettingsPath()))
	{
		m_MaterialStatus = "An authored settings file already exists. Reload it instead of overwriting it from conversion data.";
		return;
	}

	filesystem::path converterPath = m_pSelectedModel->Get_ModelFilePath();
	converterPath.replace_extension(L".json");
	try
	{
		ifstream in(converterPath, ios::binary);
		if (!in.is_open())
		{
			m_MaterialStatus = "Converter inspection JSON was not found next to the model.";
			return;
		}
		nlohmann::json converter;
		in >> converter;
		if (!converter.contains("materials") || !converter["materials"].is_array())
		{
			m_MaterialStatus = "Converter inspection JSON has no material array.";
			return;
		}

		const vector<string> modelMaterialNames = m_pSelectedModel->Get_MaterialNames();
		if (converter["materials"].size() != modelMaterialNames.size())
		{
			m_MaterialStatus = "Converter material count does not match the loaded model.";
			return;
		}

		Engine::MODEL_MATERIAL_SETTINGS draft;
		draft.modelTag = Helper::To_String(m_pSelectedModel->Get_ModelTag());
		draft.modelPath = m_pSelectedModel->Get_ModelResourcePath();
		draft.materials.reserve(modelMaterialNames.size());

		const nlohmann::json emptyArray = nlohmann::json::array();
		const nlohmann::json inspection = converter.value("inspection", nlohmann::json::object());
		const nlohmann::json reference = inspection.value("referenceMaterials", nlohmann::json::object());
		const nlohmann::json exactMatches = reference.value("exactMatches", emptyArray);
		const nlohmann::json duplicateConverter = inspection.value("duplicateConverterMaterialNames", emptyArray);
		const nlohmann::json duplicateSource = reference.value("duplicateSourceMaterialNames", emptyArray);
		const nlohmann::json converterOnly = reference.value("converterOnlyMaterials", emptyArray);

		for (size_t arrayIndex = 0; arrayIndex < converter["materials"].size(); ++arrayIndex)
		{
			const nlohmann::json& source = converter["materials"][arrayIndex];
			Engine::MODEL_MATERIAL_SETTING setting;
			setting.materialIndex = source.value("materialIndex", static_cast<uint32>(arrayIndex));
			setting.sourceMaterialName = source.value("name", string{});
			if (setting.materialIndex >= modelMaterialNames.size() ||
				setting.sourceMaterialName != modelMaterialNames[setting.materialIndex])
			{
				m_MaterialStatus = "Converter material identity does not match the loaded model.";
				return;
			}

			for (const nlohmann::json& match : exactMatches)
			{
				if (match.value("materialIndex", UINT32_MAX) != setting.materialIndex ||
					match.value("materialName", string{}) != setting.sourceMaterialName)
					continue;
				setting.sourceShaderName = match.value("sourceShaderName", string{});
				setting.sourceTechniqueName = match.value("sourceTechniqueName", string{});
				break;
			}

			const Bool unresolved = JsonStringArrayContains(duplicateConverter, setting.sourceMaterialName) ||
				JsonStringArrayContains(duplicateSource, setting.sourceMaterialName) ||
				JsonStringArrayContains(converterOnly, setting.sourceMaterialName);
			if (unresolved)
				setting.passName = "Unresolved";
			else
				// Source shader names are provenance, not implemented engine pass names.
				setting.passName = "Default_Pass";

			if (source.contains("textures") && source["textures"].is_array())
			{
				for (const nlohmann::json& texture : source["textures"])
				{
					const Char* slotName = TextureSlotForAssimpType(texture.value("type", UINT32_MAX));
					const string importedPath = texture.value("path", string{});
					if (!slotName || importedPath.empty() || importedPath.find("..") != string::npos)
						continue;
					if (std::ranges::any_of(setting.textures, [&](const Engine::MATERIAL_TEXTURE_SETTING& value) {
						return value.slotName == slotName;
					}))
						continue;

					const filesystem::path relativeModelDirectory =
						filesystem::path(Helper::To_wString(draft.modelPath)).parent_path();
					filesystem::path relativeTexturePath =
						(relativeModelDirectory / filesystem::path(Helper::To_wString(importedPath))).lexically_normal();
					const filesystem::path fallbackPath = relativeModelDirectory / L"Textures" /
						filesystem::path(Helper::To_wString(importedPath)).filename();
					if (filesystem::is_regular_file(filesystem::path(PATH.GetResourceDir()) / fallbackPath))
						relativeTexturePath = fallbackPath;
					setting.textures.push_back({ slotName, To_Utf8Path(relativeTexturePath) });
				}
			}
			draft.materials.push_back(std::move(setting));
		}

		std::ranges::sort(draft.materials, {}, &Engine::MODEL_MATERIAL_SETTING::materialIndex);
		m_MaterialDraft = std::move(draft);
		m_HasMaterialDraft = true;
		m_MaterialStatus = "Generated an unsaved draft from Converter inspection data.";
	}
	catch (const std::exception& exception)
	{
		m_MaterialStatus = string("Failed to generate material draft: ") + exception.what();
	}
}

void ModelViewer::Save_MaterialDraft()
{
	if (!m_pSelectedModel || !m_HasMaterialDraft)
	{
		m_MaterialStatus = "There is no material draft to save.";
		return;
	}
	Save_MaterialSettings(m_pSelectedModel, m_MaterialDraft, Get_MaterialSettingsPath(), m_MaterialStatus);
}

HRESULT ModelViewer::Save_MaterialSettings(const Shared<Engine::Model>& model,
	const Engine::MODEL_MATERIAL_SETTINGS& draft, const filesystem::path& target, string& status)
{
	if (!model || target.empty())
	{
		status = "A model and target path are required.";
		return E_INVALIDARG;
	}

	nlohmann::json document{
		{ "schemaVersion", 1 },
		{ "modelTag", draft.modelTag },
		{ "modelPath", draft.modelPath },
		{ "materials", nlohmann::json::array() }
	};
	for (const Engine::MODEL_MATERIAL_SETTING& setting : draft.materials)
	{
		nlohmann::json textures = nlohmann::json::object();
		for (const Engine::MATERIAL_TEXTURE_SETTING& texture : setting.textures)
		{
			if (texture.slotName.empty() || textures.contains(texture.slotName))
			{
				status = "Texture slot names must be non-empty and unique per material.";
				return E_INVALIDARG;
			}
			textures[texture.slotName] = texture.resourcePath;
		}
		document["materials"].push_back({
			{ "materialIndex", setting.materialIndex },
			{ "sourceMaterialName", setting.sourceMaterialName },
			{ "sourceShaderName", setting.sourceShaderName },
			{ "sourceTechniqueName", setting.sourceTechniqueName },
			{ "passName", setting.passName },
			{ "renderGroup", setting.isBlend ? "BLEND" : "NONBLEND" },
			{ "textures", std::move(textures) }
		});
	}

	const filesystem::path temporary = target.wstring() + L".tmp";
	std::error_code errorCode;
	filesystem::create_directories(target.parent_path(), errorCode);
	if (errorCode)
	{
		status = "Failed to create the material settings directory.";
		return E_FAIL;
	}
	{
		ofstream out(temporary, ios::binary | ios::trunc);
		if (!out.is_open())
		{
			status = "Failed to open the temporary material settings file.";
			return E_FAIL;
		}
		out << document.dump(4);
		out.flush();
		if (!out.good())
		{
			status = "Failed while writing temporary material settings.";
			return E_FAIL;
		}
	}

	const auto shader = MaterialShader(model);
	Shared<const Engine::MODEL_MATERIAL_SETTINGS> prepared;
	if (!shader || model->Prepare_MaterialSettings(
		temporary, model->Get_ModelTag(), prepared, shader) != S_OK)
	{
		filesystem::remove(temporary, errorCode);
		status = "Draft validation failed; the existing settings file was preserved.";
		return E_FAIL;
	}
	if (!MoveFileExW(temporary.c_str(), target.c_str(),
		MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
	{
		filesystem::remove(temporary, errorCode);
		status = "Failed to atomically replace the material settings file.";
		return E_FAIL;
	}
	if (FAILED(model->Apply_MaterialSettings(prepared)))
	{
		status = "Saved settings could not be applied to the preview model.";
		return E_FAIL;
	}
	status = "Material settings saved atomically and applied to the preview model.";
	return S_OK;
}

void ModelViewer::View_MaterialSettings()
{
	ImGui::Text("ModelTag Material Settings");
	ImGui::Separator();
	if (!m_pSelectedModel)
	{
		ImGui::Text("No model selected.");
		return;
	}
	ComPtr<ID3D11ShaderResourceView> preview;
	if (SUCCEEDED(Preview_Model(m_pSelectedModel, preview, m_PreviewMeshIndex)))
	{
		ImGui::TextUnformatted("Saved material preview");
		ImGui::Image(reinterpret_cast<ImTextureID>(preview.Get()), ImVec2(384.f, 256.f));
	}
	else ImGui::TextUnformatted("Preview unavailable: check the material pass and texture slots.");

	ImGui::TextWrapped("ModelTag: %s", Helper::To_String(m_pSelectedModel->Get_ModelTag()).c_str());
	ImGui::TextWrapped("Model: %s", m_pSelectedModel->Get_ModelResourcePath().c_str());
	if (ImGui::Button("Preview All Meshes")) m_PreviewMeshIndex = -1;
	ImGui::SameLine();
	if (m_PreviewMeshIndex < 0) ImGui::TextUnformatted("All meshes");
	else ImGui::Text("Mesh %d only", m_PreviewMeshIndex);
	if (ImGui::CollapsingHeader("Mesh / Material Mapping", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const auto names = m_pSelectedModel->Get_MaterialNames();
		if (ImGui::BeginChild("MeshMaterialMapping", ImVec2(0, 160.f), true))
		{
			for (uint32 meshIndex = 0; meshIndex < m_pSelectedModel->Get_NumMeshes(); ++meshIndex)
			{
				string meshName;
				uint32 materialIndex{};
				if (FAILED(m_pSelectedModel->Get_MeshMaterialInfo(meshIndex, meshName, materialIndex))) continue;
				const string label = std::to_string(meshIndex) + ": " + meshName + " -> " +
					std::to_string(materialIndex) + " / " +
					(materialIndex < names.size() ? names[materialIndex] : "INVALID") + "##mesh" + std::to_string(meshIndex);
				if (ImGui::Selectable(label.c_str(), m_PreviewMeshIndex == static_cast<int32>(meshIndex)))
					m_PreviewMeshIndex = static_cast<int32>(meshIndex);
			}
		}
		ImGui::EndChild();
	}
	ImGui::TextWrapped("Mesh mapping is imported from the model. Save applies the draft to the preview; existing instances keep their snapshot until reloaded.");
	if (ImGui::Button("Reload Authored Settings")) Load_MaterialDraft();
	ImGui::SameLine();
	if (ImGui::Button("Generate Default Draft")) Generate_MaterialDraft();
	ImGui::SameLine();
	if (ImGui::Button("Save Atomically")) Save_MaterialDraft();
	if (!m_MaterialStatus.empty()) ImGui::TextWrapped("%s", m_MaterialStatus.c_str());
	if (!m_HasMaterialDraft) return;

	if (ImGui::BeginChild("MaterialSettingsEditor", ImVec2(0, 0), true))
	{
		for (Engine::MODEL_MATERIAL_SETTING& setting : m_MaterialDraft.materials)
		{
			ImGui::PushID(static_cast<int>(setting.materialIndex));
			const string heading = std::to_string(setting.materialIndex) + " - " + setting.sourceMaterialName;
			if (ImGui::CollapsingHeader(heading.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::TextWrapped("Source: %s / %s", setting.sourceShaderName.c_str(), setting.sourceTechniqueName.c_str());
				ImGui::TextWrapped("Source shader metadata does not reproduce its shading. Concrete passes are authored separately.");
				ImGui::InputText("Pass", &setting.passName);
				const auto shader = MaterialShader(m_pSelectedModel);
				if (!shader || !shader->Has_Pass(setting.passName))
					ImGui::TextColored(ImVec4(1.f, 0.4f, 0.3f, 1.f), "Pass is not available in this model shader.");
				int renderGroup = setting.isBlend ? 1 : 0;
				if (ImGui::Combo("Render Group", &renderGroup, "NONBLEND\0BLEND\0"))
					setting.isBlend = renderGroup == 1;
				ImGui::TextWrapped("Render Group is stored metadata; per-mesh queue separation is not implemented yet.");

				for (size_t textureIndex = 0; textureIndex < setting.textures.size();)
				{
					ImGui::PushID(static_cast<int>(textureIndex));
					ImGui::InputText("Slot", &setting.textures[textureIndex].slotName);
					if (!shader || !shader->Has_SRV(setting.textures[textureIndex].slotName))
						ImGui::TextColored(ImVec4(1.f, 0.4f, 0.3f, 1.f), "Texture slot is not available in this model shader.");
					ImGui::InputText("Resource Path", &setting.textures[textureIndex].resourcePath);
					if (ImGui::Button("Remove Slot"))
					{
						setting.textures.erase(setting.textures.begin() + textureIndex);
						ImGui::PopID();
						continue;
					}
					ImGui::PopID();
					++textureIndex;
				}

				auto addSlot = [&](const Char* slotName) {
					if (!std::ranges::any_of(setting.textures, [&](const Engine::MATERIAL_TEXTURE_SETTING& texture) {
						return texture.slotName == slotName;
					}))
						setting.textures.push_back({ slotName, {} });
				};
				if (ImGui::Button("+ Albedo")) addSlot("g_AlbedoMap");
				ImGui::SameLine();
				if (ImGui::Button("+ Normal")) addSlot("g_NormalMap");
				ImGui::SameLine();
				if (ImGui::Button("+ Mask")) addSlot("g_MaskMap");
				ImGui::SameLine();
				if (ImGui::Button("+ Opacity")) addSlot("g_OpacityMap");
			}
			ImGui::PopID();
		}
	}
	ImGui::EndChild();
}

HRESULT ModelViewer::Preview_Model(const Shared<Engine::Model>& model,
	ComPtr<ID3D11ShaderResourceView>& image, int32 meshIndex)
{
	using namespace Engine;
	image.Reset();
	if (!model || meshIndex < -1 || (meshIndex >= 0 && static_cast<uint32>(meshIndex) >= model->Get_NumMeshes()))
		return E_INVALIDARG;
	auto sourceShader = MaterialShader(model);
	if (!sourceShader || FAILED(model->Validate_MaterialBindings(sourceShader))) return E_FAIL;
	auto device = GAME_INSTANCE->Get_Device();
	auto context = GAME_INSTANCE->Get_Context();
	if (!m_PreviewTarget)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = 384; desc.Height = 256; desc.MipLevels = 1; desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		ComPtr<ID3D11Texture2D> color, normal, depth;
		ComPtr<ID3D11RenderTargetView> colorTarget, normalTarget;
		ComPtr<ID3D11ShaderResourceView> colorImage;
		ComPtr<ID3D11DepthStencilView> depthTarget;
		if (FAILED(device->CreateTexture2D(&desc, nullptr, color.GetAddressOf())) ||
			FAILED(device->CreateRenderTargetView(color.Get(), nullptr, colorTarget.GetAddressOf())) ||
			FAILED(device->CreateShaderResourceView(color.Get(), nullptr, colorImage.GetAddressOf())) ||
			FAILED(device->CreateTexture2D(&desc, nullptr, normal.GetAddressOf())) ||
			FAILED(device->CreateRenderTargetView(normal.Get(), nullptr, normalTarget.GetAddressOf()))) return E_FAIL;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		if (FAILED(device->CreateTexture2D(&desc, nullptr, depth.GetAddressOf())) ||
			FAILED(device->CreateDepthStencilView(depth.Get(), nullptr, depthTarget.GetAddressOf()))) return E_FAIL;
		m_PreviewTarget = std::move(colorTarget); m_PreviewNormal = std::move(normalTarget);
		m_PreviewImage = std::move(colorImage); m_PreviewDepth = std::move(depthTarget);
	}
	if (m_PreviewModel.lock() != model)
	{
		vector<Float> positions;
		vector<int32> indices;
		model->Extract_RawMeshData(positions, indices);
		if (positions.size() < 3) return E_FAIL;
		Vector3 minimum(positions[0], positions[1], positions[2]), maximum = minimum;
		for (size_t i = 3; i + 2 < positions.size(); i += 3)
		{
			Vector3 point(positions[i], positions[i + 1], positions[i + 2]);
			minimum = Vector3::Min(minimum, point); maximum = Vector3::Max(maximum, point);
		}
		m_PreviewCenter = (minimum + maximum) * 0.5f;
		m_PreviewRadius = std::max((maximum - minimum).Length() * 0.5f, 0.01f);
		m_PreviewShader = make_shared<Shader>(*sourceShader);
		m_PreviewModel = model;
	}
	ID3D11RenderTargetView* previousTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	ComPtr<ID3D11DepthStencilView> previousDepth;
	context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, previousTargets, previousDepth.GetAddressOf());
	D3D11_VIEWPORT previousViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	context->RSGetViewports(&viewportCount, previousViewports);
	ID3D11RenderTargetView* targets[]{ m_PreviewTarget.Get(), m_PreviewNormal.Get() };
	context->OMSetRenderTargets(2, targets, m_PreviewDepth.Get());
	const Float background[]{ 0.08f, 0.09f, 0.11f, 1.f };
	context->ClearRenderTargetView(m_PreviewTarget.Get(), background);
	context->ClearRenderTargetView(m_PreviewNormal.Get(), background);
	context->ClearDepthStencilView(m_PreviewDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	const D3D11_VIEWPORT viewport{ 0.f, 0.f, 384.f, 256.f, 0.f, 1.f };
	context->RSSetViewports(1, &viewport);
	const Vector3 eye = m_PreviewCenter + Vector3(0.f, m_PreviewRadius * 0.25f, -m_PreviewRadius * 3.f);
	Matrix world = Matrix::Identity;
	Matrix view = XMMatrixLookAtLH(eye, m_PreviewCenter, Vector3::Up);
	Matrix projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.5f, m_PreviewRadius * 0.01f, m_PreviewRadius * 10.f);
	HRESULT result = S_OK;
	if (m_PreviewShader->Supports_CBuffer(ConstantBuffer::Camera))
	{
		CameraCB camera{}; camera.viewMatrix = view; camera.projMatrix = projection;
		camera.cameraPosition = Vector4(eye.x, eye.y, eye.z, 1.f);
		result = m_PreviewShader->Bind_CBufferData(camera);
	}
	else if (FAILED(m_PreviewShader->Bind_Matrix(ViewMatrix, &view)) ||
		FAILED(m_PreviewShader->Bind_Matrix(ProjMatrix, &projection))) result = E_FAIL;
	if (FAILED(m_PreviewShader->Bind_Matrix(WorldMatrix, &world))) result = E_FAIL;
	for (uint32 i = 0; SUCCEEDED(result) && i < model->Get_NumMeshes(); ++i)
	{
		if (meshIndex >= 0 && i != static_cast<uint32>(meshIndex)) continue;
		if ((model->Is_Skeletal() && FAILED(model->Bind_BoneMatrices(m_PreviewShader, BoneMatrices, i))) ||
			FAILED(model->BindAndBeginMaterial(m_PreviewShader, i)) || FAILED(model->Render(i))) result = E_FAIL;
	}
	context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, previousTargets, previousDepth.Get());
	for (auto* target : previousTargets) if (target) target->Release();
	context->RSSetViewports(viewportCount, previousViewports);
	if (SUCCEEDED(result)) image = m_PreviewImage;
	return result;
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
