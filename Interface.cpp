#include "stdafx.h"
#include "Interface.hpp"

#include "UniversalPicker.hpp"
#include "BuildingTime.hpp"
#include "EntityStyle.hpp"
#include "Dimension.hpp"
//
#include "GenericPairEditDlg.hpp"
//

void Interface::init()
{
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxHelp", L"yxHelp", ACRX_CMD_MODAL, Interface::cmdHelp);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxSetByLayer", L"yxSetByLayer", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdSetByLayer);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxDimensionFix", L"yxDimensionFix", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdDimensionFix);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxDimensionResume", L"yxDimensionResume", ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, Interface::cmdDimensionResume);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxAddSurroundingCharsForDimension", L"yxAddSurroundingCharsForDimension", ACRX_CMD_MODAL, Interface::cmdAddSurroundingCharsForDimension);
    acedRegCmds->addCommand(L"IYATTyxCadTools", L"yxRemoveSurroundingCharsForDimension", L"yxRemoveSurroundingCharsForDimension", ACRX_CMD_MODAL, Interface::cmdRemoveSurroundingCharsForDimension);
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
    UniversalPicker::Options options
    {
        .prompt = L"\n功能：设置选中实体的图层属性为ByLayer。\n"
    };
    UniversalPicker::run(options, EntityStyle::setByLayer);
}

void Interface::cmdDimensionFix()
{
    AcString filterAcString;
    filterAcString.format(L"%s", AcDbDimension::desc()->dxfName());

    UniversalPicker::Options options
    {
        .filter = filterAcString.kACharPtr(),
        .prompt = L"\n功能：固定选中标注的文本\n"
    };
    UniversalPicker::run(options, Dimension::dimensionFix);
}

void Interface::cmdDimensionResume()
{
    AcString filter;
    filter.format(L"%s", AcDbDimension::desc()->dxfName());

    UniversalPicker::Options options
    {
        .filter = filter.kACharPtr(),
        .prompt = L"\n功能：清空选中标注的文本，恢复关联标注。注意可能造成手动编辑的符号、公差等丢失。\n",
        .mode = UniversalPicker::SelectMode::Immediate
    };
    UniversalPicker::run(options, Dimension::dimensionResume);
}

void Interface::cmdAddSurroundingCharsForDimension()
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
        .filter = filter.kACharPtr(),
        .prompt = L"\n功能：在标注首末位置添加符号\n",
        .mode = UniversalPicker::SelectMode::Immediate
    };

    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    UniversalPicker::run(
        options,
        [left, right](AcDbObjectId objId)
        {
            Dimension::addSurroundingCharsForDimension(objId, left, right);
        }
    );
}

void Interface::cmdRemoveSurroundingCharsForDimension()
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
        .prompt = L"\n功能：在标注首末位置移除符号\n",
        .mode = UniversalPicker::SelectMode::Immediate
    };

    const ACHAR* left = edit1Result.GetString();
    const ACHAR* right = edit2Result.GetString();
    UniversalPicker::run(
        options,
        [left, right](AcDbObjectId objId)
        {
            Dimension::removeSurroundingCharsForDimension(objId, left, right);
        }
    );
}
