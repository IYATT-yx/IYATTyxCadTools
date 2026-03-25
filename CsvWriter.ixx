module;
#include "StdAfx.h"
#include <fstream>

export module CsvWriter;

/**
 * @brief CSV 文件写入类
 *
 * 只接受 AcString 类型字段输入，
 * 输出 UTF-8 文件（无 BOM），可指定字段分隔符。
 */
export class CsvWriter
{
public:

    /**
     * @brief 构造函数，打开 CSV 文件
     * @param filePath CSV 文件路径（CString 类型）
     * @param delimiter 字段分隔符，默认为逗号 ','
     */
    explicit CsvWriter(const CString& filePath, wchar_t delimiter = L',');

    /**
     * @brief 析构函数，关闭文件流
     */
    ~CsvWriter();

    /**
     * @brief 写入一行 CSV（AcString 版本）
     * @param fields AcString 类型字段向量
     *
     * 内部会调用 escape 对字段进行转义
     */
    void writeRow(const std::vector<AcString>& fields);

    /**
     * @brief 验证文件是否有效打开
     * @return true 文件有效打开， false 文件未打开
     */
    bool CsvWriter::isValid() const;

private:
    std::ofstream mOfs;   /**< 文件输出流 */
    wchar_t mDelimiter;   /**< 字段分隔符 */

    /**
     * @brief 打开 CSV 文件
     * @param filePath 文件路径
     *
     * 使用二进制模式打开，UTF-8 文件，无 BOM
     */
    void open(const CString& filePath);

    /**
     * @brief 写入一行字段到文件
     * @param fields 宽字符串向量，每个元素为一个字段
     */
    void writeFields(const std::vector<std::wstring>& fields);

    /**
     * @brief 将宽字符串写入文件（UTF-8）
     * @param ws 宽字符串
     *
     * 不写 BOM，使用 WideCharToMultiByte 转换
     */
    void writeUtf8(const std::wstring& ws);

    /**
     * @brief 转义单个 CSV 字段
     * @param field 原始字段
     * @return 转义后的字段
     *
     * 转义规则：
     * - 包含分隔符、双引号或换行的字段，用双引号括起来
     * - 双引号内部重复一次
     */
    std::wstring escape(const std::wstring& field);

    /**
     * @brief 将 AcString 转换为 std::wstring
     * @param s AcString 对象
     * @return 转换后的宽字符串
     */
    std::wstring toWString(const AcString& s) const;
};
