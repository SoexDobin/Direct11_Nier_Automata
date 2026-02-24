#pragma once
#include "Engine_Define.h"
#include <nlohmann/json.hpp>

NS_BEGIN(Engine)
	class Object;

class ENGINE_DLL JsonSerializer 
{
public:
  static nlohmann::json Serialize(const rttr::variant &var);
  static rttr::variant Deserialize(const nlohmann::json &json,
                                   const rttr::type &type);

  static HRESULT SerializeToFile(const rttr::variant &var,
                                 const wstring &filePath);
  static rttr::variant DeserializeFromFile(const wstring &filePath,
                                           const rttr::type &type);
  static void Inject_Properties(Shared<Object> object,
                                const nlohmann::json &json);

private:
  static nlohmann::json Serialize_Recursive(const rttr::variant &var);
  static rttr::variant Deserialize_Recursive(const nlohmann::json &json,
                                             const rttr::type &type);
};

NS_END
