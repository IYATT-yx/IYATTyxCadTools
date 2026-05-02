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
	static void batchSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, SortMode defaultSortMode, bool isLocked, double tol);

	/**
	 * @brief 即时选择处理
	 * @param arcv 实体类型数组指针，用于过滤选择对象 
	 * @param processor 处理函数，接受一个实体ID参数
	 */
	static void immediateSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor);

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
	static void run(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, const wchar_t* prompt, UniversalPicker::SelectMode defaultSelectMode = UniversalPicker::SelectMode::Batch, bool lockSelectMode = false, SortMode defaultSortMode = UniversalPicker::SortMode::None, bool lockSortMode = true, double sortTol = 3.5);

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

