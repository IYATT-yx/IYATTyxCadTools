module;
#include "stdafx.h"
#include "GenericPairEditDlg.hpp"

module Interface;

import EntityStyle;
import BuildingTime;
import UniversalPicker;
import Dimension;
import Common;
import Block;
import CsvWriter;
import GeometricTolerance;
import TextUtil;
import ImeAutoSwitcher;


void Interface::init()
{
    int baseFlags = ACRX_CMD_MODAL;
    int pickFlags = ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET;
    Common::registerYxCmd(L"yx", baseFlags, Interface::cmdHelp);
    Common::registerYxCmd(L"yxTest", baseFlags, test);
    Common::registerYxCmd(L"yxSetByLayer", pickFlags, Interface::cmdSetByLayer);
    Common::registerYxCmd(L"yxDimensionFx", pickFlags, Interface::cmdDimensionFix);
    Common::registerYxCmd(L"yxDimensionResume", pickFlags, Interface::cmdDimensionResume);
    Common::registerYxCmd(L"yxAddSurroundingCharsForDimension", pickFlags, Interface::cmdiAddSurroundingCharsForDimension);
    Common::registerYxCmd(L"yxRemoveSurroundingCharsForDimension", pickFlags, Interface::cmdiRemoveSurroundingCharsForDimension);
    Common::registerYxCmd(L"yxSetBasicBox", pickFlags, Interface::cmdSetBasicBox);
    Common::registerYxCmd(L"yxUnsetBasicBox", pickFlags, Interface::cmdUnsetBasicBox);
    Common::registerYxCmd(L"yxSetRefDim", pickFlags, Interface::cmdSetRefDim);
    Common::registerYxCmd(L"yxUnsetRefDim", pickFlags, Interface::cmdUnsetRefDim);
    Common::registerYxCmd(L"yxInsertBalloonNumberBlockWithStartNumber", baseFlags, Interface::cmdInsertBalloonNumberBlockWithStartNumber);
    Common::registerYxCmd(L"yxPrintClassHierarchy", baseFlags, Interface::cmdPrintClassHierarchy);
    Common::registerYxCmd(L"yxExtractAnnotations", baseFlags, Interface::cmdExtractAnnotations);
    Common::registerYxCmd(L"yxUpdateBalloonNumberBlock", baseFlags, Interface::cmdUpdateBalloonNumberBlock);
    Common::registerYxCmd(L"yxImeAutoSwitch", baseFlags, Interface::cmdImeAutoSwitch);

    // 输入法自动切换自启动 
    bool bAutoStart = false;
    int nAutoMonitorInterval = 200;
    ImeAutoSwitcher::loadSettings(bAutoStart, nAutoMonitorInterval);
    if (bAutoStart)
    {
        ImeAutoSwitcher::start(nAutoMonitorInterval);
    }
}

// 测试使用
void Interface::test()
{
}

void Interface::unload()
{
	acedRegCmds->removeGroup(Common::commandGroup);
}

void Interface::cmdHelp()
{
    acutPrintf(L"\nCAD 工具箱  %s_%s\n", BuildingTime::WDATE, BuildingTime::WTIME);
    acutPrintf(L"作者：IYATT-yx\n");
    acutPrintf(L"项目开源地址：https://github.com/IYATT-yx/IYATTyxCadTools\n");
}

void Interface::cmdSetByLayer()
{
    const ACHAR* prompt = L"\n功能：设置选中实体的图层属性为ByLayer。\n";
    UniversalPicker::run(nullptr, EntityStyle::setByLayer, prompt);
}

void Interface::cmdDimensionFix()
{
    const ACHAR* prompt = L"\n功能：固定选中标注的文本\n";
    UniversalPicker::run(&Common::DimensionSubClasses, Dimension::dimensionFix, prompt);
}

void Interface::cmdDimensionResume()
{
    const ACHAR* prompt = L"\n功能：清空选中标注的文本，恢复关联标注。注意可能造成手动编辑的符号、公差等丢失。\n";
    UniversalPicker::run(&Common::DimensionSubClasses, Dimension::dimensionResume, prompt);
}

