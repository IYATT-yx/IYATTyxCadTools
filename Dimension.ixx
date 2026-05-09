module;
#include "StdAfx.h"
#include "resource.h"

export module Dimension;
import Common;

export namespace Dimension
{
	// 标注数据
	struct DimensionData
	{
		bool status = false; // 读取成功状态
		double measured = 0; // 实测值，读取时使用 dimensionValue 方法
		int precision = 0; // 尺寸精度
		AcString prefix = L""; // 前缀，如直径、半径等符号
        AcString suffix = L""; // 后缀，如角度符号
		bool angle = false; // 是否为角度标注
		AcString dimensionText= L""; // 文字替代原始内容
		AcString text = L""; // 解析后的尺寸内容
		double upperDeviation = 0; // 上偏差极限
        double lowerDeviation = 0; // 下偏差极限
		int tolerancePrecision = 0; // 公差精度

		/**
		 * @brief 获取尺寸值，自动处理角度
		 * @return 尺寸值。
		 */
		constexpr double dimensionValue()
		{
			if (this->angle)
			{
				return Common::rad2deg(this->measured);
			}
			else
			{
				return this->measured;
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
	void dimensionSolidify(const AcDbObjectId& objId);
	/**
	 * @brief 取消固定标注
	 * @param objId 标注对象ID
	 */
	void dimensionRelink(const AcDbObjectId& objId);
	/**
	 * @brief 添加标注前后字符
	 * @param objId 标注对象ID
	 * @param left 左字符
	 * @param right 右字符
	 * @param isLGdt 左字符是否为 GDT
	 * @param isRGdt 右字符是否为 GDT
	 */
	void addSurroundingCharsForDimension(const AcDbObjectId& objId, const wchar_t* left, const wchar_t* right, bool isLGdt = false, bool isRGdt = false);
	/**
	 * @brief 删除标注前后字符
	 * @param objId 标注对象ID
	 * @param left 左字符
	 * @param right 右字符
	 * @param isLGdt 左字符是否为 GDT
	 * @param isRGdt 右字符是否为 GDT
	 */
	void removeSurroundingCharsForDimension(const AcDbObjectId& objId, const wchar_t* left, const wchar_t* right, bool isLGdt = false, bool isRGdt = false);
	/**
	 * @brief 设置或取消标注的理论尺寸框
	 * @param objId 标注对象ID
	 * @param isSet 是否设置
	 */
	void setAndUnsetBasicBox(const AcDbObjectId& objId, bool isSet);

	/**
	 * @brief 设置或取消标注的参考尺寸括号
	 * @param objId 标注对象ID
	 * @param isSet 是否设置
	 */
	void setAndUnsetRefDim(const AcDbObjectId& objId, bool isSet);

	/**
	 * @brief 读取标注数据
	 * @param id 标注对象ID
	 * @param data 传出数据
	 */
	void readDim(const AcDbObjectId& id, DimensionData& data);

	/**
	 * @brief 设置标注精度
	 * @param id 标注对象ID
	 * @param iDimPrec 主单位精度，-1 表示不修改，0 至 8 的整数修改精度
	 * @param iTolPrec 公差精度，-1 表示不修改，0 至 8 的整数修改精度
	 */
	void setDimensionTolerancePreccision(const AcDbObjectId& id, const int& iDimPrec = -1, const int& iTolPrec = -1);

	/**
	 * @brief 检查标注的文字替代字符串中是否包含包裹在大括号内的有效公差控制字符
	 * @param text 标注对象的文字替代内容
	 * @return bool 如果检测到有效的公差格式则返回 true，否则返回 false
	 */
	bool isEnhancedToleranceApplied(const AcString& text);
}

