/**
 * @file      LineUtil.ixx
 * @brief     线模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"

export module LineUtil;

export namespace LineUtil
{
	/**
	 * @brief 计算两条曲线（及其延长线）的交点。
	 * @details 使用 AcDb::kExtendBoth 模式计算两个实体在几何意义上的所有交点。
	 * @param lineId1 第一个实体的 ObjectId（需派生自 AcDbCurve）。
	 * @param lineId2 第二个实体的 ObjectId（需派生自 AcDbCurve）。
	 * @param results 存储计算出的交点坐标数组。
	 * @return 如果成功计算出至少一个交点则返回 true，否则返回 false。
	 */
	bool calculateLineIntersection(const AcDbObjectId& line1, const AcDbObjectId& line2, AcGePoint3dArray& results);
};