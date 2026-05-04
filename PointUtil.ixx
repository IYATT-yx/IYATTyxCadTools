module;
#include "stdafx.h"

export module PointUtil;

export namespace PointUtil
{
	/**
	 * @brief 将点坐标数组批量绘制为 AcDbPoint 实体并添加至当前数据库空间。
	 * @details 函数会自动获取当前活动的块表记录（模型空间或纸张空间），并通过事务管理获取其指针。
	 *          对于每一个点实体，若添加失败将自动执行内存清理以防止泄漏。
	 * @param points 包含目标交点或坐标的 AcGePoint3dArray 常量引用。
	 * @return size_t 成功创建并添加至数据库的点实体总数。
	 */
	size_t drawPoints(const AcGePoint3dArray& points);
};