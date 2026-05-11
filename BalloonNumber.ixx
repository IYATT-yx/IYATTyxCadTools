/**
 * @file      BalloonNumber.ixx
 * @brief     气泡号模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module BalloonNumber;
import Common;

export namespace BalloonNumber
{
	/**
	 * @brief 用于动态拖拽插入序号块，可以实时预览块
	 */
	class BalloonNumberJig : public AcEdJig
	{
	public:
		/**
		 * @brief 构造函数，初始化序号块
		 * @param num 序号
		 */
		BalloonNumberJig(unsigned int num);

		/**
		 * @brief 析构函数,释放分配的块参照对象
		 */
		~BalloonNumberJig();

		/**
		 * @brief 采样函数，用于获取用户拖拽点
		 * @return DragStatus 返回拖拽状态
		 */
		AcEdJig::DragStatus sampler() override;

		/**
		 * @brief 更新函数，每次鼠标移动时更新块参照位置和属性
		 * @return Adesk::Boolean 返回更新状态，kTrue 表示更新成功
		 */
		Adesk::Boolean update() override;

		/**
		 * @brief 获取点给钱实体指针
		 * @return AcDbEntity* 返回块参照对象指针
		 */
		AcDbEntity* entity() const override;

		/**
		 * @brief 获取当前鼠标点
		 * @return AcGePoint3d 当前点坐标
		 */
		AcGePoint3d getPoint() const;

	private:
		void setupAttributes();

	private:
		AcDbBlockReference* mpBlockReference = nullptr; // 预览使用的块参照
		AcDbObjectId mBlockDefineId; // 块定义 ID
		AcGePoint3d mCurPt; // 当前鼠标点
		unsigned int mNum; // 当前序列号
	};
}

export namespace BalloonNumber
{
	/**
	 * @brief 创建序列号块
	 */
	void createBalloonNumberBlock();

	/**
	 * @brief 从指定序号开始插入
	 * @param num 开始序号
	 */
	void insertBalloonNumberBlockWithStartNumber(int num);

	/**
	 * @brief 更新块参照的序列号
	 * @param blockRefId 块参照 ID
	 * @param newNum 新的序列号
	 * @return true 表示更新成功; false 表示更新失败
	 */
	bool updateBalloonNumberBlock(const AcDbObjectId& blockRefId, unsigned int newNum);

	/**
	 * @brief 获取指定块参照的气泡号属性内容
	 * @param blockRefId 块参照的 ObjectId
	 * @param outValue 输出参数，读取到的原始字符串内容
	 * @return true 如果块名匹配且找到了对应的属性标签；false 否则
	 */
	bool getBalloonAttributeValue(const AcDbObjectId& blockRefId, AcString& outValue);

	/**
	 * @brief 对气泡号的序号执行偏置处理
	 * @param id 块参照 ID
	 * @param offset 偏置值
	 */
	void balloonNumberOffset(const AcDbObjectId& id, int offset);

	/**
	 * @brief 判断气泡号编号是否匹配条件
	 * @param attrValue 气泡号编号值
	 * @param criteria 条件
	 * @return true 表示匹配；false 表示不匹配
	 */
	bool meetCriteria(const AcString& attrValue, const AcString& criteria);
}

namespace BalloonNumber
{
	/**
	 * @brief 插入序列号块
	 * @param insPt 插入点
	 * @param numStr 序列号
	 */
	void insertBalloonNumber(AcGePoint3d insPt, unsigned int num);

	/**
	 * @brief 更新气泡号块参照的属性
	 * @param pBlkRef 块参照对象指针
	 * @param num 序号
	 */
	void syncAttributesFromDefinition(AcDbBlockReference* pBlkRef, unsigned int num);
}

export namespace BalloonNumber
{
	// 过滤器筛选操作类型
	namespace OperatorType
	{
		constexpr const wchar_t* equal = L"==";
		constexpr const wchar_t* notEqual1 = L"<>";
		constexpr const wchar_t* notEqual2 = L"《》";
        constexpr const wchar_t* greater1 = L">>";
		constexpr const wchar_t* greater2 = L"》》";
        constexpr const wchar_t* less1 = L"<<";
        constexpr const wchar_t* less2 = L"《《";
		constexpr const wchar_t* greaterEqual1 = L">=";
		constexpr const wchar_t* greaterEqual2 = L"》=";
		constexpr const wchar_t* lessEqual1 = L"<=";
        constexpr const wchar_t* lessEqual2 = L"《=";
	};
}