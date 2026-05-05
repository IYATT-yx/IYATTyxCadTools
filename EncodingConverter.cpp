/**
 * @file      EncodingConverter.cpp
 * @brief     字符编码转换模块的具体实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include <Windows.h>

export module EncodingConverter;
import std;

export namespace EncodingConverter
{
    std::string FromWstringToUtf8(const std::wstring& wstr)
    {
        if (wstr.empty())
        {
            return std::string();
        }

        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);

        if (utf8Len <= 0)
        {
            return std::string();
        }

        std::string result(static_cast<size_t>(utf8Len) - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result.data(), utf8Len, nullptr, nullptr);

        return result;
    }

    std::wstring FromUtf8ToWstring(const std::string& utf8Str)
    {
        if (utf8Str.empty())
        {
            return std::wstring();
        }

        int wideCharLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);

        if (wideCharLen <= 0)
        {
            return std::wstring();
        }

        std::wstring result(static_cast<size_t>(wideCharLen) - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, result.data(), wideCharLen);

        return result;
    }
};