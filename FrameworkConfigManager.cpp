/**
 * @file      FrameworkConfigManager.cpp
 * @brief     FrameworkConfigManager 类的具体逻辑实现。
 * @details   通过调用 UtilIoJsonWrapper 封装层实现对磁盘 IO 的安全访问。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module FrameworkConfigManager;
import UtilIoJsonWrapper;
import std;
import FrameworkTranslator;

bool FrameworkConfigManager::loadConfig(const std::wstring& filename)
{
    this->mLastError = L"";
    UtilIoJsonWrapper::json j = UtilIoJsonWrapper::openJson(filename, this->mLastError);
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

bool FrameworkConfigManager::saveConfig()
{
    if (this->mConfigFilename.empty())
    {
        this->mLastError = L"Path not set";
        return false;
    }

    try
    {
        UtilIoJsonWrapper::json j = this->mConfig;
        std::wstring saveErr;
        if (!UtilIoJsonWrapper::saveJson(this->mConfigFilename, j, saveErr))
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

std::wstring FrameworkConfigManager::getLastError() const
{
    return this->mLastError;
}

FrameworkConfigManager& FrameworkConfigManager::getInstance()
{
    static FrameworkConfigManager instance;
    return instance;
}

std::wstring FrameworkConfigManager::getConfigFilename()
{
    return this->mConfigFilename;
}

ConfigItems::GlobalConfig& FrameworkConfigManager::getConfig()
{
    return mConfig;
}