/**
 * @file      IYATTyxJsonWrapper.cpp
 * @brief     nlohmann/json 库的模块化包装实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "resource.h"

module IYATTyxJsonWrapper;
import EncodingConverter;
import Common;

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
                errorMsg = Common::loadString(IDS_ERR_CannotCreateFile);
                return IYATTyxJsonWrapper::json::object();
            }
            newFile << "{}";
            newFile.close();
            file.open(utf8Filename);
        }

        if (!file.is_open())
        {
            errorMsg = Common::loadString(IDS_ERR_ReopenFailed);
            return IYATTyxJsonWrapper::json::object();
        }

        try
        {
            return IYATTyxJsonWrapper::json::parse(file);
        }
        catch (const std::exception& e)
        {
            std::string rawErr = e.what();
            errorMsg = Common::loadString(IDS_ERR_JsonParseEx).GetString() + std::wstring(rawErr.begin(), rawErr.end());
            return IYATTyxJsonWrapper::json::object();
        }
        catch (...)
        {
            errorMsg = Common::loadString(IDS_ERR_UnknownEx);
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
            errorMsg = Common::loadString(IDS_ERR_SaveOpenFailed);
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
            errorMsg = Common::loadString(IDS_ERR_FileWriteEx).GetString() + std::wstring(rawErr.begin(), rawErr.end());
            file.close();
        }
        return false;
    }
};