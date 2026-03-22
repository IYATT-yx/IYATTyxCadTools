module;
#include "StdAfx.h"

export module Block;

import Common;

export namespace Block
{
	/**
	 * @brief 创建序列号块
	 */
	void createSerialNumberBlock();

	/**
	 * @brief 插入序列号块
	 * @param insPt 插入点
	 * @param numStr 序列号
	 * @param dScale 比例
	 */
	void insertSerialNumber(AcGePoint3d insPt, unsigned int num, double dScale = 1.0);

	/**
	 * @brief 用于动态拖拽插入序号块，可以实时预览块
	 */
	class SerialNumberJig : public AcEdJig
	{
	public:
		/**
		 * @brief 构造函数，初始化序号块
		 * @param num 序号
		 * @param dScale 比例
		 */
		SerialNumberJig(unsigned int num, double dScale = 1.0);

		/**
		 * @brief 析构函数,释放分配的块参照对象
		 */
		~SerialNumberJig();

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
		double mdScale; // 比例
	};

	/**
	 * @brief 从指定序号开始插入
	 * @param num 开始序号
	 * @param dScale 比例
	 */
	void insertSerialNumberBlockWithStartNumber(int num, double dScale);
}

