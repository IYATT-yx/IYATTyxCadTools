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

void Interface::init()
{
    int baseFlags = ACRX_CMD_MODAL;
    int pickFlags = ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET;
    Common::registerYxCmd(L"yx", baseFlags, Interface::cmdHelp);
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
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：固定选中标注的文本\n";
    UniversalPicker::run(filter.kACharPtr(), Dimension::dimensionFix, prompt);
}

void Interface::cmdDimensionResume()
{
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：清空选中标注的文本，恢复关联标注。注意可能造成手动编辑的符号、公差等丢失。\n";
    UniversalPicker::run(filter.kACharPtr(), Dimension::dimensionResume, prompt);
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

    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：在标注首末位置添加符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    acutPrintf(L"\n开始 %d %d \n", isLGdt, isRGdt);
    UniversalPicker::run(
        filter.kACharPtr(),
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

    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：在标注首末位置移除符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    bool isLGdt = dlg.getGdtCheckStatus(0);
    bool isRGdt = dlg.getGdtCheckStatus(1);
    UniversalPicker::run(
        filter.kACharPtr(),
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
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：设置理论尺寸框\n";
    UniversalPicker::run(
        filter.kACharPtr(),
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
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：取消理论尺寸框\n";
    UniversalPicker::run(
        filter.kACharPtr(),
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
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：设置参考尺寸括号\n";
    UniversalPicker::run(
        filter.kACharPtr(),
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
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    const ACHAR* prompt = L"\n功能：取消参考尺寸括号\n";
    UniversalPicker::run(
        filter.kACharPtr(),
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
    GenericPairEditDlg dlg(L"设置序号块起始序号", L"起始序号：", L"", true, true);

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

    Block::createSerialNumberBlock();
    Block::insertSerialNumberBlockWithStartNumber(startNum);
}
