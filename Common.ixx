module;
#include "StdAfx.h"

export module Common;
export import :Inline;

// 定义在 Common-Inline.ixx
export namespace Common
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
	 * @brief 获取命令的简名
	 * @param cmdName 命令全名
	 * @return 命令简名。yx 开头，加命令全名中的大写字母拼接。
	 */
	AcString getShortCommandName(const ACHAR* cmdName);

	/**
	 * @brief 注册命令
	 * @param cmdName 命令全名
	 * @param flags 命令标志
	 * @param proc 命令处理函数
	 */
	void registerYxCmd(const ACHAR* cmdName, int flags, AcRxFunctionPtr proc);
}

// 常量
export namespace Common
{
	// 命令组
	constexpr const ACHAR* commandGroup = L"IYATTyxCadTools";

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

	// 启动 charmap 字符映射表使用的常量
	namespace CharMap
	{
		constexpr const WCHAR* path = L"Software\\Microsoft\\CharMap";
		constexpr const WCHAR* key = L"Font";
		constexpr const WCHAR* font = L"GDT";
		constexpr const WCHAR* programName = L"charmap.exe";
	}

	// 序号圆
	namespace SerialNumberCircleBlock
	{
		constexpr const ACHAR* blockName = L"SerialNumberCircle";
		constexpr const ACHAR* AttTag = L"number";
		constexpr const ACHAR* AttPrompt = L"输入序号：";
		constexpr double defaultCircleRadius = 2.5;
		constexpr double defaultTextHeight = 2.5;
	}
}