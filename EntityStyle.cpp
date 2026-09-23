/**
 * @file      EntityStyle.cpp
 * @brief     实体样式模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module EntityStyle;

import Common;
import Commands;
import UniversalPicker;
import Translator;

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

// 命令注册
namespace
{
	void cmdSetByLayer()
	{
		UniversalPicker::run(nullptr, EntityStyle::setByLayer, _(L"设置实体样式为当前层样式"));
	}

	Commands::AutoRegister ar({L"yxSetByLayer", []() { return _(L"设置实体样式为当前层样式"); }, Commands::CommandFlags::PickRedraw, cmdSetByLayer});
}