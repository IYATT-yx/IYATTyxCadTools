module;
#include "StdAfx.h"

export module UniversalPicker;

// 处理函数原型
export using EntityProcessor = std::function<void(const AcDbObjectId)>;

export class UniversalPicker
{
public:
	// 选择模式
	enum class SelectMode
	{
		Batch, // 批量模式：选完按回车一起处理
		Immediate // 即时模式：点一个处理一个
	};

private:
	/**
	 * @brief 构建过滤器
	 * @param filterStr 过滤字符串，英文逗号分隔的实体类型列表，如 "LINE,CIRCLE"
	 * @return 过滤器链表，调用者负责释放
	 */
	static resbuf* buildFilter(const ACHAR* filterStr);

	/**
	 * @brief 批量选择处理
	 * @param filter 过滤器
	 * @param processor 处理函数，接受一个实体ID参数
	 */
	static void batchSelect(const ACHAR* filter, EntityProcessor processor);

	/**
	 * @brief 即时选择处理
	 * @param filter 过滤器
	 * @param processor 处理函数，接受一个实体ID参数
	 */
	static void immediateSelect(const ACHAR* filter, EntityProcessor processor);

	/**
	 * @brief 释放过滤器链表
	 * @param filterRb 过滤器链表，调用者负责传入，函数内部会释放
	 */
	static void freeFilter(resbuf* filterRb);

public:
	/**
	 * @brief 运行选择器
	 * @param filter 过滤器
	 * @param processor 处理函数，接受一个实体ID参数
	 * @param prompt 提示信息
	 * @param defaultSelectMode 默认选择模式，默认为批量模式
	 */
	static void run(const ACHAR* filter, EntityProcessor processor, const ACHAR* prompt, UniversalPicker::SelectMode defaultSelectMode = UniversalPicker::SelectMode::Batch);
};

