#pragma once
#include "Engine_Define.h"
#include <atomic>
#include <combaseapi.h> // CoCreateGuid
#include <functional>   // std::hash

NS_BEGIN(Engine)
NS_BEGIN(Helper)

	using	OBJECT_ID_FLAG	=	unsigned __int32;
	constexpr uint32 OBJECT_ID_NONE = 0x00000000;
	constexpr uint32 OBJECT_ID_TYPE = 0xFFFF0000; // 상위 16비트 (Type)
	constexpr uint32 OBJECT_ID_UNIQUE = 0x0000FFFF; // 하위 16비트 (Instance)
	constexpr uint32 OBJECT_ID_FULL = 0xFFFFFFFF;

	static std::atomic<uint32> g_IDCounter = { 1 }; // 프로토타입 개수 카운터

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

	static uint32 Get_ID(OBJECT_ID_FLAG requestFlag, const uint32& memberHex)
	{
		switch (requestFlag)
		{
		case OBJECT_ID_UNIQUE:
			return (memberHex & 0x0000FFFF);
		case OBJECT_ID_TYPE:
			return (memberHex & 0xFFFF0000);
		case OBJECT_ID_FULL:
			return memberHex;
		default:
			break;
		}

		MSG_BOX("Failed To Get ObjectID");
		return OBJECT_ID_NONE;
	}

	// 0000 0000 2진수 id 계산
	// 상위 16비트는 prototype id : 중복 체크용
	// 하위 16비트는 unique id : 고유 id
	// flag는 적용시키고 싶은 비트 마스크를 주면된다.
	static void CreateID(OBJECT_ID_FLAG requestFlag, uint32& memberHex)
	{
		GUID guid = {};
		if (memberHex == OBJECT_ID_NONE)
		if (FAILED(CoCreateGuid(&guid)))
		{
			MSG_BOX("Failed To Create GUID");
			return;
		}
		
		uint32 id = Hash_GUID(guid);

		switch (requestFlag)
		{
		case OBJECT_ID_UNIQUE:
			{
				g_IDCounter.fetch_add(1);
			}
			break;
		case OBJECT_ID_TYPE:
			{
				memberHex = (id & 0x0000FFFF) | memberHex;
				memberHex = (id & 0x0000FFFF) << 16;
			}
			break;
		case OBJECT_ID_FULL:
			memberHex = id;
			break;
		default: 
			MSG_BOX("Failed To Create ObjectID");
			return;
		}
	}

	NS_END
NS_END
