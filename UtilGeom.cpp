/**
 * @file      UtilGeom.cpp
 * @brief     通用几何模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module UtilGeom;

namespace UtilGeom
{
	bool getEntityCenter(AcDbEntity* pEnt, AcGePoint3d* pCenter)
	{
		if (pEnt == nullptr || pCenter == nullptr)
		{
			return false;
		}

		AcDbExtents extents;
		if (pEnt->getGeomExtents(extents) == Acad::eOk)
		{
			AcGePoint3d minPt = extents.minPoint();
			AcGePoint3d maxPt = extents.maxPoint();

			pCenter->set(
				(minPt.x + maxPt.x) * 0.5,
				(minPt.y + maxPt.y) * 0.5,
				(minPt.z + maxPt.z) * 0.5
			);
			return true;
		}

		return false;
	}

	AcDbObjectIdArray getNeighborsAtPoint(const AcGePoint3d& pt, resbuf* pFilter)
	{
		AcDbObjectIdArray neighbors;
		ads_point adsPt;
		adsPt[X] = pt.x;
		adsPt[Y] = pt.y;
		adsPt[Z] = pt.z;

		ads_name ss;
		// 在坐标点处进行 Crossing 检索
		if (acedSSGet(L"C", adsPt, adsPt, pFilter, ss) == RTNORM)
		{
			Adesk::Int32 length = 0;
			acedSSLength(ss, &length);

			for (long i = 0; i < length; ++i)
			{
				ads_name ent;
				acedSSName(ss, i, ent);

				AcDbObjectId objId;
				if (acdbGetObjectId(objId, ent) == Acad::eOk)
				{
					neighbors.append(objId);
				}
			}
			acedSSFree(ss);
		}
		return neighbors;
	}
}