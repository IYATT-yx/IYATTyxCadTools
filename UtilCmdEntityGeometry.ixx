/**
 * @file      UtilCmdEntityGeometry.ixx
 * @brief     实体几何操作命令
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilCmdEntityGeometry;
import UiUniversalPicker;
import FrameworkCommands;
import FrameworkTranslator;
import UtilCommon;
import UtilPoint;
import UtilLine;
import UtilEntityStyle;

namespace
{
	void cmdChainSelection()
	{
		UiUniversalPicker::AcRxClassVector arcv = { AcDbArc::desc(), AcDbPolyline::desc(), AcDbLine::desc(), AcDbSpline::desc() };
		resbuf* pFilter = UiUniversalPicker::buildFilter(&arcv);

		UiUniversalPicker::run(
			&arcv,
			[&](const AcDbObjectId& id, bool& bBreak)
			{
				// 局部容器：确保每次点击都是独立的搜索过程
				std::queue<AcDbObjectId> waitingQueue;
				std::set<AcDbObjectId> processedIds;
				AcDbObjectIdArray resultIds;

				waitingQueue.push(id);
				processedIds.insert(id);

				// 广度优先搜索 (BFS)
				while (!waitingQueue.empty())
				{
					AcDbObjectId currentId = waitingQueue.front();
					waitingQueue.pop();
					resultIds.append(currentId);

					AcDbCurve* pCurve = UtilCommon::getObject<AcDbCurve>(currentId, AcDb::kForRead);
					if (pCurve == nullptr)
					{
						continue;
					}

					AcGePoint3d startPt, endPt;
					if (pCurve->getStartPoint(startPt) == Acad::eOk && pCurve->getEndPoint(endPt) == Acad::eOk)
					{
						AcGePoint3d checkPts[2] = { startPt, endPt };

						for (const auto& pt : checkPts)
						{
							AcDbObjectIdArray neighbors = UtilCommon::getNeighborsAtPoint(pt, pFilter);

							for (int i = 0; i < neighbors.length(); ++i)
							{
								AcDbObjectId nId = neighbors[i];

								// 严格去重：
								// 1. 跳过当前正在处理的实体本身 (nId != currentId)
								// 2. 跳过已经进入过队列或处理过的实体 (processedIds.find == end)
								if (nId != currentId && processedIds.find(nId) == processedIds.end())
								{
									processedIds.insert(nId);
									waitingQueue.push(nId);
								}
							}
						}
					}
				}

				if (resultIds.length() > 0)
				{
					UiUniversalPicker::setSelection(resultIds);
					acutPrintf(_(L"\n自动链式选择成功：共选中 %d 条线条实体。"), resultIds.length());
					bBreak = true;
				}
			},
			_(L"选中实体后自动链式选择"),
			UiUniversalPicker::SelectMode::Immediate,
			true
		);

		UiUniversalPicker::freeFilter(pFilter);
	}

	void cmdIntersect()
	{
		FrameworkCommands::CommandList pszCmdList =
		{
			L"FILLET",
			L"M",
			L"R",
			L"0"
		};
		FrameworkCommands::executeCommand(pszCmdList);
	}

	void cmdCreateIntersectionPoints()
	{
		// AcDbCurve 曲线类的子类
		UiUniversalPicker::AcRxClassVector arcv =
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
		UiUniversalPicker::run(
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
					if (UtilLine::calculateLineIntersection(lastId, id, intersectionPoints))
					{
						UtilPoint::drawPoints(intersectionPoints);
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
			UiUniversalPicker::SelectMode::Immediate,
			true
		);
	}

	void cmdSetByLayer()
	{
		UiUniversalPicker::run(nullptr, UtilEntityStyle::setByLayer, _(L"设置实体样式为当前层样式"));
	}

	FrameworkCommands::AutoRegister ar =
	{
		{ L"yxChainSelection", []() { return _(L"选中实体后自动链式选择"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdChainSelection },
		{ L"yxIntersect", []() { return _(L"将两条直线延伸或修剪至其交点"); }, FrameworkCommands::CommandFlags::Base, cmdIntersect },
		{ L"yxCreateIntersectionPoints", []() { return _(L"创建两条线(及延长线)的交点。可使用PTYPE设置点样式。"); }, FrameworkCommands::CommandFlags::Base, cmdCreateIntersectionPoints },
		{ L"yxSetByLayer", []() { return _(L"设置实体样式为当前层样式"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdSetByLayer }
	};
}