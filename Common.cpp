module;
#include "StdAfx.h"

module Common;

namespace Common
{
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision)
	{
		//AcStringValue.format(L"%.*g", precision, doubleValue);
		acdbRToS(doubleValue, AcStringValue, Common::UnitMode::Decimal, precision);
	}

	bool setCharMapFontToGDT()
	{
		HKEY key = nullptr;

		LSTATUS status = RegOpenKeyExW(
			HKEY_CURRENT_USER,
			Common::CharMap::path,
			0,
			KEY_SET_VALUE,
			&key
		);

		if (status != ERROR_SUCCESS)
		{
			status = RegCreateKeyExW(
				HKEY_CURRENT_USER,
				Common::CharMap::path,
				0,
				nullptr,
				REG_OPTION_NON_VOLATILE,
				KEY_SET_VALUE,
				nullptr,
				&key,
				nullptr
			);
		}

		if (status == ERROR_SUCCESS)
		{
			DWORD valueSize = static_cast<DWORD>((wcslen(Common::CharMap::font) + 1) * sizeof(WCHAR));
			status = RegSetValueExW(
				key,
				Common::CharMap::key,
				0,
				REG_SZ,
				reinterpret_cast<const BYTE*>(Common::CharMap::font),
				valueSize
			);
			RegCloseKey(key);
		}
		return (status == ERROR_SUCCESS);
	}

	void startCharMapWithGDT()
	{
		if (Common::setCharMapFontToGDT())
		{
			HINSTANCE inst = ShellExecuteW(
				nullptr,
				L"open",
				Common::CharMap::programName,
				nullptr,
				nullptr,
				SW_SHOWNORMAL
			);

			if ((INT_PTR)inst <= 32)
			{
				AfxMessageBox(L"启动字符映射表失败", MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			AfxMessageBox(L"设置字符映射表字体为 GDT 失败", MB_OK | MB_ICONERROR);
		}
	}

	AcString wrapWithGdtFont(const ACHAR* input)
	{
		AcString result = L"";
		if (input == nullptr)
		{
			return result;
		}

		int len = (int)wcslen(input);
		for (int i = 0; i < len; i++)
		{
			AcString temp;
			// 拼接 AutoCAD MText 格式：{\FGDT;字符}
			// 注意：\\ 是为了转义反斜杠
			temp.format(L"{\\Famgdt;%c}", input[i]);
			result += temp;
		}

		return result;
	}

	AcString getShortCommandName(const ACHAR* cmdName)
	{
		AcString shortName = L"yx";

		if (cmdName == nullptr)
		{
			return shortName;
		}

		const ACHAR* pChar = cmdName;
		while (*pChar != L'\0')
		{
			if (iswupper(*pChar))
			{
                shortName += *pChar;
			}
			++pChar;
		}
		return shortName;
	}

	void registerYxCmd(const ACHAR* cmdName, int flags, AcRxFunctionPtr proc)
	{
		acedRegCmds->addCommand(Common::commandGroup, cmdName, cmdName, flags, proc);
		AcString shortCmdName = Common::getShortCommandName(cmdName);
		if (shortCmdName.compare(cmdName) != 0)
		{
			acedRegCmds->addCommand(Common::commandGroup, shortCmdName, shortCmdName, flags, proc);
		}
	}
}