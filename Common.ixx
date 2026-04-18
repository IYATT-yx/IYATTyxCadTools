module;
#include "StdAfx.h"

export module Common;
export import :Inline;

import UniversalPicker;

// 定义在 Common-Inline.ixx
export namespace Common
{
	/**
	 * @brief 通过对象ID获取对象指针。对象指针自动释放，不能调用close。
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
export namespace Common
{
	/**
	 * @brief 将double值转换为AcString格式的字符串
	 * @param doubleValue 要转换的double值
	 * @param AcStringValue 转换后的AcString字符串
	 * @param precision 小数点精度，默认为3位
	 */
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision = 3);

	/**
	 * @brief 设置字符映射表的启动选中字体为 GDT
	 * @return 设置成功返回true，否则返回false
	 */
	bool setCharMapFontToGDT();

	/**
	 * @brief 启动 charmap 字符映射表并默认选中 GDT 字体
	 */
	void startCharMapWithGDT();

	/**
	 * @brief 将输入字符串用 GDT 字体包裹
	 * @param input 要包裹的字符串
	 * @return 包裹后的字符串
	 */
	AcString wrapWithGdtFont(const wchar_t* input);

	/**
	 * @brief 打印指定对象ID的类层次结构
	 * @param objId 对象ID
	 */
	void printClassHierarchy(const AcDbObjectId& objId);

	/**
	 * @brief 读取 String Table 中的字符串
	 * @param nID String Table 中的字符串 ID
	 * @return 字符串
	 */
	CString loadString(UINT nID);
}

// 常量
export namespace Common
{
	constexpr double defaultTextHeight = 3.5; // 全局默认文本高度

	// 符号代码
	namespace SymbolCodes
	{
		constexpr const wchar_t* Diameter = L"%%c";
		constexpr const wchar_t* PlusMinus = L"%%p";
		constexpr const wchar_t* Degree = L"%%d";
		constexpr const wchar_t* Radius = L"R";
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
	constexpr const wchar_t* ACDB_DIM_TEXT_DEFAULT = L"<>";

	// 启动 charmap 字符映射表使用的常量
	namespace CharMap
	{
		constexpr const WCHAR* path = L"Software\\Microsoft\\CharMap";
		constexpr const WCHAR* key = L"Font";
		constexpr const WCHAR* font = L"GDT";
		constexpr const WCHAR* programName = L"charmap.exe";
	}

	// 序号圆
	namespace BalloonNumberBlock
	{
		constexpr const wchar_t* blockName = L"气泡号块";
		constexpr const wchar_t* AttTag = L"序号";
		constexpr const wchar_t* AttPrompt = L"输入序号：";
		constexpr double defaultCircleRadius = Common::defaultTextHeight;
		constexpr double defaultTextHeight = Common::defaultTextHeight;
	}

	// 标注类型的子类
	UniversalPicker::AcRxClassVector DimensionSubClasses =
	{
		AcDb2LineAngularDimension::desc(),
		AcDb3PointAngularDimension::desc(),
		AcDbAlignedDimension::desc(),
		AcDbArcDimension::desc(),
		AcDbDiametricDimension::desc(),
		AcDbOrdinateDimension::desc(),
		AcDbRadialDimension::desc(),
		AcDbRadialDimensionLarge::desc(),
		AcDbRotatedDimension::desc()
	};
}