void Interface::cmdiAddSurroundingCharsForDimension()
{
    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(L"在标注首末位置添加符号", L"首位符号：", L"末位符号：");

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

    const ACHAR* prompt = L"\n功能：在标注首末位置添加符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    acutPrintf(L"\n开始 %d %d \n", isLGdt, isRGdt);

    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [left, right, isLGdt, isRGdt](AcDbObjectId objId)
        {
            Dimension::addSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdiRemoveSurroundingCharsForDimension()
{
    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(L"在标注首末位置移除符号", L"首位符号：", L"末位符号：");

    CString edit1Result;
    CString edit2Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
        edit2Result = dlg.getEdit2Result();
    }
    else
    {
        acutPrintf(L"\n取消操作");
        return;
    }

    const ACHAR* prompt = L"\n功能：在标注首末位置移除符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [left, right, isLGdt, isRGdt](AcDbObjectId objId)
        {
            Dimension::removeSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdSetBasicBox()
{
    const ACHAR* prompt = L"\n功能：设置理论尺寸框\n";
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetBasicBox(objId, true);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdUnsetBasicBox()
{
    const ACHAR* prompt = L"\n功能：取消理论尺寸框\n";
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetBasicBox(objId, false);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdSetRefDim()
{
    const ACHAR* prompt = L"\n功能：设置参考尺寸括号\n";
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, true);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdUnsetRefDim()
{
    const ACHAR* prompt = L"\n功能：取消参考尺寸括号\n";
    UniversalPicker::run(
        &Common::DimensionSubClasses,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, false);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdInsertBalloonNumberBlockWithStartNumber()
{
    acutPrintf(L"\n功能：插入气泡号，自动递增序号\n");

    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(L"设置气泡号起始序号", L"起始序号：", L"提示：", false, true, true);
    // 设置默认字高
    CString csTips;
    csTips.Format(L"默认序号字体高度（圆圈半径）：%g，气泡缩放比例由注释性比例控制", Common::BalloonNumberBlock::defaultTextHeight);
    dlg.modifyEditControl(L"", csTips);

    CString edit1Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
    }
    else
    {
        return;
    }

    if (edit1Result.IsEmpty())
    {
        AfxMessageBox(L"必须输入起始序号", MB_OK | MB_ICONERROR);
        return;
    }

    if (edit1Result.SpanIncluding(L"0123456789") != edit1Result)
    {
        AfxMessageBox(L"起始序号必须为非 0 整数", MB_OK | MB_ICONERROR);
        return;
    }
    int startNum = _wtoi(edit1Result);
    Block::createBalloonNumberBlock();
    Block::insertBalloonNumberBlockWithStartNumber(startNum);
}

void Interface::cmdPrintClassHierarchy()
{
    const ACHAR* prompt = L"\n功能：打印类层级关系\n";
    UniversalPicker::run(nullptr, Common::printClassHierarchy, prompt, UniversalPicker::SelectMode::Immediate, true);
}

void Interface::cmdExtractAnnotations()
{
    acutPrintf(L"\n请选择提取标注后保存的文件。");
    CString filePath = Common::ShowSaveFileDialog(L"选择提取标注保存的文件路径");
    if (filePath.IsEmpty())
    {
        acutPrintf(L"\n取消。");
        return;
    }

    CsvWriter csv(filePath);
    if (!csv.isValid())
    {
        AfxMessageBox(L"文件路径打开失败，请检查是否被占用或不", MB_OK | MB_ICONERROR);
        return;
    }

    const ACHAR* prompt = L"\n功能：提取标注到 csv 文件\n";
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
                
                //acutPrintf(L"\n调试：%s %s %f %f ", dimData.prefix.kACharPtr(), dimData.suffix.kACharPtr(), dimData.tolUpper, dimData.tolLower);
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
                        asTol.format(L"%s%.*g", Common::Symbols::PlusMinus, dimData.tolPrecision, dAbsTol);
                        asTolUpper.format(L"%.*g", dimData.tolPrecision, dAbsTol);
                        asTolLower.format(L"-%.*g", dimData.tolPrecision, dAbsTol);
                    }
                    else // 双向公差
                    {
                        
                        Common::double2AcString(dimData.tolUpper, asTolUpper, dimData.tolPrecision);
                        Common::double2AcString(dimData.tolLower, asTolLower, dimData.tolPrecision);
                        asTol.format(L"+%s/%s", asTolUpper.kACharPtr(), asTolLower.kACharPtr());
                        asTolUpper.format(L"%.*g", dimData.tolPrecision, dimData.tolUpper);
                        asTolLower.format(L"%.*g", dimData.tolPrecision, dimData.tolLower);
                    }
                }
                AcString asPrefix = Common::getSymbol(dimData.prefix);
                AcString asSuffix = Common::getSymbol(dimData.suffix);
                AcString asDimText = asPrefix + asMeasuredValue + asTol + asSuffix;
                acutPrintf(L"\n尺寸：%s", asDimText.kACharPtr());
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
                        AcString row = name + " " + value + " " + primary + " " + secondary + " " + tertiary;
                        acutPrintf(L"\n形位公差：%s", row.kACharPtr());
                        std::vector<AcString> asvRow = { row, name, value, primary, secondary, tertiary };
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
                acutPrintf(L"\n多行文本：%s", asMText.kACharPtr());
                std::vector<AcString> rows = { asMText };
                csv.writeRow(rows);
            }
            else if (TextUtil::readDText(objId, asDText))
            {
                acutPrintf(L"\n单行文本：%s", asDText.kACharPtr());
                std::vector<AcString> rows = { asDText };
                csv.writeRow(rows);
            }
        },
        prompt,
        UniversalPicker::SelectMode::Immediate,
        false,
        UniversalPicker::SortMode::RD,
        false
    );
}

