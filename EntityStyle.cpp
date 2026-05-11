/**
 * @file      EntityStyle.cpp
 * @brief     实体样式模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"

module EntityStyle;

import Common;

namespace EntityStyle
{
	void setByLayer(const AcDbObjectId& objId)
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