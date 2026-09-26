/**
 * @file      UtilString.cpp
 * @brief     字符串处理模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module UtilString;
import UtilConstants;

namespace UtilString
{
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision, bool forcePlusSign, bool addSpaceIfZero)
	{
		acdbRToS(doubleValue, AcStringValue, UtilConstants::UnitMode::Decimal, precision);

		if (forcePlusSign && doubleValue > UtilConstants::Epsilon)
		{
			AcStringValue = L"+" + AcStringValue;
		}
		else if (addSpaceIfZero && std::abs(doubleValue) <= UtilConstants::Epsilon)
		{
			AcStringValue = L" " + AcStringValue;
		}
	}

	AcString wrapWithGdtFont(const wchar_t* input)
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

	CString loadString(UINT nID)
	{
		CString tmp;
		tmp.LoadStringW(nID);
		return tmp;
	}
}