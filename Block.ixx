module;
#include "StdAfx.h"

export module Block;

import Common;

export namespace Block
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

export namespace Block
{
	/**
	 * @brief 创建序列号块
	 */
	void createBalloonNumberBlock();

	/**
	 * @brief 插入序列号块
	 * @param insPt 插入点
	 * @param numStr 序列号
	 */
	void insertBalloonNumber(AcGePoint3d insPt, unsigned int num);

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
	bool updateBalloonNumberBlock(AcDbObjectId blockRefId, unsigned int newNum);

	/**
	 * @brief 更新气泡号块参照的属性
	 * @param pBlkRef 块参照对象指针
	 * @param num 序号
	 */
	void syncAttributesFromDefinition(AcDbBlockReference* pBlkRef, unsigned int num);
}