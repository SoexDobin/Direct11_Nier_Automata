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
        return Helper::To_wString(std::format(fmt, std::forward<Args>(args)...));
    }
    
    // --- 3. C++20 format 기반 가변 인자 변환 (String) ---
    // 사실상 std::format의 래퍼지만, 통일성을 위해 제공합니다.
    template <typename... Args>
    static std::string format_s(std::format_string<Args...> fmt, Args&&... args)
	{
        return std::format(fmt, std::forward<Args>(args)...);
    }

    static std::string Clean_RTTR_Name(const std::string& rawName) {
        size_t pos = rawName.rfind("::"); // 가장 마지막에 있는 :: 찾기
        if (pos != std::string::npos) {
            return rawName.substr(pos + 2); // :: 다음 글자부터 끝까지 반환!
        }

        // 만약 "class Terrain" 처럼 남았다면 "class " 지우기
        size_t classPos = rawName.find("class ");
        if (classPos != std::string::npos) {
            return rawName.substr(classPos + 6);
        }
        return rawName;
    }
    static std::wstring Clean_RTTR_Name(const std::wstring& rawName) {
        size_t pos = rawName.rfind(L"::"); // 가장 마지막에 있는 :: 찾기
        if (pos != std::string::npos) {
            return rawName.substr(pos + 2); // :: 다음 글자부터 끝까지 반환!
        }

        // 만약 "class Terrain" 처럼 남았다면 "class " 지우기
        size_t classPos = rawName.find(L"class ");
        if (classPos != std::string::npos) {
            return rawName.substr(classPos + 6);
        }
        return rawName;
    }

    static std::string Trim(const std::string& s) {
        const std::string whitespace = " \t\n\r\f\v";
        size_t start = s.find_first_not_of(whitespace);
        if (start == std::string::npos) return ""; // 공백만 있는 경우
        size_t end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    }

    static std::wstring Trim(const std::wstring& s) {
        const std::wstring whitespace = L" \t\n\r\f\v";
        size_t start = s.find_first_not_of(whitespace);
        if (start == std::wstring::npos) return L"";
        size_t end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    }
NS_END
NS_END