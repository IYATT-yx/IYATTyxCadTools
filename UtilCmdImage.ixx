/**
 * @file      UtilCmdImage.ixx
 * @brief     光栅图片命令
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilCmdImage;
import FrameworkCommands;
import UiUniversalPicker;
import UiFileDialog;
import UtilImage;
import UtilSystem;
import FrameworkTranslator;

namespace
{
    void cmdPasteClipImage()
    {
        if (!UtilImage::clipboardHasImage())
        {
            AfxMessageBox(_(L"剪贴板中没有检测到图像数据。"), MB_OK | MB_ICONWARNING);
            return;
        }

        UiFileDialog::FileDialogFilterBuilder filterBuilder;
        CString fileFilter = filterBuilder.addFilter(_(L"PNG 图片"), { L"*.png" }).build();
        CString defaultFilename;
        defaultFilename.Format(_(L"图片%s.png"), UtilSystem::getTimestamp());
        CString filename = UiFileDialog::ShowSaveFileDialog(_(L"选择图片保存路径"), defaultFilename, L"png", fileFilter, UtilSystem::getCurrPath(true));
        if (filename.IsEmpty())
        {
            acutPrintf(_(L"取消操作"));
            return;
        }
        if (!UtilImage::saveClipboardBitmapToFile(filename))
        {
            AfxMessageBox(_(L"保存剪贴板图像数据到文件失败"), MB_OK | MB_ICONERROR);
            return;
        }
        if (!UtilImage::copyFileToClipboard(filename))
        {
            AfxMessageBox(_(L"复制文件到剪贴板失败"), MB_OK | MB_ICONERROR);
            return;
        }
        const wchar_t* appName = acedGetAppName();
        FrameworkCommands::CommandList pszCmdList =
        {
            L"PASTECLIP"
        };
        FrameworkCommands::executeCommand(pszCmdList);
    }

    void cmdForceRemoveImage()
    {
        UiUniversalPicker::AcRxClassVector arcv = { AcDbRasterImage::desc() };
        UiUniversalPicker::run(
            &arcv,
            UtilImage::forceRemoveImageAndFile,
            _(L"删除光栅图像及图片文件（无法撤销恢复）"),
            UiUniversalPicker::SelectMode::Immediate,
            false,
            UiUniversalPicker::SortMode::None,
            true
        );
    }

    FrameworkCommands::AutoRegister ar =
    {
        { L"yxPasteClipImage", []() { return _(L"将剪贴板中的截图/图像数据保存到文件并插入图纸中"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdPasteClipImage },
        { L"yxForceRemoveImage", []() { return _(L"删除光栅图像及图片文件（无法撤销恢复）"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdForceRemoveImage }
    };
}