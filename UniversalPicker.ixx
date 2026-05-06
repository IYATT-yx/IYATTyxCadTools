/**
 * @file      UniversalPicker.ixx
 * @brief     实体通用选择器模块，负责 AutoCAD 交互式对象筛选与空间排序。
 * @details   集成批量与即时选择模式，支持通过坐标容差进行九轴空间定位排序。
 *            内置高亮与夹点同步机制，确保自定义实体在 API 选中后的视觉反馈。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UniversalPicker;

export class UniversalPicker
{
public:
	// 选择模式
	enum class SelectMode
	{
		Batch, // 批量模式：选完按回车一起处理
		Immediate // 即时模式：点一个处理一个
	};

	/**
	 * @brief 实体空间排序模式枚举
	 * * 排序逻辑由字符顺序决定：
	 * - 首字母为 L/R: 水平优先 (按行排序)
	 * - 首字母为 U/D: 垂直优先 (按列排序)
	 */
	enum class SortMode
	{
		None,   ///< 不排序
		RD,     ///< 右-下 (水平优先：从左到右，行从上到下)
		RU,     ///< 右-上 (水平优先：从左到右，行从下到上)
		LD,     ///< 左-下 (水平优先：从右到左，行从上到下)
		LU,     ///< 左-上 (水平优先：从右到左，行从下到上)
		DR,     ///< 下-右 (垂直优先：从上到下，列从左到右)
		DL,     ///< 下-左 (垂直优先：从上到下，列从右到左)
		UR,     ///< 上-右 (垂直优先：从下到上，列从左到右)
		UL      ///< 上-左 (垂直优先：从下到上，列从右到左)
	};

	/**
	 * @brief 内部辅助结构体，存储对象 ID 及其空间坐标
	 */
	struct EntityInfo
	{
		AcDbObjectId id;    ///< 对象的 ObjectId
		AcGePoint3d pos;    ///< 对象的排序基准点
	};

	// 处理函数原型
	using EntityProcessor = std::function<void(const AcDbObjectId&)>;
	using EntityProcessorBreak = std::function<void(const AcDbObjectId&, bool&)>;
	// 实体类型数组
	using AcRxClassVector = std::vector<AcRxClass*>;
	using AcRxClassVectorPtr = AcRxClassVector*;

private:
	/**
	 * @brief 批量选择处理
	 * @param arcv 实体类型数组指针，用于过滤选择对象 
	 * @param processor 处理函数，接受一个实体ID参数
	 * @param defaultSortMode 默认排序模式
	 * @param isLocked 是否锁定选择模式
 	 * @param tol 坐标容差
	 */
	static inline void batchSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, SortMode defaultSortMode, bool isLocked, double sortTol);

	/**
	 * @brief 批量选择处理，带单次执行中断参数，仅用于兼容立即模式的中断参数，实际没有作用的
	 * @param arcv 
	 * @param processor 带中断参数的处理函数
	 * @param defaultSortMode 
	 * @param isLocked 
	 * @param tol 
	 */
	static void batchSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessorBreak processor, SortMode defaultSortMode, bool isLocked, double tol);

	/**
	 * @brief 即时选择处理
	 * @param arcv 实体类型数组指针，用于过滤选择对象 
	 * @param processor 处理函数，接受一个实体ID参数
	 */
	static inline void immediateSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor);

	/**
	 * @brief 即时选择处理，带单次执行中断参数
	 * @param arcv 实体类型数组指针，用于过滤选择对象
	 * @param processor 处理函数，接受一个实体ID参数和一个bool中断参数。处理函数中将中断参数设为 true 时，则单次执行后结束，否则可以继续选择实体进行处理。
	 */
	static void immediateSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessorBreak processor);

	/**
	 * @brief 根据指定的方位模式比较两个实体的空间位置
	 * @param a 实体 A
	 * @param b 实体 B
	 * @param mode 排序规则模式，默认优先行从左到右，从上到下
	 * @param sortTol 坐标容差
	 * @return true 如果 a 应该排在 b 之前
	 */
	static bool compareEntities(const UniversalPicker::EntityInfo& a, const UniversalPicker::EntityInfo& b, UniversalPicker::SortMode mode, double sortTol);

	/**
	 * @brief 将排序模式转为字符串
	 * @param mode 排序模式 
	 * @return 字符串
	 */
	static const wchar_t* SortModeToString(UniversalPicker::SortMode mode);

