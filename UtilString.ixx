/**
 * @file      UtilString.ixx
 * @brief     字符串处理模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilString;

// 内联
export namespace UtilString
{
	/**
	 * @brief 解析 CString 字符串并执行自定义校验逻辑
	 * @tparam T 目标数值类型
	 * @tparam Validator 校验器类型（通常为 Lambda 表达式）
	 * @param strInput 输入的 MFC CString 字符串
	 * @param expectedSize 期望解析到的有效数值数量
	 * @param validator 用于校验每个数值的 Lambda 或函数对象
	 * @param outResults 存储最终解析结果的容器引用
	 * @return 解析成功且通过所有校验返回 true，否则返回 false
	 */
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

export namespace UtilString
{
	/**
	 * @brief 将double值转换为AcString格式的字符串
	 * @param doubleValue 要转换的double值
	 * @param AcStringValue 转换后的AcString字符串
	 * @param precision 小数点精度，默认为3位
	 * @param forcePlusSign 是否强制显示正号，默认为false
	 * @param addSpaceIfZero 如果值为零，是否在前面添加一个空格，默认为false
	 */
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision = 3, bool forcePlusSign = false, bool addSpaceIfZero = false);

	/**
	 * @brief 将输入字符串用 GDT 字体包裹
	 * @param input 要包裹的字符串
	 * @return 包裹后的字符串
	 */
	AcString wrapWithGdtFont(const wchar_t* input);

	/**
	 * @brief 读取 String Table 中的字符串
	 * @param nID String Table 中的字符串 ID
	 * @return 字符串
	 */
	CString loadString(UINT nID);
}