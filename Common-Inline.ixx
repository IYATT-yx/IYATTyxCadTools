/**
 * @file      Common-Inline.ixx
 * @brief     通用工具模块的内联分区，包含高性能模板函数与常量表达式。
 * @details   本文件作为 Common 模块的内联分区（Module Partition）。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module Common:Inline;
export import std;

export namespace Common
{
	template <typename T>
	T* getObject(const AcDbObjectId objId, AcDb::OpenMode mode)
	{
		AcTransaction* pTrans = actrTransactionManager->topTransaction();
		if (pTrans == nullptr)
		{
			return nullptr;
		}

		AcDbObject* pObj = nullptr;
		if (pTrans->getObject(pObj, objId, mode) == Acad::eOk)
		{
			return T::cast(pObj);
		}
		return nullptr;
	}

	constexpr double deg2rad(double degrees)
	{
		return degrees * M_PI / 180.0;
	}

	constexpr double rad2deg(double radians)
	{
		return radians * 180.0 / M_PI;
	}

	template <typename T, typename Validator>
	bool parse(const CString& strInput, size_t expectedSize, Validator validator, std::vector<T>& outResults)
	{
		outResults.clear();
		std::wstringstream wss((LPCTSTR)strInput);
		T dTmp;

		while (wss >> dTmp)
		{
			if (!validator(dTmp))
			{
				return false;
			}
			outResults.push_back(dTmp);
		}

		if (!wss.eof() && wss.fail())
		{
			return false;
		}

		return outResults.size() == expectedSize;
	}
}

