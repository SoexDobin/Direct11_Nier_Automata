#include "LayerRegistry.h"
#include "Engine_Define.h"
#include "String_Helper.h"
#include <fstream>
#include <nlohmann/json.hpp>

#include "SpdLogger.h"


wstring LayerRegistry::Get_LayerName(LAYER layer) const {
  const auto iter = m_LayerToName.find(layer);

  if (iter != m_LayerToName.end())
    return iter->second;

  return Helper::To_wString(magic_enum::enum_name(layer));
}

wstring LayerRegistry::Get_LayerName(uint32 layerBit) const {
  return Get_LayerName(static_cast<LAYER>(layerBit));
}

LAYER LayerRegistry::Get_LayerByName(const wstring &name) const {
  auto it = m_NameToLayer.find(name);
  if (it != m_NameToLayer.end())
    return it->second;

  return LAYER::LAYER0;
}

void LayerRegistry::Set_LayerName(LAYER layer, const wstring &name) {
  auto oldName = m_LayerToName.find(layer);
  if (oldName != m_LayerToName.end())
    m_NameToLayer.erase(oldName->second);

  m_LayerToName[layer] = name;
  m_NameToLayer[name] = layer;
}

void LayerRegistry::Set_LayerName(uint32 layerBit, const wstring &name) {
  Set_LayerName(static_cast<LAYER>(layerBit), name);
}

void LayerRegistry::LoadFromFile(const wstring &path) {
  Clear();

  LoadDefaults();

  std::ifstream file(path);
  if (!file.is_open()) {
    return;
  }

  try {
    nlohmann::json jsonData;
    file >> jsonData;
    file.close();

    if (!jsonData.contains("version") || jsonData["version"] != "1.0") {
      return;
    }

    if (jsonData.contains("layers")) {
      for (const auto &layerJson : jsonData["layers"]) {
        uint32 bit = layerJson["bit"];
        std::string nameStr = layerJson["name"];

        if (nameStr.empty())
          continue;

        wstring name = Helper::To_wString(nameStr);
        LAYER layer = static_cast<LAYER>(bit);

        Set_LayerName(layer, name);
      }
    }
  } catch (const std::exception &e) {
  }
}

void LayerRegistry::SaveToFile(const wstring &path) const {
    nlohmann::json jsonData;
    jsonData["version"] = "1.0";

    auto &layersArray = jsonData["layers"];

    for (const auto &[layer, name] : m_LayerToName) {
        nlohmann::json layerJson;
        layerJson["bit"] = static_cast<uint32>(layer);
        layerJson["name"] = Helper::To_String(name);
        layersArray.push_back(layerJson);
    }

    std::filesystem::path filePath(path);
    std::ofstream file(filePath);        
    if (!file.is_open())
    {
        LOG_CRITICAL(L"Failed To Save Layers");
        return;
    }
		

    file << jsonData.dump(2);
    file.close();
}

void LayerRegistry::LoadDefaults() {
    m_NameToLayer[Helper::To_wString(magic_enum::enum_name(LAYER::LAYER0))] =
        LAYER::LAYER0;
    m_LayerToName[LAYER::LAYER0] =
        Helper::To_wString(magic_enum::enum_name(LAYER::LAYER0));

    for (size_t i = 0; i < ETOI(LAYER::END); ++i) {
        LAYER layer = static_cast<LAYER>(1 << i);
        wstring defaultName = Helper::To_wString(magic_enum::enum_name(layer));

        if (defaultName.empty())
          defaultName = L"LAYER" + std::to_wstring(i);

        m_NameToLayer.emplace(defaultName, layer);
        m_LayerToName.emplace(layer, defaultName);
    }
}

void LayerRegistry::Clear() {
  m_NameToLayer.clear();
  m_LayerToName.clear();
}