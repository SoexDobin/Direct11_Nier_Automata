#include "TypeCatalog.h"
#include "Object.h"
#include "SpdLogger.h"
#include "String_Helper.h"

HRESULT Engine::Registry::Initialize(void* arg) {
    return Refresh();
}

void Engine::Registry::On_Destroy() {
    Clear();
}

HRESULT Engine::Registry::Refresh() {
    std::unordered_map<std::string, Entry> byRegisteredName;
    std::unordered_map<RuntimeTypeId, std::string> registeredNameByRuntimeTypeId;

    for (const rttr::type reflectedType : rttr::type::get_types()) {
        const std::string registeredName = reflectedType.get_name().to_string();
        const auto rawTypeId = reflectedType.get_id();
        if (registeredName.empty() || rawTypeId == 0) {
            return E_FAIL;
        }

        const RuntimeTypeId runtimeTypeId = static_cast<RuntimeTypeId>(rawTypeId);
        if (!byRegisteredName.emplace(registeredName,
                Entry{registeredName, runtimeTypeId, reflectedType}).second) {
            return E_FAIL;
        }
        if (!registeredNameByRuntimeTypeId.emplace(runtimeTypeId, registeredName).second) {
            return E_FAIL;
        }

        std::unordered_set<std::string> propertyNames;
        for (const rttr::property property : reflectedType.get_properties()) {
            const std::string propertyName = property.get_name().to_string();
            if (propertyName.empty() || !propertyNames.emplace(propertyName).second) {
                LOG_ERROR(L"Registry rejected duplicate or empty property name on {}",
                          Helper::To_wString(registeredName));
                return E_FAIL;
            }
        }
    }

    const rttr::type objectType = rttr::type::get<Object>();
    for (const auto& [registeredName, entry] : byRegisteredName) {
        if (entry.reflectedType == objectType ||
            !entry.reflectedType.is_derived_from(objectType) ||
            !entry.reflectedType.get_constructor().is_valid()) {
            continue;
        }

        if (!entry.reflectedType.get_method("Create").is_valid() &&
            !entry.reflectedType.get_method("CreatePrototype").is_valid()) {
            LOG_ERROR(L"Registry rejected concrete Object type without a factory: {}",
                      Helper::To_wString(registeredName));
            return E_FAIL;
        }
    }

    std::lock_guard lock(m_Mutex);
    m_ByRegisteredName = std::move(byRegisteredName);
    m_RegisteredNameByRuntimeTypeId = std::move(registeredNameByRuntimeTypeId);
    return S_OK;
}

rttr::type Engine::Registry::Find_Type(std::string_view registeredName) const {
    std::lock_guard lock(m_Mutex);
    const auto it = m_ByRegisteredName.find(std::string{registeredName});
    return it == m_ByRegisteredName.end()
        ? rttr::type::get_by_name("")
        : it->second.reflectedType;
}

Engine::RuntimeTypeId Engine::Registry::Find_RuntimeTypeId(std::string_view registeredName) const {
    std::lock_guard lock(m_Mutex);
    const auto it = m_ByRegisteredName.find(std::string{registeredName});
    return it == m_ByRegisteredName.end() ? RuntimeTypeId{} : it->second.runtimeTypeId;
}

std::string Engine::Registry::Find_RegisteredName(RuntimeTypeId runtimeTypeId) const {
    std::lock_guard lock(m_Mutex);
    const auto it = m_RegisteredNameByRuntimeTypeId.find(runtimeTypeId);
    return it == m_RegisteredNameByRuntimeTypeId.end() ? std::string{} : it->second;
}

std::vector<Engine::Registry::Entry> Engine::Registry::Get_ObjectTypes() const {
    std::lock_guard lock(m_Mutex);
    std::vector<Entry> result;
    const rttr::type objectType = rttr::type::get<Object>();
    for (const auto& [registeredName, entry] : m_ByRegisteredName) {
        if (entry.reflectedType != objectType && entry.reflectedType.is_derived_from(objectType))
            result.push_back(entry);
    }
    std::ranges::sort(result, {}, &Entry::registeredName);
    return result;
}

void Engine::Registry::Clear() {
    std::lock_guard lock(m_Mutex);
    m_RegisteredNameByRuntimeTypeId.clear();
    m_ByRegisteredName.clear();
}
