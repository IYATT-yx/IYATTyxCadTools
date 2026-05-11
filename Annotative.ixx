/**
 * @file      Annotative.ixx
 * @brief     注释性模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"

export module Annotative;

export namespace Annotative
{
	/**
	 * @brief 对实体对象应用当前注释性比例
	 * @param pEnt 实体对象
	 */
	void applyCurrentAnnotativeScale(AcDbEntity* pEnt);

	/**
	 * @brief 设置对象的注释性（Annotative）属性
	 * @param pObj 目标对象的指针（如 AcDbBlockTableRecord 或 AcDbEntity）
	 * @param bAnnotative 是否开启注释性
	 * @return Acad::ErrorStatus 返回操作状态
	 */
	Acad::ErrorStatus setObjAnnotative(AcDbObject* pObj, bool bAnnotative = true);

	/**
	 * @brief 获取当前数据库的注释比例缩放值
	 * @return double 缩放系数。例如 1:100 返回 100.0；1:1 返回 1.0。返回负数代表失败。
	 */
	double getCurrentScaleValue();
};