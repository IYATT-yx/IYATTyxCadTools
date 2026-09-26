/**
 * @file      UtilEntity.cpp
 * @brief     通用实体模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module UtilEntity;
import FrameworkTranslator;

namespace UtilEntity
{
	Adesk::UInt16 getEntityActualColorIndex(const AcDbEntity* pEntity)
	{
		// 默认回退颜色为 7 (AutoCAD 默认黑白)
		Adesk::UInt16 colorIndex = 7;

		if (pEntity == nullptr)
		{
			return colorIndex;
		}

		AcCmColor cmColor = pEntity->entityColor();
		if (cmColor.isByLayer())
		{
			AcDbLayerTableRecord* pLayer = UtilEntity::getObject<AcDbLayerTableRecord>(pEntity->layerId(), AcDb::kForRead);
			if (pLayer != nullptr)
			{
				colorIndex = pLayer->color().colorIndex();
			}
		}
		else if (cmColor.isByBlock())
		{
		}
		else
		{
			colorIndex = cmColor.colorIndex();
		}

		if (colorIndex == 0 || colorIndex >= 256)
		{
			colorIndex = 7;
		}

		return colorIndex;
	}

	void printClassHierarchy(const AcDbObjectId& objId)
	{
		CAcModuleResourceOverride resOverride;
		AcDbObject* pObj = UtilEntity::getObject<AcDbObject>(objId, AcDb::kForRead);
		if (pObj == nullptr)
		{
			return;
		}

		// 获取当前对象的类描述
		AcRxClass* pClass = pObj->isA();
		if (pClass == nullptr)
		{
			return;
		}

		// 存储继承链中的类名指针
		AcArray<const wchar_t*> hierarchy;
		AcRxClass* pTempClass = pClass;

		// 向上迭代获取所有父类名
		while (pTempClass != nullptr)
		{
			// AcRxClass::name() 在 Unicode 版 ARX 中返回 const wchar_t*
			hierarchy.append(pTempClass->name());
			pTempClass = pTempClass->myParent();
		}

		acutPrintf(_(L"\n--- 继承链 ---"));

		// 从最高层级向当前层级打印
		for (int i = hierarchy.length() - 1; i >= 0; --i)
		{
			// 计算缩进量
			int indent = (hierarchy.length() - 1 - i) * 2;

			// 打印缩进和类名
			acutPrintf(L"\n%*s%s", indent, L"", hierarchy[i]);

			// 标记对象自身的最终类名
			if (i == 0)
			{
				acutPrintf(_(L"<-- [当前类名]"));
			}
		}

		acutPrintf(L"\n----------------------------\n");
	}
}