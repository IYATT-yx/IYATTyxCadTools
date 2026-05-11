/**
 * @file      CsvModule.cpp
 * @brief     CSV 读写模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"

module CsvModule;
import std;

CsvWriter::CsvWriter(const CString& filePath, wchar_t delimiter)
    : mDelimiter(delimiter)
{
    open(filePath);
}

CsvWriter::~CsvWriter()
{
    if (mOfs.is_open())
    {
        mOfs.close();
    }
}

void CsvWriter::open(const CString& filePath)
{
    // 追加模式打开
    this->mOfs.open(std::wstring(filePath.GetString()), std::ios::binary | std::ios::app);

    // 如果是新建的文件，写入 BOM
    if (this->mOfs.is_open())
    {
        this->mOfs.seekp(0, std::ios::end);
        if (this->mOfs.tellp() == 0)
        {
            const unsigned char bom[] = {0xEF, 0xBB, 0xBF};
            this->mOfs.write(reinterpret_cast<const char*>(bom), sizeof(bom));
        }
    }
}

void CsvWriter::writeRow(const std::vector<AcString>& fields)
{
    std::vector<std::wstring> wfields;

    for (const auto& f : fields)
    {
        wfields.push_back(toWString(f));
    }

    writeFields(wfields);
}

void CsvWriter::writeFields(const std::vector<std::wstring>& fields)
{
    std::wstring line;
    bool first = true;

    for (const auto& f : fields)
    {
        if (!first)
        {
            line += mDelimiter;
        }

        line += escape(f);
        first = false;
    }

    line += L"\r\n";
    writeUtf8(line);
}

void CsvWriter::writeUtf8(const std::wstring& ws)
{
    int size = WideCharToMultiByte(
        CP_UTF8, 0,
        ws.c_str(), -1,
        nullptr, 0,
        nullptr, nullptr
    );

    if (size <= 0)
    {
        return;
    }

    std::string utf8(size - 1, 0); // 去掉末尾 NUL

    WideCharToMultiByte(
        CP_UTF8, 0,
        ws.c_str(), -1,
        utf8.data(), size,
        nullptr, nullptr
    );

    mOfs.write(utf8.c_str(), utf8.size());
}

std::wstring CsvWriter::escape(const std::wstring& field)
{
    if (field.find(mDelimiter) != std::wstring::npos ||
        field.find(L'"') != std::wstring::npos ||
        field.find(L'\n') != std::wstring::npos)
    {
        std::wstring result = L"\"";

        for (wchar_t ch : field)
        {
            if (ch == L'"')
            {
                result += L"\"\"";
            }
            else
            {
                result += ch;
            }
        }

        result += L"\"";
        return result;
    }

    return field;
}

std::wstring CsvWriter::toWString(const AcString& s) const
{
    return std::wstring(s.constPtr());
}


bool CsvWriter::isValid() const
{
    return mOfs.is_open() && !mOfs.fail();
}

namespace CsvModule
{
    bool readCsvToAcStringMatrix(const CString& strFilePath, AcStringMatrix& matrixData, char delimiter)
    {
        matrixData.clear();

        std::wstring wPath = strFilePath.GetString();

        // 以二进制模式读取，确保能手动识别并处理 UTF-8 的 BOM 头
        std::ifstream file(wPath, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            return false;
        }

        // 检查并跳过 UTF-8 文件的 BOM 签名（0xEF, 0xBB, 0xBF）
        unsigned char bom[3] = { 0 };
        file.read((char*)bom, 3);
        if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF))
        {
            // 若无 BOM，则重置文件指针到起始位置
            file.seekg(0, std::ios::beg);
        }

        std::string line;
        while (std::getline(file, line))
        {
            // 去除行尾的回车符（兼容 Windows 的 \r\n 格式）
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            std::vector<AcString> row;
            std::string currentCell;
            bool inQuotes = false;

            // 遍历行字符，处理 CSV 的引号转义与字段分割逻辑
            for (size_t i = 0; i < line.length(); ++i)
            {
                char c = line[i];

                if (c == '"')
                {
                    if (inQuotes && i + 1 < line.length() && line[i + 1] == '"')
                    {
                        // 处理转义的双引号 "" -> "
                        currentCell += '"';
                        i++;
                    }
                    else
                    {
                        // 切换是否处于引号包围状态
                        inQuotes = !inQuotes;
                    }
                }
                else if (c == delimiter && !inQuotes)
                {
                    // 分隔符不在引号内，完成当前单元格提取
                    // 使用 AcString::Utf8 标志确保 UTF-8 字符串正确转为 Unicode
                    row.push_back(AcString(currentCell.c_str(), AcString::Utf8));
                    currentCell.clear();
                }
                else
                {
                    currentCell += c;
                }
            }

            // 压入每行的最后一个字段
            row.push_back(AcString(currentCell.c_str(), AcString::Utf8));
            matrixData.push_back(row);
        }

        file.close();
        return true;
    }
};