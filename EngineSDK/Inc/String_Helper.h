#pragma once
#include "Engine_Define.h"
#include <format>


NS_BEGIN(Engine)
	NS_BEGIN(Helper)
	
    // --- Concepts ---
    template <typename T>
    concept formattableW = requires(T format) { std::format(L"{}", format); };
    template <typename T>
    concept formattable = requires(T format) { std::format("{}", format); };
    
    template <typename T>
    concept narrowString = std::convertible_to<T, std::string_view>;
    template <typename T>
    concept wideString = std::convertible_to<T, std::wstring_view>;

    /** @brief multibyte types to wString */
    template <formattableW T>
    std::wstring ToWString(const T& value) {
        return std::format(L"{}", value);
    }
    
    /** @brief char types to wString */
    inline std::wstring ToWString(narrowString auto const& value) {
        std::string_view stringView(value);
        if (stringView.empty()) return L"";
    
        uint32 sizeNeeded = MultiByteToWideChar(
            CP_UTF8, 
            0, 
            stringView.data(), 
            static_cast<uint32>(stringView.size()),
            nullptr, 
            0
        );

        std::wstring result(sizeNeeded, 0);

        MultiByteToWideChar(
            CP_UTF8,
            0, 
            stringView.data(), 
            static_cast<uint32>(stringView.size()), 
            result.data(), 
            sizeNeeded
        );

        return result;
    }
    
    inline std::wstring ToWString(Bool value) {
        return value ? L"true" : L"false";
    }
    
    /** @brief multibyte types to String */
    template <formattable T>
    std::string ToString(const T& value) {
        return std::format("{}", value);
    }
    
    /** @brief wchar_t types to String */
    inline std::string ToString(wideString auto const& value) {
        std::wstring_view wStringView(value);
        if (wStringView.empty()) return "";
    
        uint32 sizeNeeded = WideCharToMultiByte(
            CP_UTF8, 
            0, 
            wStringView.data(), 
            static_cast<uint32>(wStringView.size()), 
            nullptr,
            0, 
            nullptr,
            nullptr
        );

        std::string result(sizeNeeded, 0);

        WideCharToMultiByte(
            CP_UTF8,
            0, 
            wStringView.data(),
            static_cast<uint32>(wStringView.size()), 
            result.data(),
            sizeNeeded,
            nullptr, 
            nullptr
        );

        return result;
    }
    
    inline std::string ToString(Bool value) {
        return value ? "true" : "false";
    }

	NS_END
NS_END