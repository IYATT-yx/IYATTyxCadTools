#include "stdafx.h"
#include "EntityStyle.hpp"
#include "Common.hpp"

namespace EntityStyle
{
	void setByLayer(AcDbObjectId objId)
	{
		AcDbEntity* pEntity = Common::getObject<AcDbEntity>(objId, AcDb::kForWrite);
		if (pEntity == nullptr)
		{
			return;
		}

		// 颜色
		AcCmColor color;
		color.setByLayer();
		pEntity->setColor(color);


		// 线型
		pEntity->setLinetype(L"ByLayer");
		// 线宽
		pEntity->setLineWeight(AcDb::kLnWtByLayer);
	}
}