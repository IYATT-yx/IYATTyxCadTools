module;
#include "StdAfx.h"

export module Dimension;
import Common;

export namespace Dimension
{
	// 标注数据
	struct DimensionData
	{
		bool status = false; // 读取成功状态
		double measuredValue = 0; // 实测值
		int measuredValuePrecision = 0; // 实测值精度
		AcString prefix = L""; // 前缀，如直径、半径等符号
        AcString suffix = L""; // 后缀，如角度符号
		bool isAngle = false; // 是否为角度标注
		AcString dimText= L""; // 标注文本
		double tolUpper = 0; // 上偏差极限
        double tolLower = 0; // 下偏差极限
		int tolPrecision = 0; // 极限偏差精度

		/**
		 * @brief 获取角度值
		 * @return 角度值。非角度值返回 -1。
		 */
		constexpr double degreeValue()
		{
			if (this->isAngle)
			{
				return Common::rad2deg(this->measuredValue);
			}
			else
			{
				AfxMessageBox(L"该标注不是角度，无法获取角度值。", MB_OK | MB_ICONWARNING);
				return -1;
			}
		}
	};
}

export namespace Dimension
{
	/**
	 * @brief 固定标注
	 * @param objId 标注对象ID
	 */
	void dimensionFix(AcDbObjectId objId);
	/**
	 * @brief 取消固定标注
	 * @param objId 标注对象ID
	 */
	void dimensionResume(AcDbObjectId objId);
	/**
	 * @brief 添加标注前后字符
	 * @param objId 标注对象ID
	 * @param left 左字符
	 * @param right 右字符
	 * @param isLGdt 左字符是否为 GDT
	 * @param isRGdt 右字符是否为 GDT
	 */
	void addSurroundingCharsForDimension(AcDbObjectId objId, const wchar_t* left, const wchar_t* right, bool isLGdt = false, bool isRGdt = false);
	/**
	 * @brief 删除标注前后字符
	 * @param objId 标注对象ID
	 * @param left 左字符
	 * @param right 右字符
	 * @param isLGdt 左字符是否为 GDT
	 * @param isRGdt 右字符是否为 GDT
	 */
	void removeSurroundingCharsForDimension(AcDbObjectId objId, const wchar_t* left, const wchar_t* right, bool isLGdt = false, bool isRGdt = false);
	/**
	 * @brief 设置或取消标注的理论尺寸框
	 * @param objId 标注对象ID
	 * @param isSet 是否设置
	 */
	void setAndUnsetBasicBox(AcDbObjectId objId, bool isSet);

	/**
	 * @brief 设置或取消标注的参考尺寸括号
	 * @param objId 标注对象ID
	 * @param isSet 是否设置
	 */
	void setAndUnsetRefDim(AcDbObjectId objId, bool isSet);

	/**
	 * @brief 读取标注数据
	 * @param id 标注对象ID
	 * @param data 传出数据
	 */
	void readDim(AcDbObjectId id, DimensionData& data);
}

