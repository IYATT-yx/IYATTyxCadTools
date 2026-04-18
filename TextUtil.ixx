module;
#include "StdAfx.h"

export module TextUtil;
import UniversalPicker;

export namespace TextUtil
{
	// 文本类型列表
	UniversalPicker::AcRxClassVector textClassList =
	{
		AcDbMText::desc(),
		AcDbText::desc()
	};
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
	 * @brief 从标注中读取MText内容
	 * @param id 对象 ID
	 * @param text 内容文本
	 * @param isRawContent true 获取带控制字符的原始内容；false 获取纯文本内容
	 * @return true 成功；false 失败
	 */
	bool readMText(const AcDbObjectId& id, AcString& text, bool isRawContent = false);

	/**
	 * @brief 从标注中读取DText内容
	 * @param id 对象 ID
	 * @param text 内容文本
	 * @param isRawContent true 获取带控制字符的原始内容；false 获取纯文本内容
	 * @return true 成功；false 失败
	 */
	bool readDText(const AcDbObjectId& id, AcString& text, bool isRawContent = false);

	/**
	 * @brief 解析控制字符得到纯文本
	 * @param text 传带控制字符的文本，传出纯文本
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
	 * @param matrixData   矩阵文本数据 (vector<vector<AcString>>)
	 * @param topLeftPt    左上角起始基准点
	 * @param dLineSpacingFactor 行距比例。默认值 1，不可小于 0.25，不可大于 4.0。实际行间距 = 文字高度 x 1.6667 x 行距比例。
	 */
	void createMTextMatrix(double colWidth, double colStep, double rowStep, const std::vector<std::vector<AcString>>& matrixData, AcGePoint3d topLeftPt, double dLineSpacingFactor = 1.0);
}
