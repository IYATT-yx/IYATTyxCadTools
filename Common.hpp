#pragma once
#include "Common.inl"

// 定义在 Common.inl
namespace Common
{
	/**
	 * @brief 通过对象ID获取对象指针
	 * @tparam T 目标对象类型
	 * @param objId 对象ID
	 * @param mode 打开模式，默认为只读
	 * @return 对象指针，如果获取失败则返回nullptr
	 */
	template <typename T>
	T* getObject(const AcDbObjectId objId, AcDb::OpenMode mode = AcDb::kForRead);

	/**
	 * @brief 角度转弧度
	 * @param degrees 角度值
	 * @return 弧度值
	 */
	constexpr double deg2rad(double degrees);

	/**
	 * @brief 弧度转角度
	 * @param radians 弧度值
	 * @return 角度值
	 */
	constexpr double rad2deg(double radians);
}

// 定义在 Common.cpp
namespace Common
{
	/**
	 * @brief 将double值转换为AcString格式的字符串
	 * @param doubleValue 要转换的double值
	 * @param AcStringValue 转换后的AcString字符串
	 * @param precision 小数点精度，默认为3位
	 */
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision = 3);
}

// 常量
namespace Common
{
	// 标注符号
	namespace DimSymbol
	{
		constexpr const ACHAR* Diameter = L"%%c";
		constexpr const ACHAR* PlusMinus = L"%%p";
		constexpr const ACHAR* Degree = L"%%d";
		constexpr const ACHAR* Radius = L"R";
	}

	// 单位模式
	enum UnitMode
	{
		Scientific = 1, // 科学计数
		Decimal = 2, // 十进制
		Engineering = 3, // 工程，英寸和十进制英寸
		Architectural = 4, // 建筑，英寸和分数英寸
		Fractional = 5 // 分数
	};

	// 标注的默认尺寸文本占位符
	constexpr const ACHAR* ACDB_DIM_TEXT_DEFAULT = L"<>";
}