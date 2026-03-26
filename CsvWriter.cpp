module;
#include "stdafx.h"

module CsvWriter;

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
    return std::wstring(s.kACharPtr());
}


bool CsvWriter::isValid() const
{
    return mOfs.is_open() && !mOfs.fail();
}