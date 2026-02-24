#include "JsonSerializer.h"
#include "String_Helper.h"
#include <fstream>
#include <iostream>

NS_BEGIN(Engine)

nlohmann::json JsonSerializer::Serialize(const rttr::variant &var) {
  return Serialize_Recursive(var);
}

rttr::variant JsonSerializer::Deserialize(const nlohmann::json &json,
                                          const rttr::type &type) {
  return Deserialize_Recursive(json, type);
}

HRESULT JsonSerializer::SerializeToFile(const rttr::variant &var,
                                        const wstring &filePath) {
  nlohmann::json j = Serialize(var);
  string path = Helper::To_String(filePath);

  ofstream file(path);
  if (!file.is_open())
    return E_FAIL;

  file << j.dump(4);
  file.close();

  return S_OK;
}

rttr::variant JsonSerializer::DeserializeFromFile(const wstring &filePath,
                                                  const rttr::type &type) {
  string path = Helper::To_String(filePath);
  ifstream file(path);
  if (!file.is_open())
    return rttr::variant();

  nlohmann::json j;
  file >> j;
  file.close();

  return Deserialize(j, type);
}

void JsonSerializer::Inject_Properties(Shared<Object> object,
                                       const nlohmann::json &json) {
  if (!object || json.is_null()) {
    return;
  }

  rttr::variant var = object;
  rttr::type type = rttr::type::get(*object);

  for (auto &prop : type.get_properties()) {
    string propName = prop.get_name().to_string();
    if (json.contains(propName)) {
      rttr::variant val = Deserialize(json[propName], prop.get_type());
      if (val.is_valid()) {
        prop.set_value(var, val);
      }
    }
  }
}

nlohmann::json JsonSerializer::Serialize_Recursive(const rttr::variant &var) {
  rttr::type type = var.get_type();

  // NULL variant
  if (!var.is_valid())
    return nullptr;

  // Arithmetic types (int, float, bool, etc.)
  if (type.is_arithmetic()) {
    if (type == rttr::type::get<bool>())
      return var.get_value<bool>();
    if (type == rttr::type::get<char>())
      return var.get_value<char>();
    if (type == rttr::type::get<int>())
      return var.get_value<int>();
    if (type == rttr::type::get<unsigned int>())
      return var.get_value<unsigned int>();
    if (type == rttr::type::get<long long>())
      return var.get_value<long long>();
    if (type == rttr::type::get<float>())
      return var.get_value<float>();
    if (type == rttr::type::get<double>())
      return var.get_value<double>();
  }

  // String types
  if (type == rttr::type::get<string>())
    return var.get_value<string>();

  if (type == rttr::type::get<wstring>())
    return Helper::To_String(var.get_value<wstring>());

  // Enum types
  if (type.is_enumeration()) {
    rttr::enumeration enumeration = type.get_enumeration();
    return string(enumeration.value_to_name(var));
  }

  // Sequential containers (vector, list, etc.)
  if (type.is_sequential_container()) {
    rttr::variant_sequential_view view = var.get_sequential_view();
    nlohmann::json j = nlohmann::json::array();
    for (const auto &item : view) {
      j.push_back(Serialize_Recursive(item));
    }
    return j;
  }

  // Associative containers (map, unordered_map, etc.)
  if (type.is_associative_container()) {
    // Note: Simplified associative container serialization (assuming string-key
    // map) For complex keys, a more robust approach is needed.
    rttr::variant_associative_view view = var.get_associative_view();
    nlohmann::json j = nlohmann::json::object();
    // nlohmann::json associative container support is tricky with generic RTTR
    // variants For now, let's implement basic object property serialization
  }

  // Class / Struct properties
  if (type.is_class()) {
    nlohmann::json j = nlohmann::json::object();
    for (auto &prop : type.get_properties()) {
      rttr::variant prop_var = prop.get_value(var);
      j[prop.get_name().to_string()] = Serialize_Recursive(prop_var);
    }
    return j;
  }

  return nullptr;
}

rttr::variant JsonSerializer::Deserialize_Recursive(const nlohmann::json &json,
                                                    const rttr::type &type) {
  if (json.is_null())
    return rttr::variant();

  if (type.is_arithmetic()) {
    if (type == rttr::type::get<bool>() && json.is_boolean())
      return json.get<bool>();
    if (type == rttr::type::get<int>() && json.is_number_integer())
      return json.get<int>();
    if (type == rttr::type::get<unsigned int>() && json.is_number_unsigned())
      return json.get<unsigned int>();
    if (type == rttr::type::get<float>() &&
        (json.is_number() || json.is_number_float()))
      return json.get<float>();
    // ... add more as needed
  }

  if (type == rttr::type::get<string>() && json.is_string())
    return json.get<string>();

  if (type == rttr::type::get<wstring>() && json.is_string())
    return Helper::To_wString(json.get<string>());

  if (type.is_enumeration() && json.is_string()) {
    rttr::enumeration enumeration = type.get_enumeration();
    return enumeration.name_to_value(json.get<string>());
  }

  if (type.is_sequential_container() && json.is_array()) {
    // RTTR sequential container deserialization is complex because view is
    // read-only. Usually requires creating the container type first. Simplified
    // version for now.
  }

  if (type.is_class() && json.is_object()) {
    // Create instance
    rttr::variant var = type.create();
    if (!var.is_valid())
      return rttr::variant();

    for (auto &prop : type.get_properties()) {
      string prop_name = prop.get_name().to_string();
      if (json.contains(prop_name)) {
        rttr::variant prop_val =
            Deserialize_Recursive(json[prop_name], prop.get_type());
        if (prop_val.is_valid())
          prop.set_value(var, prop_val);
      }
    }
    return var;
  }

  return rttr::variant();
}

NS_END
