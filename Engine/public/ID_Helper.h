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
	constexpr uint32 OBJECT_ID_UNIQUE	= 0x00000001; 
    constexpr uint32 OBJECT_ID_INSTANCE = 0x00000002;
	constexpr uint32 OBJECT_ID_NONE		= 0x00000003;
	

	static std::atomic<uint32> g_ObjectIDCounter = { 1 };	// 프로토타입 개수 카운터
	static std::atomic<uint32> g_InstanceIDCounter = { 1 };  // 고유 아이디 카운터

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
    
	static uint32 CreateInstanceID()
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
            case OBJECT_ID_UNIQUE:
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.m_objectID = Hash_GUID(guid);
                    g_ObjectIDCounter.fetch_add(1);  // 카운터는 로깅용
                }
                break;
			case OBJECT_ID_INSTANCE:
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.m_instanceID = Hash_GUID(guid);
                    g_InstanceIDCounter.fetch_add(1);  // 카운터는 로깅용
                }
                break;

            case OBJECT_ID_FULL:
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.m_typeID = Hash_GUID(guid);
                    g_ObjectIDCounter.fetch_add(1);
                }

                // uniqueID는 새로운 GUID
                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.m_objectID = Hash_GUID(guid);
                    g_InstanceIDCounter.fetch_add(1);
                }

                if (SUCCEEDED(CoCreateGuid(&guid)))
                {
                    idDesc.m_instanceID = Hash_GUID(guid);
                    g_InstanceIDCounter.fetch_add(1);
                }
                break;
			default: 
        		break;
        }

        
    }

    static uint32 Create_FixedObjectID(const wstring& prototypeTag, const wstring& objectName)
    {
        // 입력값(Tag + Name)이 바뀌지 않는 한, 결과인 ObjectID는 영구히 고정됩니다. (불변성 보장)
        wstring uniqueSeed = prototypeTag + L"_" + objectName;
        return static_cast<uint32>(std::hash<wstring>{}(uniqueSeed));
    }
	NS_END
NS_END
