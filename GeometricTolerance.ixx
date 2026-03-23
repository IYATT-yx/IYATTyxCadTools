module;
#include "StdAfx.h"
#include <map>

export module GeometricTolerance;

export namespace GeometricTolerance
{
	// 形位公差数据存储结构
	struct GeometricToleranceData
	{
		Adesk::UInt64 u64handle; // 在文件中的唯一句柄
		AcString name[3]; // 名称
		Acm::GdtSymbolType gdtSymbolType[3]; // 符号类型枚举值
		AcString gdtSymbol[3]; // 符号
		AcString value[3]; // 值
		AcString primary[3]; // 基准1
		AcString secondary[3]; // 基准2
		AcString tertiary[3]; // 基准3

		// 句柄转字符串
		AcString handleAsString() const
		{
			AcString str;
			str.format(L"%I64u", u64handle);
			return str;
		}
	};

	// 符号类型与名称的映射
	std::map<Acm::GdtSymbolType, const ACHAR*> gdtNameMap =
	{
		{Acm::kStraightness, L"直线度"},
		{Acm::kFlatness, L"平面度"},
		{Acm::kCircularity, L"圆度"},
		{Acm::kCylindricity, L"圆柱度"},
		{Acm::kProfileLine, L"线型轮廓"},
		{Acm::kProfileSurface, L"面型轮廓"},
		{Acm::kAngularity, L"角度"},
		{Acm::kPerpendicularity, L"垂直度"},
		{Acm::kParallelism, L"平行度"},
		{Acm::kPosition, L"位置度"},
		{Acm::kCoaxialityForAxes, L"同轴度"},
		{Acm::kConcentricityForCentrePoints, L"同心度"},
		{Acm::kSymmetry, L"对称度"},
		{Acm::kRunoutCircular, L"圆跳动"},
		{Acm::kRunoutTotal, L"全跳动"}
	};

	// 符号类型与 GDT 符号的映射
	std::map<Acm::GdtSymbolType, const ACHAR*> gdtSymbolMap =
	{
		{Acm::kStraightness, L"u"},
		{Acm::kFlatness, L"c"},
		{Acm::kCircularity, L"e"},
		{Acm::kCylindricity, L"g"},
		{Acm::kProfileLine, L"k"},
		{Acm::kProfileSurface, L"d"},
		{Acm::kAngularity, L"a"},
		{Acm::kPerpendicularity, L"b"},
		{Acm::kParallelism, L"f"},
		{Acm::kPosition, L"j"},
		{Acm::kCoaxialityForAxes, L"r"},
		{Acm::kConcentricityForCentrePoints, L"r"},
		{Acm::kSymmetry, L"i"},
		{Acm::kRunoutCircular, L"h"},
		{Acm::kRunoutTotal, L"t"}
	};
};

export namespace GeometricTolerance
{
	/**
	 * @brief 读取形位公差数据
	 * @param id 形位公差块表记录ID
	 * @param data 传出形位公差数据
	 */
	void readFcf(AcDbObjectId id, GeometricToleranceData& data);
}