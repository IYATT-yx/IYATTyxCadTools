/**
 * @file      UtilIoJsonWrapper.cpp
 * @brief     nlohmann/json 库的模块化包装实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module UtilIoJsonWrapper;
import UtilIoEncodingConverter;
import FrameworkTranslator;

namespace UtilIoJsonWrapper
{
    UtilIoJsonWrapper::json openJson(const std::wstring& filename, std::wstring& errorMsg)
    {
        errorMsg = L"";
        std::string utf8Filename = UtilIoEncodingConverter::FromWstringToUtf8(filename);
        std::ifstream file(utf8Filename);
        if (!file.is_open())
        {
            std::ofstream newFile(utf8Filename);
            if (!newFile.is_open())
            {
                errorMsg = _(L"无法创建或打开文件，请检查目录权限。");
                return UtilIoJsonWrapper::json::object();
            }
            newFile << "{}";
            newFile.close();
            file.open(utf8Filename);
        }

        if (!file.is_open())
        {
            errorMsg = _(L"文件创建后重开失败。");
            return UtilIoJsonWrapper::json::object();
        }

        try
        {
            return UtilIoJsonWrapper::json::parse(file);
        }
        catch (const std::exception& e)
        {
            std::string rawErr = e.what();
            errorMsg = _(L"JSON 解析异常:") + std::wstring(rawErr.begin(), rawErr.end());
            return UtilIoJsonWrapper::json::object();
        }
        catch (...)
        {
            errorMsg = _(L"发生未知解析异常。");
            return UtilIoJsonWrapper::json::object();
        }
    }

    bool saveJson(const std::wstring& filename, const UtilIoJsonWrapper::json& json, std::wstring& errorMsg)
    {
        errorMsg = L"";
        std::string utf8Filename = UtilIoEncodingConverter::FromWstringToUtf8(filename);
        std::ofstream file(utf8Filename);

        if (!file.is_open())
        {
            errorMsg = _(L"保存时无法打开写入流。");
            return false;
        }

        try
        {
            file << json.dump(4);
            file.close();
            return true;
        }
        catch (const std::exception& e)
        {
            std::string rawErr = e.what();
            errorMsg = _(L"保存时无法打开写入流。") + std::wstring(rawErr.begin(), rawErr.end());
            file.close();
        }
        return false;
    }
};