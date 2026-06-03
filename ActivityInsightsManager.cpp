/**
 * @file      ActivityInsightsManager.cpp
 * @brief     CAD活动见解历史档案解析模块的具体业务实现。
 * @details   包含文件共享读、掩码碰撞清洗、目录递归检索逻辑。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include <Windows.h>
#include <ctime>

module ActivityInsightsManager;
import std;
import IYATTyxJsonWrapper;

namespace CadHistory
{
    OpFilter operator|(OpFilter a, OpFilter b)
    {
        return static_cast<OpFilter>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
    }

    bool HasFilter(OpFilter total, OpFilter target)
    {
        return (static_cast<unsigned int>(total) & static_cast<unsigned int>(target)) != 0;
    }

    std::wstring ActivityItem::toTimeString(const std::wstring& wstrFormat) const
    {
        wchar_t wstrTimeBuf[64] = { 0 };
        __time64_t iTime = this->iUnixTime;
        tm tmTime = { 0 };

        // 采用线程级安全 API 自动转换为系统本地时区
        if (_localtime64_s(&tmTime, &iTime) == 0)
        {
            size_t resultSize = wcsftime(wstrTimeBuf, 64, wstrFormat.c_str(), &tmTime);
            if (resultSize > 0)
            {
                return std::wstring(wstrTimeBuf);
            }
        }

        // 容错兜底自愈：如果时区或时间戳严重破损，安全返回原始数字宽字符串
        return std::to_wstring(this->iUnixTime);
    }
}

std::vector<CadHistory::ActivityItem> ActivityInsightsManager::fetchHistory(CadHistory::OpFilter eFilter, bool bUnique, bool bFilterExisting)
{
    std::vector<CadHistory::ActivityItem> vecRawResults;
    std::wstring wstrCommonPath = getCommonFolderPath();

    if (wstrCommonPath.empty() || !std::filesystem::exists(wstrCommonPath))
    {
        return vecRawResults;
    }

    // 1. 递归扫描提取所有数据项
    for (const auto& entry : std::filesystem::recursive_directory_iterator(wstrCommonPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == L".json")
        {
            std::vector<CadHistory::ActivityItem> vecFileItems = parseJsonFileShared(entry.path().wstring(), eFilter);
            vecRawResults.insert(vecRawResults.end(), vecFileItems.begin(), vecFileItems.end());
        }
    }

    // 2. 严格的时间戳由新到旧降序大排队
    std::sort(vecRawResults.begin(), vecRawResults.end(), [](const CadHistory::ActivityItem& lhs, const CadHistory::ActivityItem& rhs)
        {
            return lhs.iUnixTime > rhs.iUnixTime;
        });

    // 3. 执行过滤输出分流
    std::vector<CadHistory::ActivityItem> vecFinalResults;
    std::set<std::wstring> setProcessedPaths;

    for (const auto& item : vecRawResults)
    {
        // 如果开启了物理存在检查，且当前物理文件已不在磁盘上，则直接断链丢弃
        if (bFilterExisting && !std::filesystem::exists(item.wstrDstPath))
        {
            continue;
        }

        // 如果不要求去重，直接装车流出
        if (!bUnique)
        {
            vecFinalResults.push_back(item);
            continue;
        }

        // 路径规整判重逻辑
        std::wstring wstrUpperPath = item.wstrDstPath;
        std::transform(wstrUpperPath.begin(), wstrUpperPath.end(), wstrUpperPath.begin(), ::towupper);

        if (setProcessedPaths.find(wstrUpperPath) == setProcessedPaths.end())
        {
            vecFinalResults.push_back(item);
            setProcessedPaths.insert(wstrUpperPath);
        }
    }

    return vecFinalResults;
}

std::wstring ActivityInsightsManager::getCommonFolderPath()
{
    wchar_t localAppDataBuf[MAX_PATH] = { 0 };
    DWORD dwResult = ::GetEnvironmentVariableW(L"LOCALAPPDATA", localAppDataBuf, MAX_PATH);
    if (dwResult > 0 && dwResult < MAX_PATH)
    {
        return std::wstring(localAppDataBuf) + L"\\Autodesk\\ActivityInsights\\Common";
    }
    return L"";
}

bool ActivityInsightsManager::isOpMatched(const std::wstring& wstrOp, CadHistory::OpFilter eFilter)
{
    if (eFilter == CadHistory::OpFilter::OpAll)
    {
        return true;
    }

    if (wstrOp == L"OPEN" && CadHistory::HasFilter(eFilter, CadHistory::OpFilter::OpOpen))
    {
        return true;
    }
    if ((wstrOp == L"SAVE" || wstrOp == L"UNKNOWN_EDIT") && CadHistory::HasFilter(eFilter, CadHistory::OpFilter::OpSave))
    {
        return true;
    }
    if (wstrOp == L"SAVEAS" && CadHistory::HasFilter(eFilter, CadHistory::OpFilter::OpSaveAs))
    {
        return true;
    }
    if (wstrOp == L"MOVED" && CadHistory::HasFilter(eFilter, CadHistory::OpFilter::OpMoved))
    {
        return true;
    }

    return false;
}

std::vector<CadHistory::ActivityItem> ActivityInsightsManager::parseJsonFileShared(const std::wstring& wstrPath, CadHistory::OpFilter eFilter)
{
    std::vector<CadHistory::ActivityItem> vecParsedItems;

    HANDLE hFile = ::CreateFileW(
        wstrPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return vecParsedItems;
    }

    DWORD dwSize = ::GetFileSize(hFile, nullptr);
    if (dwSize > 0)
    {
        std::string strRawContent;
        strRawContent.resize(dwSize);
        DWORD dwBytesRead = 0;

        if (::ReadFile(hFile, &strRawContent[0], dwSize, &dwBytesRead, nullptr))
        {
            try
            {
                IYATTyxJsonWrapper::json jsonDoc = IYATTyxJsonWrapper::json::parse(strRawContent);
                if (jsonDoc.is_array())
                {
                    for (const auto& jsonNode : jsonDoc)
                    {
                        // 隐式触发由 IYATTyxJsonWrapper 注入到 std 命名空间下的 from_json 管道实现
                        CadHistory::ActivityRecord record = jsonNode.get<CadHistory::ActivityRecord>();

                        if (isOpMatched(record.Op, eFilter))
                        {
                            CadHistory::ActivityItem item;
                            wchar_t* pEnd = nullptr;
                            item.iUnixTime = _wcstoi64(record.Datetime.c_str(), &pEnd, 10);
                            item.wstrDstPath = record.DstPath;
                            item.wstrOp = record.Op;
                            item.wstrProduct = record.Product;
                            item.wstrSrcPath = record.SrcPath;

                            vecParsedItems.push_back(item);
                        }
                    }
                }
            }
            catch (...)
            {
                // 异常自愈隔离：避免破损、未写完的缓冲区破坏外层迭代
            }
        }
    }
    ::CloseHandle(hFile);
    return vecParsedItems;
}