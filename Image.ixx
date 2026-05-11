/**
 * @file      Image.ixx
 * @brief     图像模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"

export module Image;

export namespace Image
{
	/**
	 * @brief 检查剪贴板中是否有图像数据
	 * @return true  剪贴板中有图像数据； false  剪贴板中没有图像数据
	 */
	bool clipboardHasImage();

	/**
	 * @brief 将剪贴板中的图像数据保存到文件
	 * @param filename 文件名
	 * @return true 保存成功； false 保存失败
	 */
	bool saveClipboardBitmapToFile(CString& filename);

	/**
	 * @brief 将指定文件路径写入剪贴板，模拟文件复制操作（CF_HDROP）。
	 * * @details 该函数使用 DROPFILES 结构体封装文件路径，使得用户可以在 Windows 资源管理器中
	 * 执行“粘贴”操作来移动或复制该文件。
	 * * @param filename [in] 要复制到剪贴板的文件全路径。
	 * @return 如果成功将路径设置到剪贴板则返回 true，否则返回 false。
	 */
	bool copyFileToClipboard(const CString& filename);

	/**
	 * @brief 删除光栅图像和引用的图片文件
	 * @param id 光栅图像 ID
	 */
	void forceRemoveImageAndFile(const AcDbObjectId& id);
};