void Interface::cmdUpdateBalloonNumberBlock()
{
    acutPrintf(L"\n功能：更新气泡号块，并自动递增序号\n");

    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(L"设置气泡号块起始序号", L"起始序号：", L"序号字高：", true, true, true);

    CString edit1Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
    }
    else
    {
        acutPrintf(L"\n取消操作");
        return;
    }

    if (edit1Result.IsEmpty())
    {
        AfxMessageBox(L"必须输入起始序号", MB_OK | MB_ICONERROR);
        return;
    }

    if (edit1Result.SpanIncluding(L"0123456789") != edit1Result)
    {
        AfxMessageBox(L"起始序号必须为非 0 整数", MB_OK | MB_ICONERROR);
        return;
    }
    int startNum = _wtoi(edit1Result);

    acutPrintf(L"\n选中的气泡号块将被设置为：%d", startNum);
    UniversalPicker::run(
        nullptr,
        [&startNum](const AcDbObjectId& id)
        {
            if (Block::updateBalloonNumberBlock(id, startNum))
            {
                ++startNum;
                acutPrintf(L"\n选中的气泡号块将被设置为：%d", startNum);
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
    GenericPairEditDlg dlg(L"设置输入法自动切换", L"自启动：", L"切换间隔(ms)：", false, true, true);

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
        acutPrintf(L"\n取消操作");
        return;
    }

    if (edit1Result.IsEmpty() || edit2Result.IsEmpty())
    {
        AfxMessageBox(L"必须输入自启动状态和切换间隔", MB_OK | MB_ICONERROR);
    }

    if (edit1Result.SpanIncluding(L"01") != edit1Result)
    {
        AfxMessageBox(L"自启动状态必须为 0 或 1，1表示自启动，0 表示不自启动", MB_OK | MB_ICONERROR);
        return;
    }

    if (edit2Result.SpanIncluding(L"0123456789") != edit2Result)
    {
        AfxMessageBox(L"切换间隔必须为非负整数，且不小于 200", MB_OK | MB_ICONERROR);
    }

    bAutoStart = edit1Result == L"1";
    nInterval = _wtoi(edit2Result);

    if (nInterval < 200)
    {
        AfxMessageBox(L"切换间隔不能小于 200", MB_OK | MB_ICONERROR);
        return;
    }

    ImeAutoSwitcher::stop();
    if (bAutoStart)
    {
        ImeAutoSwitcher::start(nInterval);
    }
    ImeAutoSwitcher::saveSettings(bAutoStart, nInterval);
}

