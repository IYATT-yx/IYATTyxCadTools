module;
#include "stdafx.h"
#include "GenericPairEditDlg.hpp"
#include "MainBar.hpp"
#include "resource.h"

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

void Interface::init()
{
    CAcModuleResourceOverride resOverride;
    // 命令列表
    Commands::commandInfoList =
    {
        {L"yx", Common::loadString(IDS_yxCommandDescription), Commands::CommandFlags::Base, Interface::cmdHelp},
        {L"yxTest", Common::loadString(IDS_yxTestCommandDescription), Commands::CommandFlags::Base, Interface::test},
        {L"yxUnload", Common::loadString(IDS_yxUnloadCommandDescription), Commands::CommandFlags::Base, Interface::cmdUnloadApp},
        {L"yxSetByLayer", Common::loadString(IDS_yxSetByLayerCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdSetByLayer},
        {L"yxDimensionFix", Common::loadString(IDS_yxDimensionFixCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdDimensionFix},
        {L"yxDimensionResume", Common::loadString(IDS_yxDimensionResumeCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdDimensionResume},
        {L"yxAddSurroundingCharsForDimension", Common::loadString(IDS_yxAddSurroundingCharsForDimensionCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdAddSurroundingCharsForDimension},
        {L"yxRemoveSurroundingCharsForDimension", Common::loadString(IDS_yxRemoveSurroundingCharsForDimensionCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdRemoveSurroundingCharsForDimension},
        {L"yxSetBasicBox", Common::loadString(IDS_yxSetBasicBoxCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdSetBasicBox},
        {L"yxUnsetBasicBox", Common::loadString(IDS_yxUnsetBasicBoxCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdUnsetBasicBox},
        {L"yxSetRefDim", Common::loadString(IDS_yxSetRefDimCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdSetRefDim},
        {L"yxUnsetRefDim", Common::loadString(IDS_yxUnsetRefDimCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdUnsetRefDim},
        {L"yxInsertBalloonNumberBlockWithStartNumber", Common::loadString(IDS_yxInsertBalloonNumberBlockWithStartNumberCommandDescription), Commands::CommandFlags::Base, Interface::cmdInsertBalloonNumberBlockWithStartNumber},
        {L"yxPrintClassHierarchy", Common::loadString(IDS_yxPrintClassHierarchyCommandDescription), Commands::CommandFlags::Base, Interface::cmdPrintClassHierarchy},
        {L"yxExtractAnnotations", Common::loadString(IDS_yxExtractAnnotationsCommandDescription), Commands::CommandFlags::Base, Interface::cmdExtractAnnotations},
        {L"yxUpdateBalloonNumberBlock", Common::loadString(IDS_yxUpdateBalloonNumberBlockCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdUpdateBalloonNumberBlock},
        {L"yxImeAutoSwitch", Common::loadString(IDS_yxImeAutoSwitchCommandDescription), Commands::CommandFlags::Base, Interface::cmdImeAutoSwitch},
        {L"yxCloneText", Common::loadString(IDS_yxCloneTextCommandDescription), Commands::CommandFlags::Base, Interface::cmdCloneText},
        {L"yxIntersect", Common::loadString(IDS_yxIntersect), Commands::CommandFlags::Base, Interface::cmdIntersect},
        {L"yxBalloonNumberOffset", Common::loadString(IDS_yxBalloonNumberOffsetCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdBalloonNumberOffset},
        {L"yxBalloonNumberFilter", Common::loadString(IDS_yxBalloonNumberFilterCommandDescription), Commands::CommandFlags::PickRedraw, Interface::cmdBalloonNumberFilter}
    };

    // 注册命令
    Commands::registerYxCmds(Commands::commandInfoList);

    // 输入法自动切换自启动 
    bool bAutoStart = false;
    int nAutoMonitorInterval = 200;
    ImeAutoSwitcher::loadSettings(bAutoStart, nAutoMonitorInterval);
    if (bAutoStart)
    {
        ImeAutoSwitcher::start(nAutoMonitorInterval);
    }
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
    // 关闭命令菜单
    MainBar::terminateBar();
    // 卸载命令
    CAcModuleResourceOverride resOverride;
    CString cmdGroup;
    cmdGroup.LoadStringW(IDS_CommandGroup);
	acedRegCmds->removeGroup(cmdGroup);
    acutPrintf(Common::loadString(IDS_Msg_Unload_FMT), Common::loadString(IDS_LocaleProjectName));
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

void Interface::cmdHelp()
{
    CAcModuleResourceOverride resOverride;
    acutPrintf(L"\n%s\n", Common::loadString(IDS_LocaleProjectName));
    acutPrintf(L"%s：%s_%s\n", Common::loadString(IDS_Version), BuildingTime::WDATE, BuildingTime::WTIME);
    acutPrintf(L"%s：IYATT-yx\n", Common::loadString(IDS_Author));
    acutPrintf(L"%s：https://github.com/IYATT-yx/IYATTyxCadTools\n", Common::loadString(IDS_ProjectUrl));
    MainBar::showBar(Commands::commandInfoList);
}

void Interface::cmdSetByLayer()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(nullptr, EntityStyle::setByLayer, Common::loadString(IDS_yxSetByLayerCommandDescription));
}

void Interface::cmdDimensionFix()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(&Common::DimensionSubClasses, Dimension::dimensionFix, Common::loadString(IDS_yxDimensionFixCommandDescription));
}

void Interface::cmdDimensionResume()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(&Common::DimensionSubClasses, Dimension::dimensionResume, Common::loadString(IDS_yxDimensionResumeCommandDescription));
}

void Interface::cmdAddSurroundingCharsForDimension()
{
    CAcModuleResourceOverride resOverride;
    CString title = Common::loadString(IDS_yxAddSurroundingCharsForDimensionCommandDescription);
    GenericPairEditDlg dlg(title, Common::loadString(IDS_PrefixSymbol), Common::loadString(IDS_SuffixSymbol));

    CString edit1Result;
    CString edit2Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
        edit2Result = dlg.getEdit2Result();
    }
    else
    {
        return;
    }

    const wchar_t* left = edit1Result.GetString();
    const wchar_t* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);

    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [left, right, isLGdt, isRGdt](AcDbObjectId objId)
        {
            Dimension::addSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
        },
        title,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdRemoveSurroundingCharsForDimension()
{
    CAcModuleResourceOverride resOverride;
    CString title = Common::loadString(IDS_yxRemoveSurroundingCharsForDimensionCommandDescription);
    GenericPairEditDlg dlg(title, Common::loadString(IDS_PrefixSymbol), Common::loadString(IDS_SuffixSymbol));

    CString edit1Result;
    CString edit2Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
        edit2Result = dlg.getEdit2Result();
    }
    else
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }

    const wchar_t* left = edit1Result.GetString();
    const wchar_t* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [left, right, isLGdt, isRGdt](AcDbObjectId objId)
        {
            Dimension::removeSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
        },
        title,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdSetBasicBox()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetBasicBox(objId, true);
        },
        Common::loadString(IDS_yxSetBasicBoxCommandDescription),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdUnsetBasicBox()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetBasicBox(objId, false);
        },
        Common::loadString(IDS_yxUnsetBasicBoxCommandDescription),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdSetRefDim()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, true);
        },
        Common::loadString(IDS_yxSetRefDimCommandDescription),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdUnsetRefDim()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, false);
        },
        Common::loadString(IDS_yxUnsetRefDimCommandDescription),
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdInsertBalloonNumberBlockWithStartNumber()
{
    CAcModuleResourceOverride resOverride;
    CString title = Common::loadString(IDS_yxInsertBalloonNumberBlockWithStartNumberCommandDescription);
    acutPrintf(L"\n%s\n", title);

    GenericPairEditDlg dlg(title, Common::loadString(IDS_StartNumber), Common::loadString(IDS_Prompt), false, true, true);
    // 设置默认字高
    CString csTips;
    csTips.Format(Common::loadString(IDS_Msg_BalloonTips_FMT), Common::BalloonNumberBlock::defaultTextHeight);
    dlg.modifyEditControl(L"", csTips);

    CString edit1Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
    }
    else
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }

    if (edit1Result.IsEmpty())
    {
        AfxMessageBox(Common::loadString(IDS_Err_EmptyStartNumber), MB_OK | MB_ICONERROR);
        return;
    }

    if (edit1Result.SpanIncluding(L"0123456789") != edit1Result)
    {
        AfxMessageBox(Common::loadString(IDS_Err_InvalidStartNumber), MB_OK | MB_ICONERROR);
        return;
    }
    int startNum = std::stoi(edit1Result.GetString());
    BalloonNumber::createBalloonNumberBlock();
    BalloonNumber::insertBalloonNumberBlockWithStartNumber(startNum);
}

