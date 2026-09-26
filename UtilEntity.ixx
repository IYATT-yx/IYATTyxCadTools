/**
 * @file      UtilEntity.ixx
 * @brief     通用实体模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilEntity;

// 内联函数
export namespace UtilEntity
{
	/**
	 * @brief 通过对象ID获取对象指针。对象指针自动释放，不能调用close。
	 * @tparam T 目标对象类型
	 * @param objId 对象ID
	 * @param mode 打开模式，默认为只读
	 * @return 对象指针，如果获取失败则返回nullptr
	 */
	template <typename T>
	T* getObject(const AcDbObjectId objId, AcDb::OpenMode mode = AcDb::kForRead)
	{
		AcTransaction* pTrans = actrTransactionManager->topTransaction();
		if (pTrans == nullptr)
		{
			return nullptr;
		}

		AcDbObject* pObj = nullptr;
		if (pTrans->getObject(pObj, objId, mode) == Acad::eOk)
		{
			return T::cast(pObj);
		}
		return nullptr;
	}
}

export namespace UtilEntity
{
	/**
	 * @brief 获取数据库实体的实际显示颜色索引 (ACI)
	 * * @details 该函数会解析实体的颜色属性。如果实体颜色为 ByLayer，则返回其所属图层的颜色；
	 * 如果为 ByBlock 或其他特殊情况，则回退到 AutoCAD 默认颜色（索引 7）。
	 * * @param pEntity 指向要查询的 AutoCAD 实体的指针
	 * @return Adesk::UInt16 返回 1-255 之间的颜色索引值，默认返回 7 (黑/白)
	 */
	Adesk::UInt16 getEntityActualColorIndex(const AcDbEntity* pEntity);

	/**
	 * @brief 打印指定对象ID的类层次结构
	 * @param objId 对象ID
	 */
	void printClassHierarchy(const AcDbObjectId& objId);
}