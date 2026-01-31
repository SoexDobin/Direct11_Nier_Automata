#pragma once
#include "Engine_Define.h"


NS_BEGIN(Engine)
NS_BEGIN(Helper)
	static std::wstring To_wString(std::string_view string)
	{
        if (string.empty()) return L"";

        int32 size = MultiByteToWideChar(
            CP_UTF8, 
            0, 
            string.data(),
            static_cast<int32>(string.size()),
            nullptr, 
            0
        );

        std::wstring wString(size, 0);

        MultiByteToWideChar(
            CP_UTF8, 
            0, 
            string.data(), 
            static_cast<int32>(string.size()), 
            wString.data(),
            size
        );

        return wString;
    }
    
	static std::string To_String(std::wstring_view wString)
	{
        if (wString.empty()) return "";

        int32 size = WideCharToMultiByte(
            CP_UTF8, 
            0, 
            wString.data(), 
            static_cast<int32>(wString.size()),
            nullptr, 
            0, 
            nullptr, 
            nullptr
        );

        std::string string(size, 0);

        WideCharToMultiByte(
            CP_UTF8, 
            0, 
            wString.data(), 
            static_cast<int32>(wString.size()), 
            string.data(),
            size, 
            nullptr, 
            nullptr
        );

        return string;
    }
    
    // --- 2. C++20 format 기반 가변 인자 변환 (Wide) ---
    template <typename... Args>
    static std::wstring format_w(std::format_string<Args...> fmt, Args&&... args) 
	{
        // 1. 일반 string으로 포맷팅 후 2. wstring으로 변환
		// format_w("Assets/Shaders/{}_{}.hlsl", "Effect", 1);
        return To_wString(std::format(fmt, std::forward<Args>(args)...));
    }
    
    // --- 3. C++20 format 기반 가변 인자 변환 (String) ---
    // 사실상 std::format의 래퍼지만, 통일성을 위해 제공합니다.
    template <typename... Args>
    static std::string format_s(std::format_string<Args...> fmt, Args&&... args)
	{
        return std::format(fmt, std::forward<Args>(args)...);
    }
NS_END
NS_END