public:
	/**
	 * @brief 运行选择器
	 * @param arcv 实体类型数组指针，用于过滤选择对象 
	 * @param processor 处理函数，接受一个实体ID参数
	 * @param prompt 提示信息
	 * @param defaultSelectMode 默认选择模式，默认为批量模式
	 * @param lockSelectMode 是否锁定选择模式，默认为不锁定
	 * @param defaultSortMode 默认排序模式，默认不排序
	 * @param lockSortMode 是否锁定排序模式，默认锁定
	 * @param sortTol 坐标容差，默认为 3.5。坐标超过容差认为是不同行或不同列。
	 */
	static inline void run(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, const wchar_t* prompt, UniversalPicker::SelectMode defaultSelectMode = UniversalPicker::SelectMode::Batch, bool lockSelectMode = false, SortMode defaultSortMode = UniversalPicker::SortMode::None, bool lockSortMode = true, double sortTol = 3.5);

	/**
	 * @brief 运行选择器，带单次执行中断参数
	 * @param arcv 
	 * @param processor 处理函数，接受一个实体ID参数和一个bool中断参数。选择模式为立即时，处理函数中将中断参数设为 true 时，则单次执行后结束，否则可以继续选择实体进行处理。
	 * @param prompt 
	 * @param defaultSelectMode 
	 * @param lockSelectMode 
	 * @param defaultSortMode 
	 * @param lockSortMode 
	 * @param sortTol 
	 */
	static void run(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessorBreak processor, const wchar_t* prompt, UniversalPicker::SelectMode defaultSelectMode = UniversalPicker::SelectMode::Batch, bool lockSelectMode = false, SortMode defaultSortMode = UniversalPicker::SortMode::None, bool lockSortMode = true, double sortTol = 3.5);

	/**
	 * @brief 构建过滤器
	 * @param arcv 实体类型数组指针
	 * @return 过滤器链表，调用者负责释放
	 */
	static resbuf* buildFilter(UniversalPicker::AcRxClassVectorPtr arcv);

	/**
	 * @brief 释放过滤器链表
	 * @param filterRb 过滤器链表，调用者负责传入，函数内部会释放
	 */
	static void freeFilter(resbuf* filterRb);

	/**
	 * @brief 获取用户选中的单个实体 ID
	 * @param arcv 实体类型数组指针，用于过滤选择对象
	 * @return 实体 ID，返回 AcDbObjectId::kNull 时失败或取消选择。
	 */
	static AcDbObjectId getSelectedSingleEntityId(UniversalPicker::AcRxClassVectorPtr arcv);

	/**
	 * @brief 设置指定 ID 列表中的实体为选中状态
	 * @param ids ID 列表
	 */
	static void setSelection(const AcDbObjectIdArray& ids);
};

export void UniversalPicker::batchSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, SortMode defaultSortMode, bool isLocked, double sortTol)
{
	UniversalPicker::batchSelect(
		arcv,
		[processor](const AcDbObjectId& id, bool& bBreak)
		{
			processor(id);
			bBreak = false;
		},
		defaultSortMode,
		isLocked,
		sortTol
	);
}

export void UniversalPicker::immediateSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor)
{
	UniversalPicker::immediateSelect(
		arcv,
		[processor](const AcDbObjectId& id, bool& bBreak)
		{
			processor(id);
			bBreak = false;
		}
	);
}

export void UniversalPicker::run(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, const wchar_t* prompt, UniversalPicker::SelectMode defaultSelectMode, bool lockSelectMode, SortMode defaultSortMode, bool lockSortMode, double sortTol)
{
	UniversalPicker::run(
		arcv,
		[processor](const AcDbObjectId& id, bool& bBreak)
		{
			processor(id);
			bBreak = false;
		},
		prompt,
		defaultSelectMode,
		lockSelectMode,
		defaultSortMode,
		lockSortMode,
		sortTol
	);
}