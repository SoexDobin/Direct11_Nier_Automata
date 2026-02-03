#pragma once
#include "Engine_Define.h"
#include <atomic>
#include <combaseapi.h> // CoCreateGuid
#include <functional>   // std::hash

NS_BEGIN(Engine)
NS_BEGIN(Helper)

	using	ObjectType = unsigned __int32;
	using	ObjectUnique = unsigned __int32;
	using	OBJECT_ID_FLAG	=	unsigned __int32;
	constexpr uint32 OBJECT_ID_FULL		= 0x00000000;
	constexpr uint32 OBJECT_ID_TYPE		= 0x00000001;
	constexpr uint32 OBJECT_ID_UNIQUE	= 0x00000002; 
	constexpr uint32 OBJECT_ID_NONE		= 0x00000003;
	

	static std::atomic<uint32> g_TypeIDCounter = { 1 };		// 프로토타입 개수 카운터
	static std::atomic<uint32> g_UniqueIDCounter = { 1 }; // 고유 아이디 카운터

    static uint32 Hash_GUID(const GUID& guid)
    {
        std::hash<uint32> hasher = {};
        uint32 hash = {};

        hash ^= hasher(guid.Data1);
        hash ^= hasher(static_cast<uint32>(guid.Data2) << 16 | guid.Data3);
        for (int i = 0; i < 8; ++i)
            hash ^= hasher(guid.Data4[i] << (i % 4 * 8));

        return hash;
    }
    
	static uint32 CreateID()
    {
        GUID guid = {};
        if (SUCCEEDED(CoCreateGuid(&guid)))
            return Hash_GUID(guid);
	    
		return 0;
    }

    static void CreateID(OBJECT_ID_FLAG requestFlag, ID_DESC& idDesc)
    {
        GUID guid = {};

        if (requestFlag >= OBJECT_ID_NONE)
        {
            MSG_BOX("Out Of ID_FLAG");
            return;
        }
			

        switch (requestFlag)
        {
            case OBJECT_ID_TYPE:
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.typeID = Hash_GUID(guid);
                    idDesc.uniqueID = 0;
                    g_TypeIDCounter.fetch_add(1);  // 카운터는 로깅용
                }
                break;

            case OBJECT_ID_UNIQUE:
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.uniqueID = Hash_GUID(guid);
                    g_UniqueIDCounter.fetch_add(1);  // 카운터는 로깅용
                }
                break;

            case OBJECT_ID_FULL:
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.typeID = Hash_GUID(guid);
                    g_TypeIDCounter.fetch_add(1);
                }

                // uniqueID는 새로운 GUID
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.uniqueID = Hash_GUID(guid);
                    g_UniqueIDCounter.fetch_add(1);
                }
                break;
			default: 
        		break;
        }
    }

	NS_END
NS_END
