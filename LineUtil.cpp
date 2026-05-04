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