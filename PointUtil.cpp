/**
 * @file      PointUtil.cpp
 * @brief     点模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"

module PointUtil;
import Common;

namespace PointUtil
{
	size_t drawPoints(const AcGePoint3dArray& points)
	{
        if (points.isEmpty())
        {
            return 0;
        }

        // 获取当前活动空间的块表记录（Model Space 或 Paper Space）
        AcDbObjectId currentSpaceId = acdbCurDwg()->currentSpaceId();
        AcDbBlockTableRecord* pBlockTableRecord = Common::getObject<AcDbBlockTableRecord>(currentSpaceId, AcDb::kForWrite);
        if (pBlockTableRecord == nullptr)
        {
            return 0;
        }

        size_t successCount = 0;
        for (int i = 0; i < points.length(); ++i)
        {
            AcDbPoint* pNewPoint = new AcDbPoint(points[i]);
            if (pBlockTableRecord->appendAcDbEntity(pNewPoint) == Acad::eOk)
            {
                pNewPoint->close();
                ++successCount;
            }
            else
            {
                delete pNewPoint;
                pNewPoint = nullptr;
            }
        }
        return successCount;
	}
};