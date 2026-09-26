/**
 * @file      UtilConstants.ixx
 * @brief     常量。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilConstants;
import std;
import FrameworkTranslator;

// 类型定义 - UiUniversalPicker 模块
export namespace UtilConstants
{
	// AcRxClass 类指针列表类型定义
	using AcRxClassVector = std::vector<AcRxClass*>;
	// AcRxClass 类指针列表指针类型定义
	using AcRxClassVectorPtr = AcRxClassVector*;
}

export namespace UtilConstants
{
	inline const wchar_t* getLocalProjectName()
	{
		static const wchar_t* name = _(L"AutoCAD Mechanical 2027 思能工具箱");
		return name;
	}

	constexpr const wchar_t* cmdGroup = L"IYATTyxCadToolsGlobalGroup";

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

	// 气泡号
	namespace BalloonNumberBlock
	{
		inline const wchar_t* getBlockName()
		{
			static const wchar_t* blockName = _(L"气泡号块");
			return blockName;
		}

		inline const wchar_t* getAttTag()
		{
			static const wchar_t* attTag = _(L"序号");
			return attTag;
		}

		inline const wchar_t* getAttPrompt()
		{
			static const wchar_t* attPrompt = _(L"输入序号：");
			return attPrompt;
		}
	}

	// 标注类型的子类
	UtilConstants::AcRxClassVector DimensionSubClasses =
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