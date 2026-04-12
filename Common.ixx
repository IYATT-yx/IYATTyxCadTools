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
	AcString wrapWithGdtFont(const ACHAR* input);

	/**
	 * @brief 打印指定对象ID的类层次结构
	 * @param objId 对象ID
	 */
	void printClassHierarchy(AcDbObjectId objId);

	/**
	 * @brief 弹出保存文件对话框
	 * @param title 对话框标题，默认 "保存文件"
	 * @param defExt 默认扩展名，默认 "csv"
	 * @param filter 文件类型过滤器，格式示例: "CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"
	 * @return 用户选择的完整文件路径，如果取消返回空字符串
	 */
	CString ShowSaveFileDialog(const CString& title = L"保存文件", const CString& defaultName = L"标注数据.csv", const CString& defExt = L"csv", const CString& filter = L"CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
}

// 常量
export namespace Common
{
	// 符号代码
	namespace SymbolCodes
	{
		constexpr const ACHAR* Diameter = L"%%c";
		constexpr const ACHAR* PlusMinus = L"%%p";
		constexpr const ACHAR* Degree = L"%%d";
		constexpr const ACHAR* Radius = L"R";
	}

	// 符号
	namespace Symbols
	{
		constexpr const ACHAR* Diameter = L"⌀";
		constexpr const ACHAR* PlusMinus = L"±";
		constexpr const ACHAR* Degree = L"°";
		constexpr const ACHAR* Radius = L"R";
	}

	// 控制代码
	const std::unordered_map<AcString, AcString> ControlCodeToSymbol =
	{
		{ L"%%c", L"⌀" }, { L"%%C", L"⌀" },  // Diameter
		{ L"%%p", L"±" }, { L"%%P", L"±" },  // PlusMinus
		{ L"%%d", L"°" }, { L"%%D", L"°" },  // Degree
		{ L"R",    L"R" },                     // Radius（通常只有一个大小写）
	};
	AcString getSymbol(const AcString& code)
	{
		auto it = ControlCodeToSymbol.find(code);
		if (it != ControlCodeToSymbol.end())
		{
			return it->second;
		}

		// 如果找不到（比如前缀是 "M"），则返回原始字符串
		return code;
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
		constexpr const ACHAR* blockName = L"气泡号块";
		constexpr const ACHAR* AttTag = L"序号";
		constexpr const ACHAR* AttPrompt = L"输入序号：";
		constexpr double defaultCircleRadius = 2.5;
		constexpr double defaultTextHeight = 2.5;
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