void Interface::cmdPrintClassHierarchy()
{
    CAcModuleResourceOverride resOverride;
    UniversalPicker::run(nullptr, Common::printClassHierarchy, Common::loadString(IDS_yxPrintClassHierarchyCommandDescription), UniversalPicker::SelectMode::Immediate, true);
}

void Interface::cmdExtractAnnotations()
{
    CAcModuleResourceOverride resOverride;
    CString filePath = FileDialog::ShowSaveFileDialog(Common::loadString(IDS_SaveCsvTitle));
    if (filePath.IsEmpty())
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }

    CsvWriter csv(filePath);
    if (!csv.isValid())
    {
        AfxMessageBox(Common::loadString(IDS_Err_FileOpenFailed), MB_OK | MB_ICONERROR);
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
                double dMeasuredValue = dimData.measuredValue;
                if (dimData.isAngle)
                {
                    dMeasuredValue = dimData.degreeValue();
                }
                Common::double2AcString(dMeasuredValue, asMeasuredValue, dimData.measuredValuePrecision);
                
                //acutPrintf(L"\n调试：%s %s %f %f ", dimData.prefix.constPtr(), dimData.suffix.constPtr(), dimData.tolUpper, dimData.tolLower);
                // 公差
                AcString asTol,asTolUpper, asTolLower;
                if (dimData.tolUpper == 0 && dimData.tolLower == 0) // 无公差
                {
                    asTol = L"";
                    asTolUpper = L"0";
                    asTolLower = L"0";
                }
                else
                {
                    if (abs(dimData.tolUpper + dimData.tolLower) < 1e-6) // 等双向公差
                    {
                        double dAbsTol = abs(dimData.tolUpper);
                        asTol.format(L"%s%.*g", Common::SymbolCodes::PlusMinus, dimData.tolPrecision, dAbsTol);
                        asTolUpper.format(L"%.*g", dimData.tolPrecision, dAbsTol);
                        asTolLower.format(L"-%.*g", dimData.tolPrecision, dAbsTol);
                    }
                    else // 双向公差
                    {
                        
                        Common::double2AcString(dimData.tolUpper, asTolUpper, dimData.tolPrecision);
                        Common::double2AcString(dimData.tolLower, asTolLower, dimData.tolPrecision);
                        asTol.format(L"+%s/%s", asTolUpper.constPtr(), asTolLower.constPtr());
                        asTolUpper.format(L"%.*g", dimData.tolPrecision, dimData.tolUpper);
                        asTolLower.format(L"%.*g", dimData.tolPrecision, dimData.tolLower);
                    }
                }

                AcString asDimText = dimData.prefix + asMeasuredValue + asTol + dimData.suffix;
                acutPrintf(L"\n%s：%s", Common::loadString(IDS_Dimension), asDimText.constPtr());
                std::vector<AcString> row = { asDimText, asMeasuredValue, asTolUpper, asTolLower }; // 完整尺寸文本、名义值、上极限偏差、下极限偏差
                csv.writeRow(row);
            }
            else if (gtData.status) // 形位公差
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
                        acutPrintf(L"\n%s：%s", Common::loadString(IDS_GDT), strRow.constPtr());
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
                acutPrintf(L"\n%s：%s", Common::loadString(IDS_MText), asMText.constPtr());
                std::vector<AcString> rows = { asMText };
                csv.writeRow(rows);
            }
            else if (TextUtil::readDText(objId, asDText))
            {
                acutPrintf(L"\n%s：%s", Common::loadString(IDS_DText), asDText.constPtr());
                std::vector<AcString> rows = { asDText };
                csv.writeRow(rows);
            }
        },
        Common::loadString(IDS_yxExtractAnnotationsCommandDescription),
        UniversalPicker::SelectMode::Immediate,
        false,
        UniversalPicker::SortMode::RD,
        false
    );
}

