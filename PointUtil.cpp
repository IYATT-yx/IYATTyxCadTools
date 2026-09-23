/**
 * @file      PointUtil.cpp
 * @brief     点模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module PointUtil;
import Common;
import UniversalPicker;
import Translator;
import LineUtil;
import Commands;

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

namespace
{
    void cmdCreateIntersectionPoints()
    {
        // AcDbCurve 曲线类的子类
        UniversalPicker::AcRxClassVector arcv =
        {
            AcDb2dPolyline::desc(),
            AcDb3dPolyline::desc(),
            AcDbArc::desc(),
            AcDbCircle::desc(),
            AcDbEllipse::desc(),
            AcDbLeader::desc(),
            AcDbLine::desc(),
            AcDbPolyline::desc(),
            AcDbRay::desc(),
            AcDbSpline::desc(),
            //AcDbHelix::desc(), // 不清楚是哪个库文件中定义的
            AcDbXline::desc()
        };

        AcDbObjectId lastId = AcDbObjectId::kNull;
        acutPrintf(_(L"\n请选择第一条线："));
        UniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (lastId == AcDbObjectId::kNull)
                {
                    lastId = id;
                    acutPrintf(_(L"\n请选择第二条线："));
                }
                else if (lastId != id)
                {
                    AcGePoint3dArray intersectionPoints;
                    if (LineUtil::calculateLineIntersection(lastId, id, intersectionPoints))
                    {
                        PointUtil::drawPoints(intersectionPoints);
                        acutPrintf(_(L"\n成功创建 %d 个交点"), intersectionPoints.length());
                    }
                    else
                    {
                        acutPrintf(_(L"\n未发现交点"));
                    }
                    lastId = AcDbObjectId::kNull;
                    acutPrintf(_(L"\n请选择第一条线："));
                }

            },
            _(L"创建两条线(及延长线)的交点。可使用PTYPE设置点样式。"),
            UniversalPicker::SelectMode::Immediate,
            true
        );
    }

    Commands::AutoRegister ar =
    {
        { L"yxCreateIntersectionPoints", []() { return _(L"创建两条线(及延长线)的交点。可使用PTYPE设置点样式。"); }, Commands::CommandFlags::Base, cmdCreateIntersectionPoints },
    };
}