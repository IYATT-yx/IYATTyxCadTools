module;
#include "StdAfx.h"

export module TextUtil;

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
	 * @param isRawContents true 获取带控制字符的原始内容；false 获取纯文本内容
	 * @return true 成功；false 失败
	 */
	bool readMText(AcDbObjectId& id, AcString& text, bool isRawContents = false);

	/**
	 * @brief 从标注中读取DText内容
	 * @param id 对象 ID
	 * @param text 内容文本
	 * @param isRawContents true 获取带控制字符的原始内容；false 获取纯文本内容
	 * @return true 成功；false 失败
	 */
	bool readDText(AcDbObjectId& id, AcString& text, bool isRawContents = false);
}
