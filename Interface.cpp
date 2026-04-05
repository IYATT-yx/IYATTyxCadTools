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

    // 输入法状态监控自启动处理
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
    acutPrintf(L"\n%s", acdbHostApplicationServices()->getMachineRegistryProductRootKey());
    return;
    static unsigned int currentNum = 1;
    double bubbleScale = 1.0;
    double offsetDistance = 7.0; // 气泡距离文字中心的偏移

    UniversalPicker::run(
        nullptr,
        [=](AcDbObjectId objId) mutable
        {
            AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForRead);
            if (pDim == nullptr)
            {
                return;
            }

            // 1. 获取文字中心和旋转角度（万能法：优先取 textRotation，特殊类型手动纠偏）
            AcGePoint3d textPt = pDim->textPosition();
            double finalRotation = pDim->textRotation();
            AcGeVector3d dirVec;

            // 处理不同类型的基准角度
            if (pDim->isKindOf(AcDbRotatedDimension::desc()))
            {
                finalRotation += AcDbRotatedDimension::cast(pDim)->rotation();
            }
            else if (pDim->isKindOf(AcDbAlignedDimension::desc()))
            {
                auto pAli = AcDbAlignedDimension::cast(pDim);
                finalRotation += (pAli->xLine2Point() - pAli->xLine1Point()).angleOnPlane(AcGePlane::kXYPlane);
            }
            else if (pDim->isKindOf(AcDbRadialDimension::desc()) || pDim->isKindOf(AcDbDiametricDimension::desc()))
            {
                // 径向标注：文字通常沿引线方向，若 textRotation 为 0，方向即为中心到文字点
                AcGePoint3d center = AcGePoint3d::kOrigin;
                if (auto pR = AcDbRadialDimension::cast(pDim)) center = pR->center();
                else if (auto pD = AcDbDiametricDimension::cast(pDim)) center = pD->chordPoint();

                AcGeVector3d radialDir = textPt - center;
                if (!radialDir.isZeroLength())
                {
                    finalRotation += radialDir.angleOnPlane(AcGePlane::kXYPlane);
                }
            }

            // 构造指向文字方向的矢量
            dirVec.set(cos(finalRotation), sin(finalRotation), 0.0);

            // 2. 计算插入点：在文字方向的反方向（左侧）
            AcGePoint3d insPt = textPt - (dirVec * offsetDistance);

            // 3. 插入块参照并填充属性
            Block::createBalloonNumberBlock();
            AcDbDatabase* pDb = pDim->database();
            AcDbBlockTable* pBT = nullptr;

            if (pDb->getBlockTable(pBT, AcDb::kForRead) == Acad::eOk)
            {
                AcDbObjectId blockDefId;
                if (pBT->getAt(Common::BalloonNumberBlock::blockName, blockDefId) == Acad::eOk)
                {
                    AcDbBlockReference* pBlkRef = new AcDbBlockReference(insPt, blockDefId);
                    pBlkRef->setScaleFactors(AcGeScale3d(bubbleScale));
                    pBlkRef->setRotation(finalRotation);

                    // --- 完整的属性填充逻辑 ---
                    AcDbBlockTableRecord* pBTR = nullptr;
                    if (acdbOpenObject(pBTR, blockDefId, AcDb::kForRead) == Acad::eOk)
                    {
                        AcDbBlockTableRecordIterator* pIt = nullptr;
                        pBTR->newIterator(pIt);
                        for (pIt->start(); !pIt->done(); pIt->step())
                        {
                            AcDbEntity* pEnt = nullptr;
                            if (pIt->getEntity(pEnt, AcDb::kForRead) == Acad::eOk)
                            {
                                AcDbAttributeDefinition* pAttDef = AcDbAttributeDefinition::cast(pEnt);
                                if (pAttDef && !pAttDef->isConstant() &&
                                    AcString(pAttDef->tag()) == Common::BalloonNumberBlock::AttTag)
                                {
                                    AcDbAttribute* pAtt = new AcDbAttribute();
                                    pAtt->setPropertiesFrom(pAttDef);
                                    // 必须基于块参照的变换矩阵设置属性位置
                                    pAtt->setAttributeFromBlock(pAttDef, pBlkRef->blockTransform());
                                    pAtt->setTextString(std::to_wstring(currentNum).c_str());

                                    pBlkRef->appendAttribute(pAtt);
                                    pAtt->close();
                                }
                                pEnt->close();
                            }
                        }
                        delete pIt;
                        pBTR->close();
                    }

                    // 提交到模型空间
                    AcDbBlockTableRecord* pMs = nullptr;
                    if (pBT->getAt(ACDB_MODEL_SPACE, pMs, AcDb::kForWrite) == Acad::eOk)
                    {
                        pMs->appendAcDbEntity(pBlkRef);
                        pMs->close();
                    }
                    pBlkRef->close();

                    currentNum++; // 仅在成功插入后递增
                }
                pBT->close();
            }
            pDim->close();
        },
        L"\n功能：全类型标注适配气泡\n",
        UniversalPicker::SelectMode::Immediate,
        true
    );
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
                AcString name = gtData.name[0];
                AcString value = gtData.value[0];
                AcString primary = gtData.primary[0];
                AcString secondary = gtData.secondary[0];
                AcString tertiary = gtData.tertiary[0];
                AcString row = name + value + primary + secondary + tertiary;
                acutPrintf(L"\n形位公差：%s", row.kACharPtr());
                std::vector<AcString> rows = { row, name, value, primary, secondary, tertiary };
                csv.writeRow(rows);

                if (gtData.gdtSymbolType[1] != Acm::kNoType)
                {
                    name = gtData.name[1];
                    value = gtData.value[1];
                    primary = gtData.primary[1];
                    secondary = gtData.secondary[1];
                    tertiary = gtData.tertiary[1];
                    row = name + value + primary + secondary + tertiary;
                    acutPrintf(L"\n形位公差：%s", row.kACharPtr());
                    rows = { row, name, value, primary, secondary, tertiary };
                    csv.writeRow( rows );

                    if (gtData.gdtSymbolType[2] != Acm::kNoType)
                    {
                        name = gtData.name[2];
                        value = gtData.value[2];
                        primary = gtData.primary[2];
                        secondary = gtData.secondary[2];
                        tertiary = gtData.tertiary[2];
                        row = name + value + primary + secondary + tertiary;
                        rows = { name, value, primary, secondary, tertiary };
                        acutPrintf(L"\n形位公差：%s", row.kACharPtr());
                        rows = { row, name, value, primary, secondary, tertiary };
                        csv.writeRow( rows );
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
    GenericPairEditDlg dlg(L"设置输入法语言自动切换", L"自启动：", L"检查间隔(ms)：", false, true, true);

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
        AfxMessageBox(L"必须输入自启动状态和检查间隔", MB_OK | MB_ICONERROR);
    }

    if (edit1Result.SpanIncluding(L"01") != edit1Result)
    {
        AfxMessageBox(L"自启动状态必须为 0 或 1，1表示自启动，0 表示不自启动", MB_OK | MB_ICONERROR);
        return;
    }

    if (edit2Result.SpanIncluding(L"0123456789") != edit2Result)
    {
        AfxMessageBox(L"检查间隔必须为非负整数，且不小于 200", MB_OK | MB_ICONERROR);
    }

    bAutoStart = edit1Result == L"1";
    nInterval = _wtoi(edit2Result);

    if (nInterval < 200)
    {
        AfxMessageBox(L"检查间隔必须不小于 200", MB_OK | MB_ICONERROR);
        return;
    }

    ImeAutoSwitcher::stop();
    if (bAutoStart)
    {
        ImeAutoSwitcher::start(nInterval);
    }
    ImeAutoSwitcher::saveSettings(bAutoStart, nInterval);
}

