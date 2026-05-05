/**
 * @file      EncodingConverter.ixx
 * @brief     字符编码转换模块接口声明
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module EncodingConverter;
import std;

namespace EncodingConverter
{
   /**
    * @brief 将宽字符串转换为 UTF-8 窄字符串。
    * @param wstr 输入的宽字符串。
    * @return std::string 转换后的 UTF-8 字节流。
    */
    std::string FromWstringToUtf8(const std::wstring& wstr);

   /**
    * @brief 将 UTF-8 窄字符串转换为宽字符串。
    * @param utf8Str 输入的 UTF-8 字节流。
    * @return std::wstring 转换后的宽字符串。
    */
    std::wstring FromUtf8ToWstring(const std::string& utf8Str);
};