/**
 * @file      Common.ixx
 * @brief     通用工具模块，提供 AutoCAD 开发的基础函数库与全局常量。
 * @details   本模块整合了对象访问、几何运算、字符处理、环境配置及 UI 辅助等核心工具。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
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

	/**
	 * @brief 解析 CString 字符串并执行自定义校验逻辑
	 * @tparam T 目标数值类型
	 * @tparam Validator 校验器类型（通常为 Lambda 表达式）
	 * @param strInput 输入的 MFC CString 字符串
	 * @param expectedSize 期望解析到的有效数值数量
	 * @param validator 用于校验每个数值的 Lambda 或函数对象
	 * @param outResults 存储最终解析结果的容器引用
	 * @return 解析成功且通过所有校验返回 true，否则返回 false
	 */
	template <typename T, typename Validator>
	bool parse(const CString& strInput, size_t expectedSize, Validator validator, std::vector<T>& outResults);
}

// 定义在 Common.cpp
export namespace Common
{
	/**
	 * @brief 将double值转换为AcString格式的字符串
	 * @param doubleValue 要转换的double值
	 * @param AcStringValue 转换后的AcString字符串
	 * @param precision 小数点精度，默认为3位
	 * @param forcePlusSign 是否强制显示正号，默认为false
	 */
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision = 3, bool forcePlusSign = false);

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

	/**
	 * @brief 获取实体的几何中心点
	 * @param pEnt 指向 AutoCAD 实体的指针
	 * @param pCenter [out] 用于接收几何中心点的指针（WCS 坐标）
	 * @return true 成功获取坐标；false 获取失败，pCenter 不被修改
	 *
	 * @details 该方法通过获取实体的最小包围框（Geometric Extents）来计算中心。
	 * 相比于文字的插入点，几何中心点更能反映物体在空间中的视觉核心位置，
	 * 且不受文字对齐方式（Alignment）或块基点偏移的影响。
	 */
	bool getEntityCenter(AcDbEntity* pEnt, AcGePoint3d* pCenter);

	/**
	 * @brief 获取 TEXTSIZE 变量值（默认文字高度）
	 * @return TEXTSIZE 值，返回负数代表失败
	 */
	double getTEXTSIZE();

	/**
	 * @brief 获取当前图纸文件路径或目录
	 * @param bDirectory [in] true 表示获取所在目录，false 表示获取完整路径文件名
	 * @return 当前图纸文件路径或目录，失败返回空字符串
	 */
	CString getCurrPath(bool bDirectory = false);

	/**
	 * @brief 获取当前时间戳字符串
	 * @return 当前时间戳字符串
	 */
	CString getTimestamp();

	/**
     * @brief 获取指定坐标点处的相邻实体
     * 
     * 使用 AutoCAD Crossing (C) 模式在指定点进行空间检索。
     * 检索逻辑会将与该点接触或包含该点的所有符合过滤条件的实体纳入结果集。
     * 
     * @param pt      检索的基准坐标点（世界坐标系 WCS）。
     * @param pFilter 实体过滤器链表（resbuf），用于限制检索范围（如仅检索直线、圆弧等）。
     *                注意：调用者负责该过滤器的创建，本函数内部不释放该指针。
     * @return AcDbObjectIdArray 检索到的实体 ObjectId 集合。若未找到任何匹配实体，则返回空数组。
     * 
     * @note 1. 内部调用 acedSSGet(L"C", ...)，受当前视图显示范围及图层锁定状态影响。
     * @note 2. 检索结果包含所有重叠在 pt 处的实体，不区分实体的几何起始或终点。
     */
	AcDbObjectIdArray getNeighborsAtPoint(const AcGePoint3d& pt, resbuf* pFilter);

	/**
	 * @brief 获取并确保 AppData 下的插件专用目录
	 * @return std::optional<std::filesystem::path> 成功返回目录路径，失败返回 nullopt
	 */
	std::optional<std::filesystem::path> getAppSubFolder();

	/**
	 * @brief 获取数据库实体的实际显示颜色索引 (ACI)
	 * * @details 该函数会解析实体的颜色属性。如果实体颜色为 ByLayer，则返回其所属图层的颜色；
	 * 如果为 ByBlock 或其他特殊情况，则回退到 AutoCAD 默认颜色（索引 7）。
	 * * @param pEntity 指向要查询的 AutoCAD 实体的指针
	 * @return Adesk::UInt16 返回 1-255 之间的颜色索引值，默认返回 7 (黑/白)
	 */
	Adesk::UInt16 getEntityActualColorIndex(const AcDbEntity* pEntity);
}

// 常量
export namespace Common
{
	// 配置
	namespace Config
	{
		constexpr const wchar_t* configName = L"config.json";
	};

	// 符号代码
	namespace SymbolCodes
	{
		constexpr const wchar_t* Diameter = L"%%C";
		constexpr const wchar_t* PlusMinus = L"%%P";
		constexpr const wchar_t* Degree = L"%%D";
		constexpr const wchar_t* Radius = L"R";
	}

	// 极小值
	constexpr double Epsilon = 1e-6;

	// 单位模式
	enum UnitMode
	{
		Scientific = 1, // 科学计数
		Decimal = 2, // 十进制
		Engineering = 3, // 工程，英寸和十进制英寸
		Architectural = 4, // 建筑，英寸和分数英寸
		Fractional = 5 // 分数
	};

	// 尺寸值的占位符
	constexpr const wchar_t* measValuePlaceholder = L"<>";

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