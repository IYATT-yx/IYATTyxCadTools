/**
 * @file      IYATTyxJsonWrapper.cpp
 * @brief     nlohmann/json 库的模块化包装实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module IYATTyxJsonWrapper;
import EncodingConverter;
import Common;
import Translator;

namespace IYATTyxJsonWrapper
{
    IYATTyxJsonWrapper::json openJson(const std::wstring& filename, std::wstring& errorMsg)
    {
        errorMsg = L"";
        std::string utf8Filename = EncodingConverter::FromWstringToUtf8(filename);
        std::ifstream file(utf8Filename);
        if (!file.is_open())
        {
            std::ofstream newFile(utf8Filename);
            if (!newFile.is_open())
            {
                errorMsg = _(L"无法创建或打开文件，请检查目录权限。");
                return IYATTyxJsonWrapper::json::object();
            }
            newFile << "{}";
            newFile.close();
            file.open(utf8Filename);
        }

        if (!file.is_open())
        {
            errorMsg = _(L"文件创建后重开失败。");
            return IYATTyxJsonWrapper::json::object();
        }

        try
        {
            return IYATTyxJsonWrapper::json::parse(file);
        }
        catch (const std::exception& e)
        {
            std::string rawErr = e.what();
            errorMsg = _(L"JSON 解析异常:") + std::wstring(rawErr.begin(), rawErr.end());
            return IYATTyxJsonWrapper::json::object();
        }
        catch (...)
        {
            errorMsg = _(L"发生未知解析异常。");
            return IYATTyxJsonWrapper::json::object();
        }
    }

    bool saveJson(const std::wstring& filename, const IYATTyxJsonWrapper::json& json, std::wstring& errorMsg)
    {
        errorMsg = L"";
        std::string utf8Filename = EncodingConverter::FromWstringToUtf8(filename);
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