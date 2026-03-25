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
    Common::registerYxCmd(L"yxInsertSerialNumberBlockWithStartNumber", baseFlags, Interface::cmdInsertSerialNumberBlockWithStartNumber);
    Common::registerYxCmd(L"yxPrintClassHierarchy", baseFlags, Interface::cmdPrintClassHierarchy);
    Common::registerYxCmd(L"yxExtractAnnotations", baseFlags, Interface::cmdExtractAnnotations);
    AcDbDimension::desc();
}

// 测试使用
void Interface::test()
{
    const CString filePath = "C:\\Users\\iyatt\\Desktop\\test.csv";
    CsvWriter csv(filePath);

    UniversalPicker::run(
        nullptr,
        [&csv](AcDbObjectId objId) {
            Dimension::DimensionData dimData{};
            Dimension::readDim(objId, dimData);
            GeometricTolerance::GeometricToleranceData gtData{};
            GeometricTolerance::readFcf(objId, gtData);

            if (dimData.status)
            {
                double measuredValue = dimData.measuredValue;
                if (dimData.isAngle)
                {
                    measuredValue = dimData.degreeValue();
                }
                AcString measuredValueText;
                measuredValueText.format(L"%f", measuredValue);
                AcString tolUpperText;
                tolUpperText.format(L"%f", dimData.tolUpper);
                AcString tolLowerText;
                tolLowerText.format(L"%f", dimData.tolLower);
                std::vector<AcString> dim{measuredValueText, tolUpperText, tolLowerText};
                csv.writeRow(dim);
            }
            else if (gtData.status)
            {
                std::vector<AcString> fcf{gtData.name[0], gtData.value[0], gtData.primary[0], gtData.secondary[0], gtData.tertiary[0]};
                csv.writeRow(fcf);

                fcf = {gtData.name[1], gtData.value[1], gtData.primary[1], gtData.secondary[1], gtData.tertiary[1]};
                csv.writeRow(fcf);
            }
        },
        L"test",
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::unload()
{
	acedRegCmds->removeGroup(Common::commandGroup);
}

void Interface::cmdHelp()
{
    acutPrintf(L"\nCAD 工具箱  %s %s\n", BuildingTime::WDATE, BuildingTime::WTIME);
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
    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：固定选中标注的文本\n";
    UniversalPicker::run(filter, Dimension::dimensionFix, prompt);
}

void Interface::cmdDimensionResume()
{
    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：清空选中标注的文本，恢复关联标注。注意可能造成手动编辑的符号、公差等丢失。\n";
    UniversalPicker::run(filter, Dimension::dimensionResume, prompt);
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

    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：在标注首末位置添加符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    acutPrintf(L"\n开始 %d %d \n", isLGdt, isRGdt);
    UniversalPicker::run(
        filter,
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
        return;
    }

    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：在标注首末位置移除符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    UniversalPicker::run(
        filter,
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
    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：设置理论尺寸框\n";
    UniversalPicker::run(
        filter,
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
    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：取消理论尺寸框\n";
    UniversalPicker::run(
        filter,
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
    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：设置参考尺寸括号\n";
    UniversalPicker::run(
        filter,
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
    const ACHAR* filter = AcDbDimension::desc()->dxfName();
    const ACHAR* prompt = L"\n功能：取消参考尺寸括号\n";
    UniversalPicker::run(
        filter,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, false);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdInsertSerialNumberBlockWithStartNumber()
{
    acutPrintf(L"\n功能：插入序号块，并自动递增序号\n");

    CAcModuleResourceOverride resOverride;
    GenericPairEditDlg dlg(L"设置序号块起始序号", L"起始序号：", L"序号字高：", false, true);
    // 设置默认字高
    CString csTextHeight;
    csTextHeight.Format(L"%f", Common::SerialNumberCircleBlock::defaultTextHeight);
    dlg.modifyEditControl(L"", csTextHeight);

    CString edit1Result, edit2Result;
    if (dlg.DoModal() == IDOK)
    {
        edit1Result = dlg.getEdit1Result();
        edit2Result = dlg.getEdit2Result();
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

    if (edit2Result.IsEmpty())
    {
        AfxMessageBox(L"必须输入序号字高", MB_OK | MB_ICONERROR);
        return;
    }

    if (edit2Result.SpanIncluding(L"0123456789.") != edit2Result)
    {
        AfxMessageBox(L"序号字高必须为非负数", MB_OK | MB_ICONERROR);
        return;
    }

    double dTextHeight = _wtof(edit2Result);
    if (dTextHeight <= 0)
    {
        AfxMessageBox(L"序号字高必须为非负数", MB_OK | MB_ICONERROR);
        return;
    }

    // 序号块比例
    double dScale = dTextHeight / Common::SerialNumberCircleBlock::defaultTextHeight;

    Block::createSerialNumberBlock();
    Block::insertSerialNumberBlockWithStartNumber(startNum, dScale);
}

void Interface::cmdPrintClassHierarchy()
{
    const ACHAR* prompt = L"\n功能：打印类层级关系\n";
    UniversalPicker::run(nullptr, Common::printClassHierarchy, prompt, UniversalPicker::SelectMode::Immediate);
}

void Interface::cmdExtractAnnotations()
{
    acutPrintf(L"\n请选择提取标注后保存的文件。");
    CString filePath = Common::ShowSaveFileDialog(L"选择提取标注保存的文件路径");
    if (filePath.IsEmpty())
    {
        acutPrintf(L"取消");
        return;
    }

    CsvWriter csv(filePath);
    if (!csv.isValid())
    {
        AfxMessageBox(L"文件路径打开失败，请检查是否被占用或不", MB_OK | MB_ICONERROR);
        return;
    }

    const ACHAR* prompt = L"\n功能：提取标注到 csv 文件\n";
    AcString filter;
    filter.format(L"%s\0%s", AcDbDimension::desc()->dxfName(), AcmFCF::desc()->dxfName());
    UniversalPicker::run(
        filter.kACharPtr(),
        [&csv](AcDbObjectId objId)
        {
            Dimension::DimensionData dimData{};
            Dimension::readDim(objId, dimData);
            GeometricTolerance::GeometricToleranceData gtData{};
            GeometricTolerance::readFcf(objId, gtData);

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
                // 公差
                //acutPrintf(L"\n调试：%s %s %f %f ", dimData.prefix.kACharPtr(), dimData.suffix.kACharPtr(), dimData.tolUpper, dimData.tolLower);
                AcString asTol;
                if (dimData.tolUpper == 0 && dimData.tolLower == 0)
                {
                    asTol = L"";
                }
                else
                {
                    if (abs(dimData.tolUpper + dimData.tolLower) < 1e-6)
                    {
                        double dAbsTol = abs(dimData.tolUpper);
                        asTol.format(L"%s%.*f", Common::Symbols::PlusMinus, dimData.tolPrecision, dAbsTol);
                    }
                    else
                    {
                        AcString asTolUpper, asTolLower;
                        Common::double2AcString(dimData.tolUpper, asTolUpper, dimData.tolPrecision);
                        Common::double2AcString(dimData.tolLower, asTolLower, dimData.tolPrecision);
                        asTol.format(L"(+%s/%s)", asTolUpper.kACharPtr(), asTolLower.kACharPtr());
                    }
                }
                AcString asPrefix = Common::getSymbol(dimData.prefix);
                AcString asSuffix = Common::getSymbol(dimData.suffix);
                AcString asDimText = asPrefix + asMeasuredValue + asTol + asSuffix;
                acutPrintf(L"\n尺寸：%s", asDimText.kACharPtr());
                std::vector<AcString> row = { asDimText };
                csv.writeRow(row);
            }
            else if (gtData.status) // 形位公差
            {
                AcString row;
                row.format(L"%s%s%s%s%s", gtData.name[0].kACharPtr(), gtData.value[0].kACharPtr(), gtData.primary[0].kACharPtr(), gtData.secondary[0].kACharPtr(), gtData.tertiary[0].kACharPtr());
                acutPrintf(L"\n形位公差：%s", row.kACharPtr());
                std::vector<AcString> rows = { row };
                csv.writeRow(rows);

                if (gtData.gdtSymbolType[1] != Acm::kNoType)
                {
                    row.format(L"%s%s%s%s%s", gtData.name[1].kACharPtr(), gtData.value[1].kACharPtr(), gtData.primary[1].kACharPtr(), gtData.secondary[1].kACharPtr(), gtData.tertiary[1].kACharPtr());
                    acutPrintf(L"\n形位公差：%s", row.kACharPtr());
                    rows = { row };
                    csv.writeRow( rows );

                    if (gtData.gdtSymbolType[2] != Acm::kNoType)
                    {
                        row.format(L"%s%s%s%s%s", gtData.name[2].kACharPtr(), gtData.value[2].kACharPtr(), gtData.primary[2].kACharPtr(), gtData.secondary[2].kACharPtr(), gtData.tertiary[2].kACharPtr());
                        acutPrintf(L"\n形位公差：%s", row.kACharPtr());
                        rows = { row };
                        csv.writeRow( rows );
                    }
                }
            }
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}
