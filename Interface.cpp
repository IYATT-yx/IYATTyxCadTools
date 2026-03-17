#include "stdafx.h"
#include "Interface.hpp"

#include "UniversalPicker.hpp"
#include "BuildingTime.hpp"
#include "EntityStyle.hpp"
#include "Dimension.hpp"
#include "GenericPairEditDlg.hpp"

void Interface::init()
{
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxHelp", L"yxHelp", ACRX_CMD_MODAL, Interface::cmdHelp);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxSetByLayer", L"yxSetByLayer", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdSetByLayer);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxSBL", L"yxSBL", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdSetByLayer);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxDimensionFix", L"yxDimensionFix", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdDimensionFix);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxDF", L"yxDF", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdDimensionFix);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxDimensionResume", L"yxDimensionResume", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdDimensionResume);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxDR", L"yxDR", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdDimensionResume);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxAddSurroundingCharsForDimension", L"yxAddSurroundingCharsForDimension", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdiAddSurroundingCharsForDimension);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxASCFD", L"yxASCFD", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdiAddSurroundingCharsForDimension);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxRemoveSurroundingCharsForDimension", L"yxRemoveSurroundingCharsForDimension", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdiRemoveSurroundingCharsForDimension);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxRSCFD", L"yxRSCFD", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdiRemoveSurroundingCharsForDimension);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxSetBasicBox", L"yxSetBasicBox", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdSetBasicBox);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxSBB", L"yxSBB", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdSetBasicBox);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxUnsetBasicBox", L"yxUnsetBasicBox", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdUnsetBasicBox);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxUBB", L"yxUBB", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdUnsetBasicBox);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxSetRefDim", L"yxSetRefDim", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdSetRefDim);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxSRD", L"yxSRD", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdSetRefDim);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxUnsetRefDim", L"yxUnsetRefDim", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdUnsetRefDim);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxURD", L"yxURD", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdUnsetRefDim);
}

void Interface::unload()
{
	acedRegCmds->removeGroup(L"IYATTyxCadTools");
}

void Interface::cmdHelp()
{
    acutPrintf(L"\nCAD 工具箱  %s %s\n", BuildingTime::WDATE, BuildingTime::WTIME);
    acutPrintf(L"作者：IYATT-yx\n");
    acutPrintf(L"项目开源地址：https://github.com/IYATT-yx/IYATTyxCadTools\n");
}

void Interface::cmdSetByLayer()
{
    UniversalPicker::Options options;
    const ACHAR* prompt = L"\n功能：设置选中实体的图层属性为ByLayer。\n";
    UniversalPicker::run(options, EntityStyle::setByLayer, prompt);
}

void Interface::cmdDimensionFix()
{
    AcString filterAcString;
    filterAcString.format(L"%s", AcDbDimension::desc()->dxfName());

    UniversalPicker::Options options
    {
        .filter = filterAcString.kACharPtr()
    };
    const ACHAR* prompt = L"\n功能：固定选中标注的文本\n";
    UniversalPicker::run(options, Dimension::dimensionFix, prompt);
}

void Interface::cmdDimensionResume()
{
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());

    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr(),
    };
    const ACHAR* prompt = L"\n功能：清空选中标注的文本，恢复关联标注。注意可能造成手动编辑的符号、公差等丢失。\n";
    UniversalPicker::run(options, Dimension::dimensionResume, prompt);
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
    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr()
    };

    const ACHAR* prompt = L"\n功能：在标注首末位置添加符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    UniversalPicker::run(
        options,
        [left, right](AcDbObjectId objId)
        {
            Dimension::addSurroundingCharsForDimension(objId, left, right);
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
    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr(),
    };

    const ACHAR* prompt = L"\n功能：在标注首末位置移除符号\n";
    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    UniversalPicker::run(
        options,
        [left, right](AcDbObjectId objId)
        {
            Dimension::removeSurroundingCharsForDimension(objId, left, right);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}

void Interface::cmdSetBasicBox()
{
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());
    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr(),
    };
    const ACHAR* prompt = L"\n功能：设置理论尺寸框\n";
    UniversalPicker::run(
        options,
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
    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr(),
    };
    const ACHAR* prompt = L"\n功能：取消理论尺寸框\n";
    UniversalPicker::run(
        options,
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
    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr(),
    };
    const ACHAR* prompt = L"\n功能：设置参考尺寸括号\n";
    UniversalPicker::run(
        options,
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
    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr(),
    };
    const ACHAR* prompt = L"\n功能：取消参考尺寸括号\n";
    UniversalPicker::run(
        options,
        [](AcDbObjectId objId)
        {
            Dimension::setAndUnsetRefDim(objId, false);
        },
        prompt,
        UniversalPicker::SelectMode::Immediate
    );
}
