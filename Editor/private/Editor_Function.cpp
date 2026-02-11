#include "Editor_Function.h"
#include "Game.h"
#include "pch.h"



NS_BEGIN(Editor)

void Editor_Function::Render_DualView(Shared<Engine::Game> pGame,
                                      bool &bPlayMode) {
  // Scene View Window
  ImGui::Begin("Scene View");
  {
    // Play Mode Toggle
    if (bPlayMode) {
      if (ImGui::Button("Stop"))
        bPlayMode = false;
    } else {
      if (ImGui::Button("Play"))
        bPlayMode = true;
    }
    ImGui::SameLine();
    ImGui::Text(bPlayMode ? "(Playing)" : "(Stopped)");

    auto srv = pGame->Get_OffScreenSRV(L"OffScreenRT_0");
    if (srv) {
      ImVec2 viewportSize = ImGui::GetContentRegionAvail();
      ImGui::Image(reinterpret_cast<ImTextureID>(srv.Get()), viewportSize);
    } else {
      ImGui::Text("Scene View: RenderTarget not available");
    }
  }
  ImGui::End();

  // Game View Window
  ImGui::Begin("Game View");
  {
    auto srv = pGame->Get_OffScreenSRV(L"OffScreenRT_1");
    if (srv) {
      ImVec2 viewportSize = ImGui::GetContentRegionAvail();
      ImGui::Image(reinterpret_cast<ImTextureID>(srv.Get()), viewportSize);
    } else {
      ImGui::Text("Game View: RenderTarget not available");
    }
  }
  ImGui::End();
}

void Editor_Function::Render_ProjectSettings(Shared<Engine::Game> pGame) {
  ImGui::Begin("Project Settings");

  if (ImGui::BeginTabBar("SettingsTabs")) {
    // 1. Layer Settings
    if (ImGui::BeginTabItem("Layers")) {
      static char layerNameBuf[64] = "";

      // List all layers
      auto &layers = pGame->Get_LayerRegister()->Get_AllLayers();
      for (auto &pair : layers) {
        ImGui::Text("Layer %d: %s", pair.first,
                    Helper::To_String(pair.second).c_str());
      }

      ImGui::Separator();
      ImGui::InputText("New Layer Name", layerNameBuf, 64);
      if (ImGui::Button("Add Layer")) {
        pGame->Get_LayerRegister()->Set_LayerName(
            static_cast<uint32>(pGame->Get_LayerRegister()->Get_AllLayers().size()),
            Helper::To_wString(layerNameBuf));
      }

      if (ImGui::Button("Save Layer Settings")) {
        pGame->Get_LayerRegister()->SaveToFile(
            L"../Data/Settings/LayerSettings.json");
      }

      ImGui::EndTabItem();
    }

    // 2. Tag Settings
    if (ImGui::BeginTabItem("Tags")) {
          static char tagNameBuf[64] = "";

          auto &tags = pGame->Get_TagRegister()->Get_AllTags();
          for (auto &pair : tags) {
            ImGui::Text("Tag %d: %s", pair.first,
                        Helper::To_String(pair.second).c_str());
          }

          ImGui::Separator();
          ImGui::InputText("New Tag Name", tagNameBuf, 64);
          if (ImGui::Button("Add Tag")) {
            pGame->Get_TagRegister()->Set_TagName(
                1 << pGame->Get_TagRegister()->Get_AllTags().size(),
                Helper::To_wString(tagNameBuf));
          }

          if (ImGui::Button("Save Tag Settings")) {
            pGame->Get_TagRegister()->SaveToFile(
                L"../Data/Settings/TagSettings.json");
          }

          ImGui::EndTabItem();
        }

    ImGui::EndTabBar();
  }

  ImGui::End();
}

NS_END