void Interface::cmdUpdateBalloonNumberBlock()
{
    CAcModuleResourceOverride resOverride;
    CString title = Common::loadString(IDS_yxUpdateBalloonNumberBlockCommandDescription);
    acutPrintf(L"\n%s\n", title);

    GenericPairEditDlg dlg(title, Common::loadString(IDS_StartNumber), Common::loadString(IDS_BalloonNumberHeight), true, true, true);

    CString edit1Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
    }
    else
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }

    if (edit1Result.IsEmpty())
    {
        AfxMessageBox(Common::loadString(IDS_Err_EmptyStartNumber), MB_OK | MB_ICONERROR);
        return;
    }

    if (edit1Result.SpanIncluding(L"0123456789") != edit1Result)
    {
        AfxMessageBox(Common::loadString(IDS_Err_InvalidStartNumber), MB_OK | MB_ICONERROR);
        return;
    }
    int startNum = std::stoi(edit1Result.GetString());

    UniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

    acutPrintf(Common::loadString(IDS_BalloonNextNumber_FMT), startNum);
    UniversalPicker::run(
        &arcv,
        [&startNum](const AcDbObjectId& id)
        {
            if (BalloonNumber::updateBalloonNumberBlock(id, startNum))
            {
                ++startNum;
                acutPrintf(Common::loadString(IDS_BalloonNextNumber_FMT), startNum);
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
    CString title = Common::loadString(IDS_yxImeAutoSwitchCommandDescription);
    GenericPairEditDlg dlg(title, Common::loadString(IDS_AutoStart), Common::loadString(IDS_Interval), false, true, true);

    CString edit1Result, edit2Result;
    bool bAutoStart;
    int nInterval;
    ImeAutoSwitcher::loadSettings(bAutoStart, nInterval);
    edit1Result.Format(L"%d", bAutoStart);
    edit2Result.Format(L"%d", nInterval);
    dlg.modifyEditControl(edit1Result, edit2Result);
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
        edit2Result = dlg.getEdit2Result();
    }
    else
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }

    if (edit1Result.IsEmpty() || edit2Result.IsEmpty())
    {
        AfxMessageBox(Common::loadString(IDS_Err_ImeAutoSwitchEmptySetting), MB_OK | MB_ICONERROR);
    }

    if (edit1Result.SpanIncluding(L"01") != edit1Result)
    {
        AfxMessageBox(Common::loadString(IDS_Err_InvalidAutoStart), MB_OK | MB_ICONERROR);
        return;
    }

    CString csInvalidInterval;
    csInvalidInterval.Format(Common::loadString(IDS_Err_InvalidInterval_FMT), ImeAutoSwitcher::defaultIntervalMs);

    if (edit2Result.SpanIncluding(L"0123456789") != edit2Result)
    {
        AfxMessageBox(csInvalidInterval, MB_OK | MB_ICONERROR);
    }

    bAutoStart = edit1Result == L"1";
    nInterval = std::stoi(edit2Result.GetString());

    if (nInterval < ImeAutoSwitcher::defaultIntervalMs)
    {
        AfxMessageBox(csInvalidInterval, MB_OK | MB_ICONERROR);
        return;
    }

    ImeAutoSwitcher::stop();
    if (bAutoStart)
    {
        ImeAutoSwitcher::start(nInterval);
    }
    ImeAutoSwitcher::saveSettings(bAutoStart, nInterval);
}

