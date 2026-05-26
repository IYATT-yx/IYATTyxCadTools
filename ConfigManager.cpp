/**
 * @file      ConfigManager.cpp
 * @brief     ConfigManager 类的具体逻辑实现。
 * @details   通过调用 IYATTyxJsonWrapper 封装层实现对磁盘 IO 的安全访问。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module ConfigManager;
import IYATTyxJsonWrapper;
import std;
import Common;
import Translator;

bool ConfigManager::loadConfig(const std::wstring& filename)
{
    this->mLastError = L"";
    IYATTyxJsonWrapper::json j = IYATTyxJsonWrapper::openJson(filename, this->mLastError);
    if (!this->mLastError.empty())
    {
        return false;
    }

    this->mConfigFilename = filename;

    try
    {
        j.get_to(this->mConfig);
    }
    catch (const std::exception& e)
    {
        std::string what = e.what();
        this->mLastError = L"Structure mapping failed: " + std::wstring(what.begin(), what.end());
        return false;
    }

    return this->saveConfig();
}

bool ConfigManager::saveConfig()
{
    if (this->mConfigFilename.empty())
    {
        this->mLastError = L"Path not set";
        return false;
    }

    try
    {
        IYATTyxJsonWrapper::json j = this->mConfig;
        std::wstring saveErr;
        if (!IYATTyxJsonWrapper::saveJson(this->mConfigFilename, j, saveErr))
        {
            this->mLastError = L"Disk persistence failed: " + saveErr;
            return false;
        }
        return true;
    }
    catch (const std::exception& e)
    {
        std::string what = e.what();
        this->mLastError = L"Serialization failed: " + std::wstring(what.begin(), what.end());
        return false;
    }
}

std::wstring ConfigManager::getLastError() const
{
    return this->mLastError;
}

ConfigManager& ConfigManager::getInstance()
{
    static ConfigManager instance;
    return instance;
}

std::wstring ConfigManager::getConfigFilename()
{
    return this->mConfigFilename;
}

ConfigItems::GlobalConfig& ConfigManager::getConfig()
{
    return mConfig;
}