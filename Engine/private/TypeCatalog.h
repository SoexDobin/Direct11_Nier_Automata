#pragma once

#include "EngineManager.h"

NS_BEGIN(Engine)

class Registry final : public EngineManager {
    NO_COPY(Registry)
public:
    struct Entry final {
        std::string registeredName;
        RuntimeTypeId runtimeTypeId{};
        rttr::type reflectedType;
    };

    Registry() = default;
    ~Registry() override = default;

    HRESULT Initialize(void* arg) override;
    void On_Destroy() override;
    HRESULT Refresh();
    rttr::type Find_Type(std::string_view registeredName) const;
    RuntimeTypeId Find_RuntimeTypeId(std::string_view registeredName) const;
    std::string Find_RegisteredName(RuntimeTypeId runtimeTypeId) const;
    std::vector<Entry> Get_ObjectTypes() const;
    void Clear();

private:
    mutable std::mutex m_Mutex;
    std::unordered_map<std::string, Entry> m_ByRegisteredName;
    std::unordered_map<RuntimeTypeId, std::string> m_RegisteredNameByRuntimeTypeId;
};

NS_END
