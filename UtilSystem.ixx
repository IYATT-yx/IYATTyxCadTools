/**
 * @file      UtilSystem.ixx
 * @brief     系统集成模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilSystem;
import std;

export namespace UtilSystem
{
	/**
	 * @brief 获取并确保 AppData 下的插件专用目录
	 * @return std::optional<std::filesystem::path> 成功返回目录路径，失败返回 nullopt
	 */
	std::optional<std::filesystem::path> getAppSubFolder();

	/**
	 * @brief 获取当前时间戳字符串
	 * @return 当前时间戳字符串
	 */
	CString getTimestamp();

	/**
	 * @brief 获取当前图纸文件路径或目录
	 * @param bDirectory [in] true 表示获取所在目录，false 表示获取完整路径文件名
	 * @return 当前图纸文件路径或目录，失败返回空字符串
	 */
	CString getCurrPath(bool bDirectory = false);

	/**
	 * @brief 设置字符映射表的启动选中字体为 GDT
	 * @return 设置成功返回true，否则返回false
	 */
	bool setCharMapFontToGDT();

	/**
	 * @brief 启动 charmap 字符映射表并默认选中 GDT 字体
	 */
	void startCharMapWithGDT();

}