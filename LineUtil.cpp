/**
 * @file      LineUtil.cpp
 * @brief     线模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"

module LineUtil;
import Common;

namespace LineUtil
{
	bool calculateLineIntersection(const AcDbObjectId& lineId1, const AcDbObjectId& lineId2, AcGePoint3dArray& results)
	{
		AcDbCurve* pLine1 = Common::getObject<AcDbCurve>(lineId1, AcDb::kForRead);
        AcDbCurve* pLine2 = Common::getObject<AcDbCurve>(lineId2, AcDb::kForRead);
		if (pLine1 == nullptr || pLine2 == nullptr)
		{
			return false;
		}

		Acad::ErrorStatus es = pLine1->intersectWith(pLine2, AcDb::kExtendBoth, results);
		if (es != Acad::eOk || results.isEmpty())
		{
			return false;
		}
        return true;
	}
};