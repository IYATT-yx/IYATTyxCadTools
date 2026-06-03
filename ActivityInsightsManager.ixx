/**
 * @file      ActivityInsightsManager.ixx
 * @brief     CAD活动见解历史档案解析模块的接口声明。
 * @details   定义核心数据结构、过滤器枚举以及业务处理类。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "nlohmann/json.hpp"

export module ActivityInsightsManager;
import std;

export namespace CadHistory
{
    // 用于接收反序列化映射的全宽字符底层业务结构体
    struct ActivityRecord
    {
        std::wstring Datetime = L"0";
        std::wstring DstPath = L"";
        std::wstring Op = L"";
        std::wstring Product = L"";
        std::wstring SrcPath = L"";

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ActivityRecord, Datetime, DstPath, Op, Product, SrcPath)
    };

    // 格式化后的强类型输出结构体，供上层业务调用
    struct ActivityItem
    {
        __time64_t iUnixTime = 0;
        std::wstring wstrDstPath = L"";
        std::wstring wstrOp = L"";
        std::wstring wstrProduct = L"";
        std::wstring wstrSrcPath = L"";

        /**
        * @brief 将内置的 Unix 时间戳安全转换为本地时区的格式化宽字符串。
        * @param wstrFormat 期望的时间格式化控制符，默认为国标标准 L"%Y-%m-%d %H:%M:%S"
        * @return 格式化后的时间字符串。若转换失败则返回原始时间戳文本。
        */
        std::wstring toTimeString(const std::wstring& wstrFormat = L"%Y-%m-%d %H:%M:%S") const;
    };

    // 操作符过滤位域掩码枚举
    enum class OpFilter : unsigned int
    {
        OpNone = 0,
        OpOpen = 1 << 0,   // "OPEN"
        OpSave = 1 << 1,   // "SAVE" 或 "UNKNOWN_EDIT"
        OpSaveAs = 1 << 2,   // "SAVEAS"
        OpMoved = 1 << 3,   // "MOVED"
        OpAll = 0xFFFFFFFF
    };

    // 位运算支持函数声明
    export OpFilter operator|(OpFilter a, OpFilter b);
    export bool HasFilter(OpFilter total, OpFilter target);
}

export class ActivityInsightsManager
{
public:
    ActivityInsightsManager() = default;
    ~ActivityInsightsManager() = default;

    /**
         * @brief 动态抓取并解析通用路径下匹配对应过滤条件的图纸活动痕迹。
         * @param eFilter 允许读取的操作符组合类型掩码。
         * @param bUnique 是否开启路径去重。若为 true，则同一张图纸仅保留最新的一次操作痕迹。
         * @param bFilterExisting 是否过滤掉物理磁盘上当前已不存在的图纸文件（死链）。
         * @return 过滤后且完全排好序的 ActivityItem 集合。
         */
    std::vector<CadHistory::ActivityItem> fetchHistory(CadHistory::OpFilter eFilter = CadHistory::OpFilter::OpAll, bool bUnique = true, bool bFilterExisting = true);

private:
    /**
     * @brief 提取脱离特定用户绑定的全局系统通用环境变量路径。
     */
    std::wstring getCommonFolderPath();

    /**
     * @brief 基于掩码校验宽字符格式的操作符是否匹配。
     */
    bool isOpMatched(const std::wstring& wstrOp, CadHistory::OpFilter eFilter);

    /**
     * @brief 采用 Windows 共享非独占锁安全提取物理文件，完美避开运行期文件句柄冲突。
     */
    std::vector<CadHistory::ActivityItem> parseJsonFileShared(const std::wstring& wstrPath, CadHistory::OpFilter eFilter);
};