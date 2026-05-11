/**
 * @file      TextUtil.ixx
 * @brief     文本模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module TextUtil;
import UniversalPicker;
import CsvModule;

export namespace TextUtil
{
	// 文本类型列表
	UniversalPicker::AcRxClassVector textClassList =
	{
		AcDbMText::desc(),
		AcDbText::desc()
	};

	struct TextEntityData
	{
		AcDbObjectId id;
		AcGePoint3d pos;
		AcString text;
	};
	using TextEntityDataList = std::vector<TextEntityData>;
};

export namespace TextUtil
{
	/**
	 * @brief 从标注文本中解析上下极限偏差
	 * @param dimText 标注文本
	 * @param tolUpper 传出上极限偏差
	 * @param tolLower 传出下极限偏差
	 */
	void parseDimensionTolerance(const AcString& dimText, double& tolUpper, double& tolLower);

	/**
	 * @brief 从多行文本中读取内容
	 * @param id 多行文本对象 ID
	 * @param text 读取到的文本内容
	 * @param isRawContent  true 获取包含控制代码的原始文本；false 获取解析文本（默认）
	 * @param pPos 获取文本坐标，传入 nullptr 则不获取
	 * @return true 成功；false 失败
	 */
	bool readMText(const AcDbObjectId& id, AcString& text, bool isRawContent = false, AcGePoint3d* pPos = nullptr);

	/**
	 * @brief 从单行文本中读取内容
	 * @param id 单行文本对象 ID
	 * @param text 读取到的文本内容
	 * @param isRawContent true 获取包含控制代码的原始文本；false 获取解析文本（默认）
	 * @param pPos 获取文本坐标，传入 nullptr 则不获取
	 * @return true 成功；false 失败
	 */
	bool readDText(const AcDbObjectId& id, AcString& text, bool isRawContent = false, AcGePoint3d* pPos = nullptr);

	/**
	 * @brief 读取多行/单行文本内容，可读取字段代码
	 * @param id 文本对象 ID
	 * @param text 文本内容
	 * @return true 成功；false 失败
	 */
	bool readTextField(const AcDbObjectId& id, AcString& text);

	/**
	 * @brief 通过包含控制代码的字符串获取解析文本
	 * @param text 传入传出字符串
	 */
	void resolveControlCodes(AcString& text);

	/**
	 * @brief 获取被选中的多行文本或单行文本的带控制字符文本
	 * @param content 传出带控制字符的文本
	 * @return true 成功；false 失败
	 */
	bool getSelectedTextRawContent(AcString& content);

	/**
	 * @brief 更新文本内容
	 * @param id 文本对象
	 * @param content 文本内容
	 */
	void updateTextEntityContent(const AcDbObjectId& id, const AcString& content);

	/**
	 * @brief 创建注释性多行文本矩阵（表格排布）。图纸文字高度为 3.5，实际显示模型文字高度= 3.5 * 注释比例缩放值。
	 * @param colWidth     矩阵列宽，即单个多行文本的长度。实际显示长度= 列宽 * 注释比例缩放值
	 * @param colStep	   列步长（小于“矩阵列宽 * 注释比例缩放值”时会重叠）
	 * @param rowStep      行步长（小于 “3.5 * 注释比例缩放值”时会重叠）
	 * @param matrixData   矩阵文本数据
	 * @param topLeftPt    左上角起始基准点
	 * @param dLineSpacingFactor 行距比例。默认值 1，不可小于 0.25，不可大于 4.0。实际行间距 = 文字高度 x 1.6667 x 行距比例。
	 */
	void createMTextMatrix(double colWidth, double colStep, double rowStep, const CsvModule::AcStringMatrix& matrixData, AcGePoint3d topLeftPt, double dLineSpacingFactor = 1.0);

	/**
	 * @brief 将零散的文本数据根据空间坐标转换为结构化的 AcString 二维矩阵
	 * * @details 算法逻辑：
	 * 1. 纵向分组：根据 Y 坐标和 yTol 将实体划分为不同的行。
	 * 2. 提取基准：分析所有行中出现的 X 坐标，利用 xTol 合并相近的 X 轴基准线以确定总列数。
	 * 3. 矩阵填充：将实体填入对应的行与列索引中，缺失位置保留为空字符串，确保导出 CSV 时行列对齐。
	 * * @param elements [in]  预先采集的文本实体数据集合（包含 ID、位置及内容）
	 * @param xTol     [in]  列判定容差。用于处理对齐偏差，建议设为文字平均高度的 1.0~1.5 倍
	 * @param yTol     [in]  行判定容差。建议设为文字平均高度的 0.6 倍
	 * @param matrix   [out] 输出的二维矩阵引用。内部使用 std::move 优化以减少拷贝开销
	 */
	void structureTextToAcStringMatrix(const TextUtil::TextEntityDataList& elements, double xTol, double yTol, CsvModule::AcStringMatrix& matrix);
}
