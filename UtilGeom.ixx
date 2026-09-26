/**
 * @file      UtilGeom.ixx
 * @brief     通用几何模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilGeom;

// 内联函数
export namespace UtilGeom
{
	/**
	 * @brief 角度转弧度
	 * @param degrees 角度值
	 * @return 弧度值
	 */
	constexpr double deg2rad(double degrees)
	{
		return degrees * M_PI / 180.0;
	}

	/**
	 * @brief 弧度转角度
	 * @param radians 弧度值
	 * @return 角度值
	 */
	constexpr double rad2deg(double radians)
	{
		return radians * 180.0 / M_PI;
	}
}

export namespace UtilGeom
{
	/**
	 * @brief 获取实体的几何中心点
	 * @param pEnt 指向 AutoCAD 实体的指针
	 * @param pCenter [out] 用于接收几何中心点的指针（WCS 坐标）
	 * @return true 成功获取坐标；false 获取失败，pCenter 不被修改
	 *
	 * @details 该方法通过获取实体的最小包围框（Geometric Extents）来计算中心。
	 * 相比于文字的插入点，几何中心点更能反映物体在空间中的视觉核心位置，
	 * 且不受文字对齐方式（Alignment）或块基点偏移的影响。
	 */
	bool getEntityCenter(AcDbEntity* pEnt, AcGePoint3d* pCenter);

	/**
	 * @brief 获取指定坐标点处的相邻实体
	 *
	 * 使用 AutoCAD Crossing (C) 模式在指定点进行空间检索。
	 * 检索逻辑会将与该点接触或包含该点的所有符合过滤条件的实体纳入结果集。
	 *
	 * @param pt      检索的基准坐标点（世界坐标系 WCS）。
	 * @param pFilter 实体过滤器链表（resbuf），用于限制检索范围（如仅检索直线、圆弧等）。
	 *                注意：调用者负责该过滤器的创建，本函数内部不释放该指针。
	 * @return AcDbObjectIdArray 检索到的实体 ObjectId 集合。若未找到任何匹配实体，则返回空数组。
	 *
	 * @note 1. 内部调用 acedSSGet(L"C", ...)，受当前视图显示范围及图层锁定状态影响。
	 * @note 2. 检索结果包含所有重叠在 pt 处的实体，不区分实体的几何起始或终点。
	 */
	AcDbObjectIdArray getNeighborsAtPoint(const AcGePoint3d& pt, resbuf* pFilter);
}