void Interface::cmdCloneText()
{
    CAcModuleResourceOverride resOverride;
    AcString asSrcTextContent;
    acutPrintf(L"\n%s", Common::loadString(IDS_Msg_CopySrcTextPrompt));
    if (!TextUtil::getSelectedTextRawContent(asSrcTextContent) || asSrcTextContent.isEmpty())
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }
    acutPrintf(L"\n读取到：%s\n", asSrcTextContent.constPtr());

    acutPrintf(L"\n%s", Common::loadString(IDS_Msg_PasteDstTextPrompt));
    UniversalPicker::run(
        &TextUtil::textClassList,
        [&asSrcTextContent](const AcDbObjectId& id)
        {
            TextUtil::updateTextEntityContent(id, asSrcTextContent);
        },
        Common::loadString(IDS_yxCloneTextCommandDescription),
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
    CString title = Common::loadString(IDS_yxBalloonNumberOffsetCommandDescription);
    GenericPairEditDlg dlg(title, Common::loadString(IDS_BalloonNumberOffsetLabel), Common::loadString(IDS_Prompt), false, true, true);
    dlg.modifyEditControl(L"", Common::loadString(IDS_BalloonNumberOffsetPromptInfo));

    CString edit1Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
    }
    else
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }

    if (edit1Result.IsEmpty())
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_Err_EmptyBalloonNumberOffset));
        return;
    }

    int offset = 0;
    try
    {
        size_t pos = 0;
        offset = std::stoi(edit1Result.GetString(), &pos);
        if (pos != edit1Result.GetLength())
        {
            throw std::exception();
        }
    }
    catch (...)
    {
        AfxMessageBox(Common::loadString(IDS_Err_InvalidInteger), MB_OK | MB_ICONERROR);
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
    CString title = Common::loadString(IDS_yxBalloonNumberFilterCommandDescription);
    GenericPairEditDlg dlg(title, Common::loadString(IDS_BalloonNumberFilterCondition), Common::loadString(IDS_Prompt), false, true, false);
    dlg.modifyEditControl(L"", Common::loadString(IDS_BalloonNumberFilterPrompt));

    CString edit1Result;
    if (dlg.DoModal() == IDOK)
    {
         edit1Result = dlg.getEdit1Result();
    }
    else
    {
        acutPrintf(L"\n%s", Common::loadString(IDS_CancelOperation));
        return;
    }

    if (edit1Result.GetLength() < 3)
    {
        AfxMessageBox(Common::loadString(IDS_Err_BalloonNumberFilterEmptyCriteria), MB_OK | MB_ICONERROR);
        return;
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

    CString strInputOpType = edit1Result.Left(2);
    CString strInputOpValue = edit1Result.Mid(2);
    auto it = std::find(opNoLimit.begin(), opNoLimit.end(), strInputOpType);
    if (it == opNoLimit.end()) // 不在无限制判定值的符号列表中，则要求判定值必须为证书
    {
        // 进一步判断是否输入了非法符号
        it = std::find(opNeedNumeric.begin(), opNeedNumeric.end(), strInputOpType);
        if (it == opNeedNumeric.end())
        {
            AfxMessageBox(Common::loadString(IDS_Err_BalloonNumberFilterInvalidOperatorType), MB_OK | MB_ICONERROR);
            return;
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
            AfxMessageBox(Common::loadString(IDS_Err_BalloonNumberFilterNeedInterger), MB_OK | MB_ICONERROR);
            return;
        }
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
        acutPrintf(Common::loadString(IDS_Msg_BalloonNumberFilterMatchCount_FMT), matchedIds.length());
    }
    else
    {
        acutPrintf(Common::loadString(IDS_Msg_BalloonNumberFilterNoMatch));
    }
}