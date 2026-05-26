/**
 * @file      Interface.cpp
 * @brief     接口模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"
#include "GenericPairEditDlg.hpp"
#include "MainBar.hpp"

module Interface;

import EntityStyle;
import BuildingTime;
import UniversalPicker;
import Dimension;
import Common;
import BalloonNumber;
import CsvModule;
import GeometricTolerance;
import TextUtil;
import ImeAutoSwitcher;
import Commands;
import FileDialog;
import Annotative;
import Image;
import LineUtil;
import PointUtil;
import ConfigManager;
import MiddleClickManager;
import AcadVarUtil;
import Translator;
import EncodingConverter;

void Interface::init()
{
    // 读取配置文件
    auto& manager = ConfigManager::getInstance();
    auto appPath = Common::getAppSubFolder();
    if (!appPath.has_value())
    {
        AfxMessageBox(_(L"获取程序数据目录失败"), MB_OK | MB_ICONERROR);
        return;
    }
    std::filesystem::path configPathObj = appPath.value() / Common::Config::configName;
    if (!manager.loadConfig(configPathObj.wstring()))
    {
        std::wstring err = manager.getLastError();
        AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
    }
    auto& config = manager.getConfig();

    // 初始化翻译器
    std::filesystem::path appName(acedGetAppName());
    std::filesystem::path appDir = appName.parent_path();
    std::filesystem::path localesDir = appDir / L"locales";
    auto& translator = Translator::getInstance();
    if (!translator.initialize(localesDir, config.languageSettings.languageCode))
    {
        AfxMessageBox(L"Initialize translator failed", MB_OK | MB_ICONERROR);
        return;
    }

    // 命令列表
    Commands::commandInfoList =
    {
        {L"yxSetByLayer", _(L"设置实体样式为当前层样式"), Commands::CommandFlags::PickRedraw, Interface::cmdSetByLayer},
        {L"yxChainSelection", _(L"选中实体后自动链式选择"), Commands::CommandFlags::PickRedraw, Interface::cmdChainSelection},
        {L"yxDimensionSolidify", _(L"尺寸固化"), Commands::CommandFlags::PickRedraw, Interface::cmdDimensionSolidify},
        {L"yxDimensionReslink", _(L"尺寸恢复关联"), Commands::CommandFlags::PickRedraw, Interface::cmdDimensionRelink},
        {L"yxDimensionTolerancePrecision", _(L"设置尺寸标注的主单位精度和公差精度"), Commands::CommandFlags::PickRedraw, Interface::cmdDimensionTolerancePrecision},
        {L"yxAddSurroundingCharsForDimension", _(L"为标注添加前后缀"), Commands::CommandFlags::PickRedraw, Interface::cmdAddSurroundingCharsForDimension},
        {L"yxRemoveSurroundingCharsForDimension", _(L"为标注移除前后缀"), Commands::CommandFlags::PickRedraw, Interface::cmdRemoveSurroundingCharsForDimension},
        {L"yxSetBasicBox", _(L"设置理论尺寸框"), Commands::CommandFlags::PickRedraw, Interface::cmdSetBasicBox},
        {L"yxUnsetBasicBox", _(L"取消理论尺寸框"), Commands::CommandFlags::PickRedraw, Interface::cmdUnsetBasicBox},
        {L"yxSetRefDim", _(L"设置参考尺寸括号"), Commands::CommandFlags::PickRedraw, Interface::cmdSetRefDim},
        {L"yxUnsetRefDim", _(L"取消参考尺寸括号"), Commands::CommandFlags::PickRedraw, Interface::cmdUnsetRefDim},
        {L"yxInsertBalloonNumberBlockWithStartNumber", _(L"插入带起始编号的气泡号"), Commands::CommandFlags::Base, Interface::cmdInsertBalloonNumberBlockWithStartNumber},
        {L"yxUpdateBalloonNumberBlock", _(L"更新气泡号"), Commands::CommandFlags::PickRedraw, Interface::cmdUpdateBalloonNumberBlock},
        {L"yxBalloonNumberOffset", _(L"气泡号偏置"), Commands::CommandFlags::PickRedraw, Interface::cmdBalloonNumberOffset},
        {L"yxBalloonNumberFilter", _(L"气泡号筛选"), Commands::CommandFlags::PickRedraw, Interface::cmdBalloonNumberFilter},
        {L"yxCheckBalloonNumberMaxMin", _(L"查找气泡号最大和最小序号"), Commands::CommandFlags::PickRedraw, Interface::cmdCheckBalloonNumberMaxMin},
        {L"yxCheckDuplicateBalloonNumbers", _(L"检查重复气泡号"), Commands::CommandFlags::PickRedraw, Interface::cmdCheckDuplicateBalloonNumbers},
        {L"yxCheckBalloonNumberBreakpoints", _(L"检查气泡号断点"), Commands::CommandFlags::PickRedraw, Interface::cmdCheckBalloonNumberBreakpoints},
        {L"yxExtractAnnotations", _(L"提取标注到CSV文件"), Commands::CommandFlags::Base, Interface::cmdExtractAnnotations},
        {L"yxCloneText", _(L"将多行/单行文本内容复制给其它多行/单行文本"), Commands::CommandFlags::Base, Interface::cmdCloneText},
        {L"yxIntersect", _(L"将两条直线延伸或修剪至其交点"), Commands::CommandFlags::Base, Interface::cmdIntersect},
        {L"yxImportCsvToMTextMatrix", _(L"从 CSV 文件导入数据生成多行文本矩阵"), Commands::CommandFlags::PickRedraw, Interface::cmdImportCsvToMTextMatrix},
        {L"yxSpatialTableExplorer", _(L"将多行/单行文本按空间位置表格化导出到 CSV 文件"), Commands::CommandFlags::PickRedraw, Interface::cmdSpatialTableExplorer},
        {L"yxPasteClipImage", _(L"将剪贴板中的截图/图像数据保存到文件并插入图纸中"), Commands::CommandFlags::PickRedraw, Interface::cmdPasteClipImage},
        {L"yxForceRemoveImage", _(L"删除光栅图像及图片文件（无法撤销恢复）"), Commands::CommandFlags::PickRedraw, cmdForceRemoveImage},
        {L"yxLocateDrawing", _(L"打开当前图纸文件"), Commands::CommandFlags::Base, Interface::cmdLocateDrawing},
        {L"yxCreateIntersectionPoints", _(L"创建两条线(及延长线)的交点。可使用PTYPE设置点样式。"), Commands::CommandFlags::Base, Interface::cmdCreateIntersectionPoints},
        {L"yxImeAutoSwitch", _(L"设置输入法自动切换"), Commands::CommandFlags::Base, Interface::cmdImeAutoSwitch},
        {L"yxDialogMiddleClickToOk", _(L"设置对话框中鼠标中键映射到确定按钮"), Commands::CommandFlags::Base, Interface::cmdDialogMiddleClickToOk},
        {L"yxCmdMiddleClickToEnter", _(L"设置命令执行状态下鼠标中键映射回车键"), Commands::CommandFlags::Base, Interface::cmdCmdMiddleClickToEnter},
        {L"yxSetLanguage", _(L"设置语言"), Commands::CommandFlags::Base, Interface::cmdSetLanguage},
        {L"yx", _(L"显示或隐藏命令菜单"), Commands::CommandFlags::Base, Interface::cmdYx},
        {L"yxTest", _(L"开发用测试命令"), Commands::CommandFlags::Base, Interface::test},
        {L"yxUnload", _(L"关闭本插件"), Commands::CommandFlags::Base, Interface::cmdUnloadApp},
        {L"yxRestart", _(L"重启本插件"), Commands::CommandFlags::Base, Interface::cmdRestartApp},
        {L"yxPrintClassHierarchy", _(L"打印类层次结构"), Commands::CommandFlags::Base, Interface::cmdPrintClassHierarchy},
        {L"yxLocateSelf", _(L"打开本工具文件"), Commands::CommandFlags::Base, Interface::cmdLocateSelf},
        {L"yxPrintConfigFilename", _(L"打开配置文件"), Commands::CommandFlags::Base, cmdPrintConfigFilename}
    };

    Interface::info();

    // 注册命令
    Commands::registerYxCmds(Commands::commandInfoList);

    // 输入法语言自动切换
    if (config.imeSettings.bEnabled)
    {
        ImeAutoSwitcher::start(config.imeSettings.iIntervalMs);
    }

    // 中键处理
    MiddleClickManager::getInstance().startUnifiedMiddleClickProc(config.middleClickManagerSettings);

    // 显示命令报表悬浮窗
    MainBar::showBar(Commands::commandInfoList);
}

// 测试使用
void Interface::test()
{
}

void Interface::unload()
{
    // 关闭输入法自动切换
    ImeAutoSwitcher::stop();
    // 关闭中键处理
    MiddleClickManager::getInstance().stopUnifiedMiddleClickProc();
    // 关闭命令菜单
    MainBar::terminateBar();
    // 卸载命令
	acedRegCmds->removeGroup(Common::cmdGroup);
    acutPrintf(_(L"\n已卸载 %s"), Common::getLocalProjectName());
}

void Interface::cmdUnloadApp()
{
    const wchar_t* appName = acedGetAppName();
    Commands::CommandList pszCmdList =
    {
        L"ARX",
        L"U",
        appName
    };
    Commands::executeCommand(pszCmdList);
}

void Interface::cmdRestartApp()
{
    std::wstring lispPath = std::filesystem::path(acedGetAppName()).generic_wstring();
    Interface::unload();
    std::wstring lispCmd = L"(progn (arxunload \"" + lispPath + L"\" nil) (arxload \"" + lispPath + L"\")(princ))";
    Commands::CommandList pszCmdList =
    {
        lispCmd.c_str()
    };
    Commands::executeCommand(pszCmdList, false);
}

void Interface::info()
{
    
    acutPrintf(L"\n%s %s_%s | %s: IYATT-yx | %s: MIT | %s: https://github.com/IYATT-yx/IYATTyxCadTools\n",
        Common::getLocalProjectName(),
        BuildingTime::WDATE, BuildingTime::WTIME,
        _(L"作者"),
        _(L"开源协议"),
        _(L"项目地址")
        );
}

void Interface::cmdYx()
{
    Interface::info();
    MainBar::showBar(Commands::commandInfoList);
}

void Interface::cmdSetByLayer()
{
    UniversalPicker::run(nullptr, EntityStyle::setByLayer, _(L"设置实体样式为当前层样式"));
}

void Interface::cmdDimensionSolidify()
{
    UniversalPicker::run(&Common::DimensionSubClasses, Dimension::dimensionSolidify, _(L"尺寸固化"));
}

void Interface::cmdDimensionRelink()
{
    UniversalPicker::run(&Common::DimensionSubClasses, Dimension::dimensionRelink, _(L"尺寸恢复关联"));
}

void Interface::cmdAddSurroundingCharsForDimension()
{
    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(_(L"为标注添加前后缀"), _(L"前缀符号"), _(L"后缀符号"));

    CString left, right;
    dlg.setValidatorAndParser([&](const CString& value1, const CString& value2) -> CString
        {
            left = value1;
            right= value2;
            return GenericPairEditDlg::ValidatorOk;
        });
    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [&](AcDbObjectId objId)
        {
            Dimension::addSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
        },
        _(L"为标注添加前后缀"),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdRemoveSurroundingCharsForDimension()
{
    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(_(L"为标注移除前后缀"), _(L"前缀符号"), _(L"后缀符号"));

    CString left, right;
    dlg.setValidatorAndParser([&](const CString& value1, const CString& value2) -> CString
        {
            left = value1;
            right = value2;
            return GenericPairEditDlg::ValidatorOk;
        });
    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [&](AcDbObjectId objId)
        {
            Dimension::removeSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
        },
        _(L"为标注移除前后缀"),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdSetBasicBox()
{
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetBasicBox(objId, true);
        },
        _(L"设置理论尺寸框"),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdUnsetBasicBox()
{
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetBasicBox(objId, false);
        },
        _(L"取消理论尺寸框"),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdSetRefDim()
{
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, true);
        },
        _(L"设置参考尺寸括号"),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdUnsetRefDim()
{
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, false);
        },
        _(L"取消理论尺寸括号"),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdInsertBalloonNumberBlockWithStartNumber()
{
    CAcModuleResourceOverride resOverride;
    CString title = _(L"插入带起始编号的气泡号");
    acutPrintf(L"\n%s\n", title);

    GenericPairEditDlg dlg(title, _(L"开始序号"), _(L"使用提示"), false, true, true);
    // 设置默认字高
    CString csTips;
    double TEXTSIZE;
    if (!AcadVarUtil::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
    {
        AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
        return;
    }
    double scale = Annotative::getCurrentScaleValue();
    csTips.Format(_(L"显示序号高度 = TEXTSIZE变量值%g × 注释比例缩放值%g = %g"), TEXTSIZE, scale, TEXTSIZE * scale);
    dlg.modifyEditControl(L"", csTips);

    int startNumber;
    dlg.setValidatorAndParser([&](const CString& strVal, const CString& _2) -> CString
        {
            try
            {
                size_t pos;
                startNumber = std::stoi(strVal.GetString(), &pos);
                if (pos != strVal.GetLength())
                {
                    throw std::exception();
                }
                if (startNumber < 0)
                {
                    throw std::exception();
                }
            }
            catch (...)
            {
                return _(L"开始序号必须为不小于 0 的整数");
            }
            return GenericPairEditDlg::ValidatorOk;
        });

    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    BalloonNumber::createBalloonNumberBlock();
    BalloonNumber::insertBalloonNumberBlockWithStartNumber(startNumber);
}

void Interface::cmdPrintClassHierarchy()
{
    UniversalPicker::run(nullptr, Common::printClassHierarchy, _(L"打印类层次结构"), UniversalPicker::SelectMode::Immediate, true);
}

void Interface::cmdExtractAnnotations()
{
    FileDialog::FileDialogFilterBuilder fileFilterBuilder;
    CString strFileFilter = fileFilterBuilder.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
    CString filePath = FileDialog::ShowSaveFileDialog(_(L"保存 CSV 文件到"), _(L"数据文件.csv"), L"csv", strFileFilter);
    if (filePath.IsEmpty())
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    CsvWriter csv(filePath);
    if (!csv.isValid())
    {
        AfxMessageBox(_(L"文件路径打开失败，请检查是否被占用或路径无效"), MB_OK | MB_ICONERROR);
        return;
    }

    UniversalPicker::AcRxClassVector filter = { AcmFCF::desc(), AcDbMText::desc(), AcDbText::desc()};
    filter.insert(filter.end(), Common::DimensionSubClasses.begin(), Common::DimensionSubClasses.end());
    UniversalPicker::run(
        &filter,
        [&csv](AcDbObjectId objId)
        {
            Dimension::DimensionData dimData{};
            Dimension::readDim(objId, dimData);
            GeometricTolerance::GeometricToleranceData gtData{};
            GeometricTolerance::readFcf(objId, gtData);
            AcString asMText, asDText;

            if (dimData.status) // 尺寸
            {
                // 名义值
                AcString asMeasuredValue;
                double dMeasuredValue = dimData.dimensionValue();
                Common::double2AcString(dMeasuredValue, asMeasuredValue, dimData.precision);
                
                //acutPrintf(L"\n调试：%s %s %f %f ", dimData.prefix.constPtr(), dimData.suffix.constPtr(), dimData.tolUpper, dimData.tolLower);
                // 公差
                AcString asTol,asTolUpper, asTolLower;
                if (dimData.upperDeviation == 0 && dimData.lowerDeviation == 0) // 无公差
                {
                    asTol = L"";
                    asTolUpper = L"0";
                    asTolLower = L"0";
                }
                else
                {
                    if (abs(dimData.upperDeviation + dimData.lowerDeviation) < 1e-6) // 对称偏差
                    {
                        double dAbsTol = abs(dimData.upperDeviation);
                        asTol.format(L"%s%.*f", Common::SymbolCodes::PlusMinus, dimData.tolerancePrecision, dAbsTol);
                        asTolUpper.format(L"%.*f", dimData.tolerancePrecision, dAbsTol);
                        asTolLower.format(L"-%.*f", dimData.tolerancePrecision, dAbsTol);
                    }
                    else // 极限偏差
                    {
                        
                        Common::double2AcString(dimData.upperDeviation, asTolUpper, dimData.tolerancePrecision);
                        Common::double2AcString(dimData.lowerDeviation, asTolLower, dimData.tolerancePrecision);
                        asTol.format(L"+%s/%s", asTolUpper.constPtr(), asTolLower.constPtr());
                        asTolUpper.format(L"%.*f", dimData.tolerancePrecision, dimData.upperDeviation);
                        asTolLower.format(L"%.*f", dimData.tolerancePrecision, dimData.lowerDeviation);
                    }
                }

                AcString asDimText = dimData.prefix + asMeasuredValue + asTol + dimData.suffix;
                TextUtil::resolveControlCodes(asDimText);
                acutPrintf(_(L"\n尺寸：%s"), asDimText.constPtr());
                std::vector<AcString> row = { dimData.text, asDimText, asMeasuredValue, asTolUpper, asTolLower }; // 完整尺寸内容、尺寸文本、名义值、上极限偏差、下极限偏差
                csv.writeRow(row);
            }
            else if (gtData.status) // 几何公差
            {
                for (int i = 0; i < GeometricTolerance::GeometricToleranceDataLen; ++i)
                {
                    GeometricTolerance::GeometricToleranceRow row = gtData.rows[i];
                    if (row.gdtSymbolType != Acm::kNoType)
                    {
                        AcString name = row.name;
                        AcString value = row.value;
                        AcString primary = row.primary;
                        AcString secondary = row.secondary;
                        AcString tertiary = row.tertiary;
                        AcString strRow = row.toString();
                        acutPrintf(_(L"\n几何公差：%s"), strRow.constPtr());
                        std::vector<AcString> asvRow = { strRow, name, value, primary, secondary, tertiary };
                        csv.writeRow(asvRow);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else if (TextUtil::readMText(objId, asMText))
            {
                acutPrintf(_(L"\n多行文本：%s"), asMText.constPtr());
                std::vector<AcString> rows = { asMText };
                csv.writeRow(rows);
            }
            else if (TextUtil::readDText(objId, asDText))
            {
                acutPrintf(_(L"\n单行文本：%s"), asDText.constPtr());
                std::vector<AcString> rows = { asDText };
                csv.writeRow(rows);
            }
        },
        _(L"提取标注到CSV文件"),
        UniversalPicker::SelectMode::Immediate,
        false,
        UniversalPicker::SortMode::RD,
        false
    );
}

void Interface::cmdUpdateBalloonNumberBlock()
{
    CAcModuleResourceOverride resOverride;
    CString title = _(L"更新气泡号");
    acutPrintf(L"\n%s\n", title);

    GenericPairEditDlg dlg(title, _(L"开始序号"), _(L"气泡序号字高"), true, true, true);

    int startNumber;
    dlg.setValidatorAndParser([&](const CString& strValue, const CString& _2) -> CString
    {
            try
            {
                size_t pos;
                startNumber = std::stoi(strValue.GetString(), &pos);
                if (pos != strValue.GetLength())
                {
                    throw std::exception();
                }
                if (startNumber < 0)
                {
                    throw std::exception();
                }
            }
            catch (...)
            {
                return _(L"开始序号必须为不小于 0 的整数");
            }
            return GenericPairEditDlg::ValidatorOk;
    });

    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    UniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

    acutPrintf(_(L"\n选中的气泡号将被设置为：%d"), startNumber);
    UniversalPicker::run(
        &arcv,
        [&](const AcDbObjectId& id)
        {
            if (BalloonNumber::updateBalloonNumberBlock(id, startNumber))
            {
                ++startNumber;
                acutPrintf(_(L"\n选中的气泡号将被设置为：%d"), startNumber);
            }
        },
        nullptr,
        UniversalPicker::SelectMode::Immediate,
        false,
        UniversalPicker::SortMode::RD,
        false
    );
}

void Interface::cmdImeAutoSwitch()
{
    CAcModuleResourceOverride resOverride;
    CString title = _(L"设置输入法自动切换");
    GenericPairEditDlg dlg(title, _(L"启用1/0"), _(L"启用1/0"), false, true, true);

    CString edit1Result, edit2Result;
    auto& manager = ConfigManager::getInstance();
    auto& config = manager.getConfig();
    bool bEnabled = config.imeSettings.bEnabled;
    unsigned long iInterval = config.imeSettings.iIntervalMs;
    const ConfigItems::ImeSettings defaultConfig;
    edit1Result.Format(L"%d", config.imeSettings.bEnabled);
    edit2Result.Format(L"%d", config.imeSettings.iIntervalMs);
    dlg.modifyEditControl(edit1Result, edit2Result);

    dlg.setValidatorAndParser([&](const CString& value1, const CString& value2) -> CString
        {
            if (value1.IsEmpty() || value2.IsEmpty())
            {
                return _(L"必须输入自启动状态和切换间隔时间");
            }
            if (value1.SpanIncluding(L"01") != value1)
            {
                return _(L"自启动状态必须为 0 或 1，1表示自启动，0 表示不自启动");
            }
            try
            {
                size_t pos;
                config.imeSettings.iIntervalMs = std::stoi(value2.GetString(), &pos);
                if (pos != value2.GetLength())
                {
                    throw std::exception();
                }
                if (config.imeSettings.iIntervalMs < defaultConfig.iIntervalMs)
                {
                    throw std::exception();
                }
            }
            catch (...)
            {
                CString csInvalidInterval;
                csInvalidInterval.Format(_(L"切换间隔必须为不小于 %d 的整数"), defaultConfig.iIntervalMs);
                return csInvalidInterval;
            }

            edit1Result = value1;
            return GenericPairEditDlg::ValidatorOk;
        });

    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    config.imeSettings.bEnabled = (edit1Result == L"1");
    ImeAutoSwitcher::stop();
    if (!manager.saveConfig())
    {
        std::wstring err = manager.getLastError();
        AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
        // 保存失败，还原状态
        config.imeSettings.bEnabled = bEnabled;
        config.imeSettings.iIntervalMs = iInterval;
    }
    if (config.imeSettings.bEnabled)
    {
        ImeAutoSwitcher::start(config.imeSettings.iIntervalMs);
    }
}

void Interface::cmdCloneText()
{
    AcString asSrcTextContent;
    acutPrintf(_(L"\n请选择要复制的源文本对象"));
    if (!TextUtil::getSelectedTextRawContent(asSrcTextContent) || asSrcTextContent.isEmpty())
    {
        acutPrintf(_(L"取消操作"));
        return;
    }
    acutPrintf(L"\n读取到：%s\n", asSrcTextContent.constPtr());

    acutPrintf(_(L"请选择要粘贴到的文本对象"));
    UniversalPicker::run(
        &TextUtil::textClassList,
        [&asSrcTextContent](const AcDbObjectId& id)
        {
            TextUtil::updateTextEntityContent(id, asSrcTextContent);
        },
        _(L"将多行/单行文本内容复制给其它多行/单行文本"),
        UniversalPicker::SelectMode::Immediate,
        false,
        UniversalPicker::SortMode::None,
        true
    );
}

void Interface::cmdIntersect()
{
    Commands::CommandList pszCmdList =
    {
        L"FILLET",
        L"M",
        L"R",
        L"0"
    };
    Commands::executeCommand(pszCmdList);
}

void Interface::cmdBalloonNumberOffset()
{
    CAcModuleResourceOverride resOverride;
    CString title = _(L"气泡号偏置");
    GenericPairEditDlg dlg(title, _(L"偏置值"), _(L"使用提示"), false, true, true);
    dlg.modifyEditControl(L"", _(L"新编号=旧编号+偏置值，若计算出新编号<0，则新编号=0"));

    int offset;
    dlg.setValidatorAndParser([&](const CString& strValue, const CString& _2) -> CString
        {
            if (strValue.IsEmpty())
            {
                return _(L"偏置值不可为空");
            }
            try
            {
                size_t pos = 0;
                offset = std::stoi(strValue.GetString(), &pos);
                if (pos != strValue.GetLength())
                {
                    throw std::exception();
                }
            }
            catch (...)
            {
                return _(L"偏置值必须为整数");
            }
            return GenericPairEditDlg::ValidatorOk;
        });

    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    UniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };
    UniversalPicker::run(
        &arcv,
        [&offset](const AcDbObjectId& id)
        {
            BalloonNumber::balloonNumberOffset(id, offset);
        },
        title,
        UniversalPicker::SelectMode::Batch,
        false,
        UniversalPicker::SortMode::None,
        true
    );
}

void Interface::cmdBalloonNumberFilter()
{
    CAcModuleResourceOverride resOverride;
    CString title = _(L"气泡号筛选");
    GenericPairEditDlg dlg(title, _(L"筛选条件"), _(L"使用提示"), false, true, false);
    dlg.modifyEditControl(L"", _(L"格式：符号+值，如：>>10筛选大于10。大于：>>或》》，小于：<<或《《，等于：==，不等于：<>或《》，>=或》=，小于等于：<=或《=。等于和不等于判断允许非整数。"));

    CString edit1Result;
    dlg.setValidatorAndParser([&](const CString& strValue, const CString& _2) -> CString
        {
            if (strValue.GetLength() < 3)
            {
                return _(L"筛选条件不能为空。");
            }

            // 验证输入合法性
            /////////////////////
            // 无限制判定值的符号列表
            std::vector<CString> opNoLimit = { BalloonNumber::OperatorType::equal, BalloonNumber::OperatorType::notEqual1, BalloonNumber::OperatorType::notEqual2 };
            // 限制判定值为数字的列表
            std::vector<CString> opNeedNumeric =
            {
                BalloonNumber::OperatorType::greater1, BalloonNumber::OperatorType::greater2,
                BalloonNumber::OperatorType::less1, BalloonNumber::OperatorType::less2,
                BalloonNumber::OperatorType::greaterEqual1, BalloonNumber::OperatorType::greaterEqual2,
                BalloonNumber::OperatorType::lessEqual1, BalloonNumber::OperatorType::lessEqual2
            };

            CString strInputOpType = strValue.Left(2);
            CString strInputOpValue = strValue.Mid(2);
            auto it = std::find(opNoLimit.begin(), opNoLimit.end(), strInputOpType);
            if (it == opNoLimit.end()) // 不在无限制判定值的符号列表中，则要求判定值必须为正数
            {
                // 进一步判断是否输入了非法符号
                it = std::find(opNeedNumeric.begin(), opNeedNumeric.end(), strInputOpType);
                if (it == opNeedNumeric.end())
                {
                    return _(L"输入的符号不合法。");
                }

                try
                {
                    size_t pos = 0;
                    int iInputOpValue = std::stoi(strInputOpValue.GetString(), &pos);
                    if (pos != strInputOpValue.GetLength())
                    {
                        throw std::exception();
                    }
                }
                catch (...)
                {
                    return _(L"非等于或不等于判定时，判定值必须为整数。");
                }
            }
            edit1Result = strValue;
            return GenericPairEditDlg::ValidatorOk;
        });

    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    AcDbObjectIdArray matchedIds;
    UniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

    UniversalPicker::run(
        &arcv,
        [&](const AcDbObjectId& id)
        {
            AcString attrValue;
            if (BalloonNumber::getBalloonAttributeValue(id, attrValue))
            {
                if (BalloonNumber::meetCriteria(attrValue, edit1Result.GetString()))
                {
                    matchedIds.append(id);
                }
            }
        },
        title,
        UniversalPicker::SelectMode::Batch,
        true,
        UniversalPicker::SortMode::None,
        true
    );

    if (matchedIds.length() > 0)
    {
        UniversalPicker::setSelection(matchedIds);
        acutPrintf(_(L"\n筛选完成：%d 个匹配项。"), matchedIds.length());
    }
    else
    {
        acutPrintf(_(L"\n未发现符合条件的气泡号。"));
    }
}

void Interface::cmdImportCsvToMTextMatrix()
{
    FileDialog::FileDialogFilterBuilder fileFilterBuilter;
    CString strFileFilter = fileFilterBuilter.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
    CString strFilePath = FileDialog::ShowOpenFileDialog(_(L"选择要导入的文件"), L"csv", strFileFilter);
    if (strFilePath.IsEmpty())
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    CsvModule::AcStringMatrix matrixData;
    CsvModule::readCsvToAcStringMatrix(strFilePath, matrixData);

    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(_(L"从 CSV 文件导入数据生成多行文本矩阵"), _(L"参数"), _(L"使用提示"), false, true, true);
    CString strTipMTextMatrixParameter;
    double TEXTSIZE;
    if (!AcadVarUtil::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
    {
        AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
        return;
    }
    double scale = Annotative::getCurrentScaleValue();
    strTipMTextMatrixParameter.Format(_(L"输入3个不小于0的数，使用空格分隔，分别为：列宽、列步长、行步长。显示文字高度 = TEXTSIZE变量值%g × 注释比例缩放值%g = %g"), TEXTSIZE, scale, TEXTSIZE * scale);
    dlg.modifyEditControl(L"", strTipMTextMatrixParameter);

    std::vector<double> params;
    dlg.setValidatorAndParser([&](const CString& edit1, const CString& _) -> CString
        {
            const int paramsNumber = 3;
            if (!Common::parse(edit1, paramsNumber, [](double v) { return v > 0; }, params))
            {
                return strTipMTextMatrixParameter;
            }
            return GenericPairEditDlg::ValidatorOk;
        });

    if (dlg.DoModal() != IDOK)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    ads_point pt{};
    if (acedGetPoint(nullptr, _(L"请选择位置"), pt) != RTNORM)
    {
        acutPrintf(_(L"取消操作"));
        return;
    }

    TextUtil::createMTextMatrix(params[0], params[1], params[2], matrixData, asPnt3d(pt));
}

    void Interface::cmdSpatialTableExplorer()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"将多行/单行文本按空间位置表格化导出到 CSV 文件");
        GenericPairEditDlg dlg(title, _(L"参数"), _(L"使用提示"), false, true, true);

        // 默认列容差和行容差
        // 字高默认使用 TEXTSIZE 变量值，列容差默认按字高的 3 倍，行容差默认按字高的 1 倍（考虑注释比例缩放值）
        CString strInitParameter;
        double scale = Annotative::getCurrentScaleValue();
        double TEXTSIZE;
        if (!AcadVarUtil::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
        {
            AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
            return;
        }
        strInitParameter.Format(L"%g %g", TEXTSIZE * scale * 3, TEXTSIZE * scale * 1);
        dlg.modifyEditControl(strInitParameter, _(L"输入2个不小于0的数，使用空格分隔，分别为：列容差、行容差。文本距离超过容差视为不同列或行。"));

        std::vector<double> params;
        dlg.setValidatorAndParser([&](const CString& edit1, const CString& _2) -> CString
            {
                const int paramsNumber = 2;
                if (!Common::parse(edit1, paramsNumber, [](double v) { return v > 0; }, params))
                {
                    return _(L"输入2个不小于0的数，使用空格分隔，分别为：列容差、行容差。文本距离超过容差视为不同列或行。");
                }
                return GenericPairEditDlg::ValidatorOk;
            });

    
        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        FileDialog::FileDialogFilterBuilder fileFilterBuilter;
        CString strFileFilter = fileFilterBuilter.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
        CString strFilePath = FileDialog::ShowSaveFileDialog(_(L"保存 CSV 文件到"), _(L"数据文件.csv"), L"csv", strFileFilter);
        if (strFilePath.IsEmpty())
        {
            acutPrintf(_(L"取消操作"));
            return;
        }
        CsvWriter writer(strFilePath);
        if (!writer.isValid())
        {
            AfxMessageBox(_(L"文件路径打开失败，请检查是否被占用或路径无效"), MB_OK | MB_ICONERROR);
            return;
        }

        TextUtil::TextEntityDataList elements;
        UniversalPicker::run(
            &TextUtil::textClassList,
            [&](const AcDbObjectId& id)
            {
                TextUtil::TextEntityData data;
                data.id = id;
                if (TextUtil::readMText(id, data.text, false, &data.pos))
                {
                    acutPrintf(_(L"\n(%g,%g,%g)多行文本：%s"), data.pos.x, data.pos.y, data.pos.z, data.text.constPtr());
                }
                else if (TextUtil::readDText(id, data.text, false, &data.pos))
                {
                    acutPrintf(_(L"\n(%g,%g,%g)单行文本：%s"), data.text.constPtr());
                }
                elements.push_back(data);
            },
            title,
            UniversalPicker::SelectMode::Batch,
            true,
            UniversalPicker::SortMode::None,
            true
        );

        CsvModule::AcStringMatrix matrixData;
        TextUtil::structureTextToAcStringMatrix(elements, params[0], params[1], matrixData);
    

        for (const auto& row : matrixData)
        {
            writer.writeRow(row);
            acutPrintf(L"\n");
            for (const auto& field : row)
            {
                acutPrintf(L"%s\t", field.constPtr());
            }
        }

        acutPrintf(_(L"\n文件位置：%s"), strFilePath);
    }

    void Interface::cmdCheckBalloonNumberMaxMin()
    {
        UniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };
        AcString strValue;
        int max = INT_MIN;
        int min = INT_MAX;
        AcDbObjectId maxId = AcDbObjectId::kNull;
        AcDbObjectId minId = AcDbObjectId::kNull;
        AcDbObjectIdArray matchedIds;
        UniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (BalloonNumber::getBalloonAttributeValue(id, strValue))
                {
                    try
                    {
                        size_t pos;
                        int number = std::stoi(strValue.constPtr(), &pos);
                        if (pos != strValue.length())
                        {
                            throw std::exception();
                        }
                        if (number > max)
                        {
                            max = number;
                            maxId = id;
                        }
                        if (number < min)
                        {
                            min = number;
                            minId = id;
                        }

                    }
                    catch (...)
                    {

                    }
                }

            },
            _(L"查找气泡号最大和最小序号"),
            UniversalPicker::SelectMode::Batch,
            true,
            UniversalPicker::SortMode::None,
            true
        );

        AcDbObjectIdArray resultIds;
        if (maxId != AcDbObjectId::kNull)
        {
            resultIds.append(maxId);
        }
        if (minId != AcDbObjectId::kNull)
        {
            resultIds.append(minId);
        }
        if (resultIds.length() > 0)
        {
            UniversalPicker::setSelection(resultIds);
            acutPrintf(_(L"\n最大气泡号：%d，最小气泡号：%d"), max, min);
        }
        else
        {
            acutPrintf(_(L"\n未发现有效数字格式的气泡号"));
        }
    }

    void Interface::cmdPasteClipImage()
    {
        if (!Image::clipboardHasImage())
        {
            AfxMessageBox(_(L"剪贴板中没有检测到图像数据。"), MB_OK | MB_ICONWARNING);
            return;
        }

        FileDialog::FileDialogFilterBuilder filterBuilder;
        CString fileFilter = filterBuilder.addFilter(_(L"PNG 图片"), { L"*.png" }).build();
        CString defaultFilename;
        defaultFilename.Format(_(L"图片%s.png"), Common::getTimestamp());
        CString filename = FileDialog::ShowSaveFileDialog(_(L"选择图片保存路径"), defaultFilename, L"png", fileFilter, Common::getCurrPath(true));
        if (filename.IsEmpty())
        {
            acutPrintf(_(L"取消操作"));
            return;
        }
        if (!Image::saveClipboardBitmapToFile(filename))
        {
            AfxMessageBox(_(L"保存剪贴板图像数据到文件失败"), MB_OK | MB_ICONERROR);
            return;
        }
        if (!Image::copyFileToClipboard(filename))
        {
            AfxMessageBox(_(L"复制文件到剪贴板失败"), MB_OK | MB_ICONERROR);
            return;
        }
        const wchar_t* appName = acedGetAppName();
        Commands::CommandList pszCmdList =
        {
            L"PASTECLIP"
        };
        Commands::executeCommand(pszCmdList);
    }

    void Interface::cmdLocateDrawing()
    {
        CString drawingPath = Common::getCurrPath();
        if (drawingPath.IsEmpty())
        {
            AfxMessageBox(_(L"图纸未保存"), MB_OK | MB_ICONERROR);
            return;
        }

        FileDialog::locateFileInExplorer(drawingPath);
    }

    void Interface::cmdCheckDuplicateBalloonNumbers()
    {
        UniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

        std::map<AcString, AcDbObjectIdArray> numberMap;
        AcString strValue;

        UniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (BalloonNumber::getBalloonAttributeValue(id, strValue))
                {
                    if (!strValue.isEmpty())
                    {
                        numberMap[strValue].append(id);
                    }
                }
            },
            _(L"检查重复气泡号"),
            UniversalPicker::SelectMode::Batch,
            true,
            UniversalPicker::SortMode::None,
            true
        );

        AcDbObjectIdArray duplicateIds;
        AcString reportMsg = L"";
        for (auto const& [text, ids] : numberMap)
        {
            if (ids.length() > 1)
            {
                duplicateIds.append(ids);

                if (!reportMsg.isEmpty())
                {
                    reportMsg.append(L", ");
                }
                reportMsg.append(text);
            }
        }

        if (duplicateIds.length() > 0)
        {
            UniversalPicker::setSelection(duplicateIds);
            acutPrintf(_(L"\n发现重复: %s"), reportMsg.constPtr());
        }
        else
        {
            acutPrintf(_(L"未发现重复"));
        }
    }

    void Interface::cmdCheckBalloonNumberBreakpoints()
    {
        UniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

        // 使用 set 自动去重并升序排序
        std::set<int> numbers;
        AcString strValue;

        UniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (BalloonNumber::getBalloonAttributeValue(id, strValue))
                {
                    try
                    {
                        size_t pos = 0;
                        // 强制宽字符转换，确保完全解析
                        int number = std::stoi(strValue.constPtr(), &pos);

                        if (pos == static_cast<size_t>(strValue.length()))
                        {
                            numbers.insert(number);
                        }
                    }
                    catch (...)
                    {
                        // 无法转换为数字的内容不参与断点计算
                    }
                }
            },
            _(L"检查气泡号断点"),
            UniversalPicker::SelectMode::Batch,
            true,
            UniversalPicker::SortMode::None,
            true
        );

        // 只有 1 个或 0 个数字无法构成断点
        if (numbers.size() < 2)
        {
            return;
        }

        AcString reportMsg = L"";
        auto it = numbers.begin();
        int prev = *it;
        ++it;
        for (; it != numbers.end(); ++it)
        {
            int curr = *it;
            // 检查数字是否连续
            if (curr != prev + 1)
            {
                if (!reportMsg.isEmpty())
                {
                    reportMsg.append(L", ");
                }

                int missStart = prev + 1;
                int missEnd = curr - 1;
                if (missStart == missEnd)
                {
                    // 单点缺失：如 3, 5 -> 4
                    AcString tmp;
                    tmp.format(L"%d", missStart);
                    reportMsg.append(tmp);
                }
                else
                {
                    // 区间缺失：如 3, 7 -> 4-6
                    AcString tmp;
                    tmp.format(L"%d-%d", missStart, missEnd);
                    reportMsg.append(tmp);
                }
            }
            prev = curr;
        }

        // 结果呈现
        if (reportMsg.empty() == false)
        {
            acutPrintf(_(L"\n缺失的气泡编号: %s"), reportMsg.constPtr());
        }
        else
        {
            acutPrintf(_(L"\n气泡编号连续，未发现断点"));
        }
    }

    void Interface::cmdForceRemoveImage()
    {
        UniversalPicker::AcRxClassVector arcv = { AcDbRasterImage::desc() };
        UniversalPicker::run(
            &arcv,
            Image::forceRemoveImageAndFile,
            _(L"删除光栅图像及图片文件（无法撤销恢复）"),
            UniversalPicker::SelectMode::Immediate,
            false,
            UniversalPicker::SortMode::None,
            true
        );
    }

    void Interface::cmdLocateSelf()
    {
        const wchar_t* appName = acedGetAppName();
        FileDialog::locateFileInExplorer(appName);
    }

    void Interface::cmdChainSelection()
    {
        UniversalPicker::AcRxClassVector arcv = { AcDbArc::desc(), AcDbPolyline::desc(), AcDbLine::desc(), AcDbSpline::desc() };
        resbuf* pFilter = UniversalPicker::buildFilter(&arcv);

        UniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id, bool& bBreak)
            {
                // 局部容器：确保每次点击都是独立的搜索过程
                std::queue<AcDbObjectId> waitingQueue;
                std::set<AcDbObjectId> processedIds;
                AcDbObjectIdArray resultIds;

                waitingQueue.push(id);
                processedIds.insert(id);

                // 广度优先搜索 (BFS)
                while (!waitingQueue.empty())
                {
                    AcDbObjectId currentId = waitingQueue.front();
                    waitingQueue.pop();
                    resultIds.append(currentId);

                    AcDbCurve* pCurve = Common::getObject<AcDbCurve>(currentId, AcDb::kForRead);
                    if (pCurve == nullptr)
                    {
                        continue;
                    }

                    AcGePoint3d startPt, endPt;
                    if (pCurve->getStartPoint(startPt) == Acad::eOk && pCurve->getEndPoint(endPt) == Acad::eOk)
                    {
                        AcGePoint3d checkPts[2] = { startPt, endPt };

                        for (const auto& pt : checkPts)
                        {
                            AcDbObjectIdArray neighbors = Common::getNeighborsAtPoint(pt, pFilter);

                            for (int i = 0; i < neighbors.length(); ++i)
                            {
                                AcDbObjectId nId = neighbors[i];

                                // 严格去重：
                                // 1. 跳过当前正在处理的实体本身 (nId != currentId)
                                // 2. 跳过已经进入过队列或处理过的实体 (processedIds.find == end)
                                if (nId != currentId && processedIds.find(nId) == processedIds.end())
                                {
                                    processedIds.insert(nId);
                                    waitingQueue.push(nId);
                                }
                            }
                        }
                    }
                }

                if (resultIds.length() > 0)
                {
                    UniversalPicker::setSelection(resultIds);
                    acutPrintf(_(L"\n自动链式选择成功：共选中 %d 条线条实体。"), resultIds.length());
                    bBreak = true;
                }
            },
            _(L"选中实体后自动链式选择"),
            UniversalPicker::SelectMode::Immediate,
            true
        );

        // 5. 清理过滤器资源
        UniversalPicker::freeFilter(pFilter);
    }

    void Interface::cmdDimensionTolerancePrecision()
    {
        CAcModuleResourceOverride resOverride;
        GenericPairEditDlg dlg(_(L"设置尺寸标注的主单位精度和公差精度"), _(L"主单位精度"), _(L"公差精度"), false, true, true);
        // 设置 -1 表示不修改精度 
        CString strDimPrec = L"-1";
        CString strTolPrec = L"-1";
        dlg.modifyEditControl(strDimPrec, strTolPrec);

        int iDimPrec = -1;
        int iTolPrec = -1;
        dlg.setValidatorAndParser([&](const CString& val1, const CString& val2) -> CString
            {
                try
                {
                    size_t pos;
                    iDimPrec = std::stoi(val1.GetString(), &pos);
                    if (pos != val1.GetLength())
                    {
                        throw std::exception();
                    }
                    if (iDimPrec < 0 && iDimPrec != -1)
                    {
                        throw std::exception();
                    }
                    if (iDimPrec > 8)
                    {
                        throw std::exception();
                    }

                    iTolPrec = std::stoi(val2.GetString(), &pos);
                    if (pos != val2.GetLength())
                    {
                        throw std::exception();
                    }
                    if (iTolPrec < 0 && iTolPrec != -1)
                    {
                        throw std::exception();
                    }
                    if (iTolPrec > 8)
                    {
                        throw std::exception();
                    }
                }
                catch (...)
                {
                    return _(L"精度值只能是 0 至 8 的整数，输入 -1 时不修改精度。");
                }
                return GenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        UniversalPicker::run(
            &Common::DimensionSubClasses,
            [&](const AcDbObjectId& id)
            {
                Dimension::setDimensionTolerancePreccision(id, iDimPrec, iTolPrec);
            },
            _(L"设置尺寸标注的主单位精度和公差精度"),
            UniversalPicker::SelectMode::Immediate,
            false,
            UniversalPicker::SortMode::None,
            true
        );
    }

    void Interface::cmdCreateIntersectionPoints()
    {
        // AcDbCurve 曲线类的子类
        UniversalPicker::AcRxClassVector arcv =
        {
            AcDb2dPolyline::desc(),
            AcDb3dPolyline::desc(),
            AcDbArc::desc(),
            AcDbCircle::desc(),
            AcDbEllipse::desc(),
            AcDbLeader::desc(),
            AcDbLine::desc(),
            AcDbPolyline::desc(),
            AcDbRay::desc(),
            AcDbSpline::desc(),
            //AcDbHelix::desc(), // 不清楚是哪个库文件中定义的
            AcDbXline::desc()
        };

        AcDbObjectId lastId = AcDbObjectId::kNull;
        acutPrintf(_(L"\n请选择第一条线："));
        UniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (lastId == AcDbObjectId::kNull)
                {
                    lastId = id;
                    acutPrintf(_(L"\n请选择第二条线："));
                }
                else if (lastId != id)
                {
                    AcGePoint3dArray intersectionPoints;
                    if (LineUtil::calculateLineIntersection(lastId, id, intersectionPoints))
                    {
                        PointUtil::drawPoints(intersectionPoints);
                        acutPrintf(_(L"\n成功创建 %d 个交点"), intersectionPoints.length());
                    }
                    else
                    {
                        acutPrintf(_(L"\n未发现交点"));
                    }
                    lastId = AcDbObjectId::kNull;
                    acutPrintf(_(L"\n请选择第一条线："));
                }

            },
            _(L"创建两条线(及延长线)的交点。可使用PTYPE设置点样式。"),
            UniversalPicker::SelectMode::Immediate,
            true
        );
    }

    void Interface::cmdPrintConfigFilename()
    {
        auto& manager = ConfigManager::getInstance();
        std::wstring configFilename = manager.getConfigFilename();
        FileDialog::locateFileInExplorer(configFilename.c_str());
    }

    void Interface::cmdDialogMiddleClickToOk()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置对话框中鼠标中键映射到确定按钮");
        GenericPairEditDlg dlg(title, _(L"启用1/0"), L"", true, true, true);

        CString edit1Result;
        auto& manager = ConfigManager::getInstance();
        auto& config = manager.getConfig();
        bool bDialogMiddleClickToOkEnabled = config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled;
        edit1Result.Format(L"%d", config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled);
        dlg.modifyEditControl(edit1Result);

        dlg.setValidatorAndParser([&](const CString& value1, const CString& _2) -> CString
            {
                if (value1.IsEmpty())
                {
                    return _(L"必须输入1或0设置是否启用中键映射确定按钮");
                }
                if (value1.SpanIncluding(L"01") != value1)
                {
                    return _(L"必须输入1或0设置是否启用中键映射确定按钮");
                }
                edit1Result = value1;
                return GenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled = (edit1Result == L"1");
        auto& middleClickManager = MiddleClickManager::getInstance();
        middleClickManager.stopUnifiedMiddleClickProc();
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            // 保存失败，还原状态
            config.middleClickManagerSettings.bDialogMiddleClickToOkEnabled = bDialogMiddleClickToOkEnabled;
        }
        middleClickManager.startUnifiedMiddleClickProc(config.middleClickManagerSettings);
    }

    void Interface::cmdCmdMiddleClickToEnter()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置命令执行状态下鼠标中键映射回车键");
        GenericPairEditDlg dlg(title, _(L"启用1/0"), _(L"间隔(ms)"), false, true, true);

        CString edit1Result, edit2Result;
        auto& manager = ConfigManager::getInstance();
        auto& config = manager.getConfig();
        bool bEnabled = config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled;
        unsigned long dCmdMiddleClickDownUpInterval = config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval;
        edit1Result.Format(L"%d", bEnabled);
        edit2Result.Format(L"%d", dCmdMiddleClickDownUpInterval);
        const ConfigItems::MiddleClickManagerSettings defaultConfig;
        dlg.modifyEditControl(edit1Result, edit2Result);

        dlg.setValidatorAndParser([&](const CString& value1, const CString& value2) -> CString
            {
                if (value1.IsEmpty() || value2.IsEmpty())
                {
                    return _(L"必须输入自启动状态和切换间隔时间");
                }
                if (value1.SpanIncluding(L"01") != value1)
                {
                    return _(L"自启动状态必须为 0 或 1，1表示自启动，0 表示不自启动");
                }
                try
                {
                    size_t pos;
                    config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval = std::stoi(value2.GetString(), &pos);
                    if (pos != value2.GetLength())
                    {
                        throw std::exception();
                    }
                    if (config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval < defaultConfig.dCmdMiddleClickDownUpInterval)
                    {
                        throw std::exception();
                    }
                }
                catch (...)
                {
                    CString csInvalidInterval;
                    csInvalidInterval.Format(_(L"切换间隔必须为不小于 %d 的整数"), defaultConfig.dCmdMiddleClickDownUpInterval);
                    return csInvalidInterval;
                }

                edit1Result = value1;
                return GenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled = (edit1Result == L"1");
        MiddleClickManager::getInstance().stopUnifiedMiddleClickProc();
        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            // 保存失败，还原状态
            config.middleClickManagerSettings.bCmdMiddleClickToEnterEnabled = bEnabled;
            config.middleClickManagerSettings.dCmdMiddleClickDownUpInterval = dCmdMiddleClickDownUpInterval;
        }
        MiddleClickManager::getInstance().startUnifiedMiddleClickProc(config.middleClickManagerSettings);
    }

    void Interface::cmdSetLanguage()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"设置语言");
        GenericPairEditDlg dlg(title, _(L"语言代码"), L"提示", false, true, true);

        auto& manager = ConfigManager::getInstance();
        auto& config = manager.getConfig();
        std::wstring languageCode = config.languageSettings.languageCode;
        dlg.modifyEditControl(languageCode.c_str(), _(L"无匹配语言代码的翻译文件时，默认显示中文"));

        dlg.setValidatorAndParser([&](const CString& value1, const CString& _2) -> CString
            {
                if (value1.IsEmpty())
                {
                    return _(L"必须输入语言代码");
                }
                config.languageSettings.languageCode = value1.GetString();
                return GenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        if (!manager.saveConfig())
        {
            std::wstring err = manager.getLastError();
            AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
            config.languageSettings.languageCode = languageCode;
        }
        AfxMessageBox(_(L"重启插件刷新语言设置"), MB_OK | MB_ICONINFORMATION);
    }