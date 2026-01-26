#pragma once
#include <atomic>
#include <combaseapi.h> // CoCreateGuid
#include <functional>   // std::hash

NS_BEGIN(Engine)
	NS_BEGIN(Helper)

static std::atomic<uint32> g_IDCounter = {1};

static uint32 Hash_GUID(const GUID& guid) {
    std::hash<uint32> hasher;
    uint32 hash = 0;

    hash ^= hasher(guid.Data1);
    hash ^= hasher(static_cast<uint32>(guid.Data2) << 16 | guid.Data3);

    for (int i = 0; i < 8; ++i)
    hash ^= hasher(guid.Data4[i] << (i % 4 * 8));

    return hash;
}

static inline uint32 Create_InstanceID() {
    GUID guid = {};
    if (FAILED(CoCreateGuid(&guid))) 
    {
		MSG_BOX("Failed To Create GUID At ID_Helper.h");
		return g_IDCounter.fetch_add(1, std::memory_order_relaxed);
    }

    uint32 guidHash = Hash_GUID(guid);
    uint32 counter = g_IDCounter.fetch_add(1, std::memory_order_relaxed);
    uint32 uniqueID = guidHash ^ counter;

    if (uniqueID == 0) 
    {
        MSG_BOX("Unique ID Is 0 At ID_Helper.h");
        uniqueID = g_IDCounter;
    }

    return uniqueID;
}

#ifdef _DEBUG
static inline void Reset_IDCounter() {
    g_IDCounter.store(1, std::memory_order_relaxed);
}
#endif


	NS_END
NS_END