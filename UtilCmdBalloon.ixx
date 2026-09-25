/**
 * @file      UtilCmdBalloon.ixx
 * @brief     气泡号命令
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "UiGenericPairEditDlg.hpp"

export module UtilCmdBalloon;
import FrameworkCommands;
import UtilBalloonNumber;
import UiUniversalPicker;
import FrameworkCommands;
import FrameworkTranslator;
import UtilAcadVar;
import UtilAnnotative;

namespace
{
    void cmdInsertBalloonNumberBlockWithStartNumber()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"插入带起始编号的气泡号");
        acutPrintf(L"\n%s\n", title);

        UiGenericPairEditDlg dlg(title, _(L"开始序号"), _(L"使用提示"), false, true, true);
        // 设置默认字高
        CString csTips;
        double TEXTSIZE;
        if (!UtilAcadVar::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
        {
            AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
            return;
        }
        double scale = UtilAnnotative::getCurrentScaleValue();
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
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        UtilBalloonNumber::createBalloonNumberBlock();
        UtilBalloonNumber::insertBalloonNumberBlockWithStartNumber(startNumber);
    }

    void cmdUpdateBalloonNumberBlock()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"更新气泡号");
        acutPrintf(L"\n%s\n", title);

        UiGenericPairEditDlg dlg(title, _(L"开始序号"), _(L"气泡序号字高"), true, true, true);

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
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        UiUniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

        acutPrintf(_(L"\n选中的气泡号将被设置为：%d"), startNumber);
        UiUniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (UtilBalloonNumber::updateBalloonNumberBlock(id, startNumber))
                {
                    ++startNumber;
                    acutPrintf(_(L"\n选中的气泡号将被设置为：%d"), startNumber);
                }
            },
            nullptr,
            UiUniversalPicker::SelectMode::Immediate,
            false,
            UiUniversalPicker::SortMode::RD,
            false
        );
    }

    void cmdBalloonNumberOffset()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"偏置气泡号");
        UiGenericPairEditDlg dlg(title, _(L"偏置值"), _(L"使用提示"), false, true, true);
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
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        UiUniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };
        UiUniversalPicker::run(
            &arcv,
            [&offset](const AcDbObjectId& id)
            {
                UtilBalloonNumber::balloonNumberOffset(id, offset);
            },
            title,
            UiUniversalPicker::SelectMode::Batch,
            false,
            UiUniversalPicker::SortMode::None,
            true
        );
    }

    void cmdBalloonNumberFilter()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"筛选气泡号");
        UiGenericPairEditDlg dlg(title, _(L"筛选条件"), _(L"使用提示"), false, true, false);
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
                std::vector<CString> opNoLimit = { UtilBalloonNumber::OperatorType::equal, UtilBalloonNumber::OperatorType::notEqual1, UtilBalloonNumber::OperatorType::notEqual2 };
                // 限制判定值为数字的列表
                std::vector<CString> opNeedNumeric =
                {
                    UtilBalloonNumber::OperatorType::greater1, UtilBalloonNumber::OperatorType::greater2,
                    UtilBalloonNumber::OperatorType::less1, UtilBalloonNumber::OperatorType::less2,
                    UtilBalloonNumber::OperatorType::greaterEqual1, UtilBalloonNumber::OperatorType::greaterEqual2,
                    UtilBalloonNumber::OperatorType::lessEqual1, UtilBalloonNumber::OperatorType::lessEqual2
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
                return UiGenericPairEditDlg::ValidatorOk;
            });

        if (dlg.DoModal() != IDOK)
        {
            acutPrintf(_(L"取消操作"));
            return;
        }

        AcDbObjectIdArray matchedIds;
        UiUniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

        UiUniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                AcString attrValue;
                if (UtilBalloonNumber::getBalloonAttributeValue(id, attrValue))
                {
                    if (UtilBalloonNumber::meetCriteria(attrValue, edit1Result.GetString()))
                    {
                        matchedIds.append(id);
                    }
                }
            },
            title,
            UiUniversalPicker::SelectMode::Batch,
            true,
            UiUniversalPicker::SortMode::None,
            true
        );

        if (matchedIds.length() > 0)
        {
            UiUniversalPicker::setSelection(matchedIds);
            acutPrintf(_(L"\n筛选完成：%d 个匹配项。"), matchedIds.length());
        }
        else
        {
            acutPrintf(_(L"\n未发现符合条件的气泡号。"));
        }
    }

    void cmdCheckBalloonNumberMaxMin()
    {
        UiUniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };
        AcString strValue;
        int max = INT_MIN;
        int min = INT_MAX;
        AcDbObjectId maxId = AcDbObjectId::kNull;
        AcDbObjectId minId = AcDbObjectId::kNull;
        AcDbObjectIdArray matchedIds;
        UiUniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (UtilBalloonNumber::getBalloonAttributeValue(id, strValue))
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
            UiUniversalPicker::SelectMode::Batch,
            true,
            UiUniversalPicker::SortMode::None,
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
            UiUniversalPicker::setSelection(resultIds);
            acutPrintf(_(L"\n最大气泡号：%d，最小气泡号：%d"), max, min);
        }
        else
        {
            acutPrintf(_(L"\n未发现有效数字格式的气泡号"));
        }
    }

    void cmdCheckDuplicateBalloonNumbers()
    {
        UiUniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

        std::map<AcString, AcDbObjectIdArray> numberMap;
        AcString strValue;

        UiUniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (UtilBalloonNumber::getBalloonAttributeValue(id, strValue))
                {
                    if (!strValue.isEmpty())
                    {
                        numberMap[strValue].append(id);
                    }
                }
            },
            _(L"检查重复气泡号"),
            UiUniversalPicker::SelectMode::Batch,
            true,
            UiUniversalPicker::SortMode::None,
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
            UiUniversalPicker::setSelection(duplicateIds);
            acutPrintf(_(L"\n发现重复: %s"), reportMsg.constPtr());
        }
        else
        {
            acutPrintf(_(L"未发现重复"));
        }
    }

    void cmdCheckBalloonNumberBreakpoints()
    {
        UiUniversalPicker::AcRxClassVector arcv = { AcDbBlockReference::desc() };

        // 使用 set 自动去重并升序排序
        std::set<int> numbers;
        AcString strValue;

        UiUniversalPicker::run(
            &arcv,
            [&](const AcDbObjectId& id)
            {
                if (UtilBalloonNumber::getBalloonAttributeValue(id, strValue))
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
            UiUniversalPicker::SelectMode::Batch,
            true,
            UiUniversalPicker::SortMode::None,
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

    FrameworkCommands::AutoRegister ar =
    {
        { L"yxInsertBalloonNumberBlockWithStartNumber", []() { return  _(L"插入带起始编号的气泡号"); }, FrameworkCommands::CommandFlags::Base, cmdInsertBalloonNumberBlockWithStartNumber },
        { L"yxUpdateBalloonNumberBlock", []() { return  _(L"更新气泡号"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdUpdateBalloonNumberBlock },
        { L"yxBalloonNumberOffset", []() { return  _(L"偏置气泡号"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdBalloonNumberOffset },
        { L"yxBalloonNumberFilter", []() { return  _(L"筛选气泡号"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdBalloonNumberFilter },
        { L"yxCheckBalloonNumberMaxMin", []() { return  _(L"查找气泡号最大和最小序号"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdCheckBalloonNumberMaxMin },
        { L"yxCheckDuplicateBalloonNumbers", []() { return  _(L"检查重复气泡号"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdCheckDuplicateBalloonNumbers },
        { L"yxCheckBalloonNumberBreakpoints", []() { return  _(L"检查气泡号断点"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdCheckBalloonNumberBreakpoints },
    };
}