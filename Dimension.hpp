#pragma once

namespace Dimension
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
	 */
	void addSurroundingCharsForDimension(AcDbObjectId objId, const ACHAR* left, const ACHAR* right);
	/**
	 * @brief 删除标注前后字符
	 * @param objId 标注对象ID
	 * @param left 左字符
	 * @param right 右字符
	 */
	void removeSurroundingCharsForDimension(AcDbObjectId objId, const ACHAR* left, const ACHAR* right);

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
}

