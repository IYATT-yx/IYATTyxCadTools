/**
 * @file      BalloonNumber.cpp
 * @brief     气泡号模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "GenericPairEditDlg.hpp"

module BalloonNumber;
import std;
import Common;
import Annotative;
import AcadVarUtil;
import Translator;
import Commands;
import UniversalPicker;

namespace BalloonNumber
{
    BalloonNumberJig::BalloonNumberJig(unsigned int num) : mNum(num)
    {
        this->mCurPt = AcGePoint3d::kOrigin;

        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
        AcDbBlockTable* pBlockTable = nullptr;
        if (pDb->getBlockTable(pBlockTable, AcDb::kForRead) != Acad::eOk)
        {
            AfxMessageBox(_(L"获取块表失败！"), MB_OK | MB_ICONERROR);
            throw WException(_(L"获取块表失败！"));
        }
        if (pBlockTable->getAt(Common::BalloonNumberBlock::getBlockName(), this->mBlockDefineId) != Acad::eOk)
        {
            pBlockTable->close();
            AfxMessageBox(L"获取块定义失败！", MB_OK | MB_ICONERROR);
            throw WException(_(L"获取块定义失败！"));
        }
        pBlockTable->close();

        this->mpBlockReference = new AcDbBlockReference();
        this->mpBlockReference->setBlockTableRecord(this->mBlockDefineId);
        this->mpBlockReference->setPosition(this->mCurPt);
        //this->mpBlockReference->setScaleFactors(AcGeScale3d(1.0)); // 缩放比例

        Annotative::applyCurrentAnnotativeScale(this->mpBlockReference); // 应用当前注释性比例
        BalloonNumber::syncAttributesFromDefinition(this->mpBlockReference, num); // 同步属性值

        this->setupAttributes();
    }

    BalloonNumberJig::~BalloonNumberJig()
    {
        if (this->mpBlockReference)
        {
            this->mpBlockReference->close();
            this->mpBlockReference = nullptr;
        }
    }

    AcEdJig::DragStatus BalloonNumberJig::sampler()
    {
        return acquirePoint(this->mCurPt);
    }

    Adesk::Boolean BalloonNumberJig::update()
    {
        // 更新块参照位置
        this->mpBlockReference->setPosition(this->mCurPt);

        // 获取块参照的变换矩阵
        AcGeMatrix3d mat = this->mpBlockReference->blockTransform();

        // 更新属性显示（仅位置，不改文字）
        AcDbObjectIterator* pAttIt = this->mpBlockReference->attributeIterator();
        if (pAttIt)
        {
            for (; !pAttIt->done(); pAttIt->step())
            {
                AcDbAttribute* pAtt = AcDbAttribute::cast(pAttIt->entity());
                if (pAtt)
                {
                    // 仅把属性放到块定义原点经过块变换的位置
                    AcGePoint3d basePt = AcGePoint3d::kOrigin;
                    pAtt->setPosition(basePt.transformBy(mat));
                }
            }
            delete pAttIt;
        }

        return Adesk::kTrue;
    }

    AcDbEntity* BalloonNumberJig::entity() const
    {
        return this->mpBlockReference;
    }

    AcGePoint3d BalloonNumberJig::getPoint() const
    {
        return this->mCurPt;
    }

    void BalloonNumberJig::setupAttributes()
    {
        AcDbBlockTableRecord* pBlockDefinition = Common::getObject<AcDbBlockTableRecord>(this->mBlockDefineId);
        if (pBlockDefinition == nullptr)
        {
            return;
        }
        AcDbBlockTableRecordIterator* pIt = nullptr;
        pBlockDefinition->newIterator(pIt);
        for (pIt->start(); !pIt->done(); pIt->step())
        {
            AcDbEntity* pEnt = nullptr;
            if (pIt->getEntity(pEnt, AcDb::kForRead) == Acad::eOk)
            {
                AcDbAttributeDefinition* pAttDef = AcDbAttributeDefinition::cast(pEnt);
                if (pAttDef && !pAttDef->isConstant() && AcString(pAttDef->tag()) == Common::BalloonNumberBlock::getAttTag())
                {
                    AcDbAttribute* pAtt = new AcDbAttribute();
                    pAtt->setPropertiesFrom(pAttDef);
                    pAtt->setAttributeFromBlock(pAttDef, this->mpBlockReference->blockTransform());
                    pAtt->setTextString(std::to_wstring(this->mNum).c_str());
                    this->mpBlockReference->appendAttribute(pAtt);
                    pAtt->close();
                }
                pEnt->close();
            }
        }
        delete pIt;
        pIt = nullptr;
    }
}

namespace BalloonNumber
{
	void createBalloonNumberBlock()
	{
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		AcDbBlockTable* pBlockTable;
		pDb->getBlockTable(pBlockTable, AcDb::kForWrite);

		// 检查块是否存在
		if (!pBlockTable->has(Common::BalloonNumberBlock::getBlockName()))
		{
			AcDbBlockTableRecord* pNewBTR = new AcDbBlockTableRecord();
			pNewBTR->setName(Common::BalloonNumberBlock::getBlockName());
			pNewBTR->setOrigin(AcGePoint3d::kOrigin);
            if (Annotative::setObjAnnotative(pNewBTR) != Acad::eOk)
            {
                AfxMessageBox(_(L"设置注释性失败！"), MB_OK | MB_ICONERROR);
                return;
            }

			// 创建圆
            double TEXTSIZE;
            if (!AcadVarUtil::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
            {
                AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
                return;
            }
			AcDbCircle* pCircle = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, TEXTSIZE);
			pNewBTR->appendAcDbEntity(pCircle);
            pCircle->setColorIndex(3);
			pCircle->close();

			// 创建属性定义
			AcDbAttributeDefinition* pAttDef = new AcDbAttributeDefinition();
			pAttDef->setTag(Common::BalloonNumberBlock::getAttTag());
			pAttDef->setPrompt(Common::BalloonNumberBlock::getAttPrompt());
			pAttDef->setHeight(TEXTSIZE);
			pAttDef->setHorizontalMode(AcDb::kTextCenter);
			pAttDef->setVerticalMode(AcDb::kTextVertMid);
			pAttDef->setAlignmentPoint(AcGePoint3d::kOrigin);
            pAttDef->setColorIndex(3);
            pAttDef->setLockPositionInBlock(true); //锁定属性位置

			pNewBTR->appendAcDbEntity(pAttDef);
			pAttDef->close();
			pBlockTable->add(pNewBTR);
			pNewBTR->close();
		}
		pBlockTable->close();
	}

    void insertBalloonNumber(AcGePoint3d insPt, unsigned int num)
    {
        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

        AcDbObjectId blockDefineId;
        AcDbBlockTable* pBlockTable = nullptr;
        if (pDb->getBlockTable(pBlockTable, AcDb::kForRead) != Acad::eOk)
        {
            AfxMessageBox(_(L"获取块表失败！"), MB_OK | MB_ICONERROR);
            throw WException(_(L"获取块表失败！"));
            return;
        }

        if (pBlockTable->getAt(Common::BalloonNumberBlock::getBlockName(), blockDefineId) != Acad::eOk)
        {
            pBlockTable->close();
            AfxMessageBox(_(L"获取块定义失败！"), MB_OK | MB_ICONERROR);
            throw WException(_(L"获取块定义失败！"));
            return;
        }
        pBlockTable->close();

        AcDbBlockReference* pBlkRef = new AcDbBlockReference(insPt, blockDefineId);
        //pBlkRef->setScaleFactors(AcGeScale3d(1.0)); // 缩放比例

        Annotative::applyCurrentAnnotativeScale(pBlkRef); // 应用当前注释性比例
        BalloonNumber::syncAttributesFromDefinition(pBlkRef, num);

        // 块参照提交到模型空间
        AcDbBlockTable* pBT = nullptr;
        pDb->getBlockTable(pBT, AcDb::kForWrite);
        AcDbBlockTableRecord* pMs = nullptr;
        if (pBT->getAt(ACDB_MODEL_SPACE, pMs, AcDb::kForWrite) == Acad::eOk)
        {
            AcDbObjectId entId;
            pMs->appendAcDbEntity(entId, pBlkRef);
            pMs->close();
        }
        pBT->close();
        pBlkRef->close();
    }

    void insertBalloonNumberBlockWithStartNumber(int num)
    {
        if (num < 0)
        {
            AfxMessageBox(_(L"开始序号不能小于 0"), MB_OK | MB_ICONERROR);
            return;
        }

        AcString asPrompt;
        while (true)
        {
            BalloonNumber::BalloonNumberJig jig(static_cast<unsigned int>(num));
            asPrompt.format(_(L"\n指定序号 %d 的插入点[退出(Esc)]：\n"), num);
            jig.setDispPrompt(asPrompt);

            if (jig.drag() == AcEdJig::kNormal)
            {
                BalloonNumber::insertBalloonNumber(jig.getPoint(), static_cast<unsigned int>(num));
                ++num;
            }
            else
            {
                break;
            }
        }
    }

    bool updateBalloonNumberBlock(const AcDbObjectId& blockRefId, unsigned int newNum)
    {
        bool bChanged = false;

        // 以写模式打开块参照
        AcDbBlockReference* pBlkRef = Common::getObject<AcDbBlockReference>(blockRefId, AcDb::kForWrite);
        if (pBlkRef == nullptr)
        {
            return false;
        }
        // 遍历块参照携带的属性
        AcDbObjectIterator* pAttIt = pBlkRef->attributeIterator();
        for (pAttIt->start(); !pAttIt->done(); pAttIt->step())
        {
            AcDbObjectId attId = pAttIt->objectId();
            AcDbAttribute* pAtt = Common::getObject<AcDbAttribute>(attId, AcDb::kForWrite);
            if (pAtt == nullptr)
            {
                continue;
            }
            // 检查标签是否匹配
            if (AcString(pAtt->tag()) == Common::BalloonNumberBlock::getAttTag())
            {
                // 修改文本内容
                pAtt->setTextString(std::to_wstring(newNum).c_str()); // 修改序号属性
                pAtt->adjustAlignment(pBlkRef->database()); // 重新计算对齐位置
                actrTransactionManager->queueForGraphicsFlush(); // 强制刷新图形缓冲区
                bChanged = true;
                break;
            }
        }
        delete pAttIt;

        // 如果修改了位置或比例，建议调用记录更新
        //pBlkRef->recordGraphicsModified();

        return bChanged;
    }

    void syncAttributesFromDefinition(AcDbBlockReference* pBlkRef, unsigned int num)
    {
        if (!pBlkRef)
        {
            return;
        }

        AcDbObjectId blockDefineId = pBlkRef->blockTableRecord();
        AcDbBlockTableRecord* pBlockDef = Common::getObject<AcDbBlockTableRecord>(blockDefineId);
        if (pBlockDef == nullptr)
        {
            return;
        }

        AcDbBlockTableRecordIterator* pIt = nullptr;
        pBlockDef->newIterator(pIt);
        for (pIt->start(); !pIt->done(); pIt->step())
        {
            AcDbEntity* pEnt = nullptr;
            if (pIt->getEntity(pEnt, AcDb::kForRead) == Acad::eOk)
            {
                AcDbAttributeDefinition* pAttDef = AcDbAttributeDefinition::cast(pEnt);
                if (pAttDef && !pAttDef->isConstant() && AcString(pAttDef->tag()) == Common::BalloonNumberBlock::getAttTag())
                {
                    AcDbAttribute* pAtt = new AcDbAttribute();
                    pAtt->setPropertiesFrom(pAttDef);
                    // 重要：此步会自动处理注释性比例带来的位置偏移
                    pAtt->setAttributeFromBlock(pAttDef, pBlkRef->blockTransform());
                    pAtt->setTextString(std::to_wstring(num).c_str());

                    pBlkRef->appendAttribute(pAtt);
                    pAtt->close();
                }
                pEnt->close();
            }
        }
        delete pIt;
    }

    bool getBalloonAttributeValue(const AcDbObjectId& blockRefId, AcString& outValue)
    {
        AcDbBlockReference* pBlkRef = Common::getObject<AcDbBlockReference>(blockRefId, AcDb::kForRead);
        if (pBlkRef == nullptr)
        {
            return false;
        }

        bool bNameMatch = false;
        AcDbObjectId btrId = pBlkRef->blockTableRecord();
        AcDbBlockTableRecord* pBTR = Common::getObject<AcDbBlockTableRecord>(btrId, AcDb::kForRead);

        if (pBTR != nullptr)
        {
            wchar_t* pBlockName = nullptr;
            pBTR->getName(pBlockName);
            if (AcString(pBlockName) == Common::BalloonNumberBlock::getBlockName())
            {
                bNameMatch = true;
            }
            acutDelString(pBlockName);
            pBTR->close();
        }

        if (!bNameMatch)
        {
            pBlkRef->close();
            return false;
        }

        bool bFound = false;
        AcDbObjectIterator* pAttIt = pBlkRef->attributeIterator();
        for (pAttIt->start(); !pAttIt->done(); pAttIt->step())
        {
            AcDbAttribute* pAtt = Common::getObject<AcDbAttribute>(pAttIt->objectId(), AcDb::kForRead);
            if (pAtt != nullptr)
            {
                if (AcString(pAtt->tag()) == Common::BalloonNumberBlock::getAttTag())
                {
                    outValue = pAtt->textString();
                    bFound = true;
                }
                pAtt->close();
            }

            if (bFound)
            {
                break;
            }
        }

        delete pAttIt;
        pBlkRef->close();
        return bFound;
    }

    void balloonNumberOffset(const AcDbObjectId& id, int offset)
    {
        AcString currVal;
        if (!BalloonNumber::getBalloonAttributeValue(id, currVal))
        {
            return;
        }

        acutPrintf(_(L"\n读取到气泡号编号：%s"), currVal.constPtr());
        try
        {
            std::wstring wstr(currVal.constPtr());
            size_t pos = 0;
            int val = std::stoi(wstr, &pos);
            if (pos != wstr.length())
            {
                throw std::exception();
            }

            int newVal = val + offset;
            if (newVal < 0)
            {
                newVal = 0;
                acutPrintf(_(L"\n偏置后气泡号编号小于 0，已重置为 0"));
            }
            acutPrintf(_(L"\n偏置后气泡号编号：%d"), newVal);
            BalloonNumber::updateBalloonNumberBlock(id, newVal);
        }
        catch (...)
        {
            acutPrintf(_(L"\n偏置气泡号失败，气泡号值可能不是数字"));
        }
    }

    bool meetCriteria(const AcString& attrValue, const AcString& criteria)
    {
        if (criteria.length() < 3)
        {
            return false;
        }

        AcString strOp = criteria.left(2);
        AcString strTarget = criteria.mid(2);

        // 等于和不等于判断允许非数字
        if (strOp == BalloonNumber::OperatorType::equal)
        {
            if (attrValue == strTarget)
            {
                return true;
            }
            return false;
        }
        if (strOp == BalloonNumber::OperatorType::notEqual1 || strOp == BalloonNumber::OperatorType::notEqual2)
        {
            if (attrValue != strTarget)
            {
                return true;
            }
            return false;
        }

        std::wstring wstrAttrValue(attrValue.constPtr());
        std::wstring wstrTarget(strTarget.constPtr());

        // 大小比较判断必须为数字
        size_t pos = 0;
        try
        {
            int iAttrValue = std::stoi(wstrAttrValue, &pos);
            int iTarget = std::stoi(wstrTarget, &pos);
            if (strOp == BalloonNumber::OperatorType::less1 || strOp == BalloonNumber::OperatorType::less2)
            {
                return iAttrValue < iTarget;
            }
            if (strOp == BalloonNumber::OperatorType::greater1 || strOp == BalloonNumber::OperatorType::greater2)
            {
                return iAttrValue > iTarget;
            }
            if (strOp == BalloonNumber::OperatorType::lessEqual1 || strOp == BalloonNumber::OperatorType::lessEqual2)
            {
                return iAttrValue <= iTarget;
            }
            if (strOp == BalloonNumber::OperatorType::greaterEqual1 || strOp == BalloonNumber::OperatorType::greaterEqual2)
            {
                return iAttrValue >= iTarget;
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }
}

namespace
{
    void cmdInsertBalloonNumberBlockWithStartNumber()
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

    void cmdUpdateBalloonNumberBlock()
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

    void cmdBalloonNumberOffset()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"偏置气泡号");
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

    void cmdBalloonNumberFilter()
    {
        CAcModuleResourceOverride resOverride;
        CString title = _(L"筛选气泡号");
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

    void cmdCheckBalloonNumberMaxMin()
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

    void cmdCheckDuplicateBalloonNumbers()
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

    void cmdCheckBalloonNumberBreakpoints()
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

    Commands::AutoRegister ar =
    {
        { L"yxInsertBalloonNumberBlockWithStartNumber", []() { return  _(L"插入带起始编号的气泡号"); }, Commands::CommandFlags::Base, cmdInsertBalloonNumberBlockWithStartNumber },
        { L"yxUpdateBalloonNumberBlock", []() { return  _(L"更新气泡号"); }, Commands::CommandFlags::PickRedraw, cmdUpdateBalloonNumberBlock },
        { L"yxBalloonNumberOffset", []() { return  _(L"偏置气泡号"); }, Commands::CommandFlags::PickRedraw, cmdBalloonNumberOffset },
        { L"yxBalloonNumberFilter", []() { return  _(L"筛选气泡号"); }, Commands::CommandFlags::PickRedraw, cmdBalloonNumberFilter },
        { L"yxCheckBalloonNumberMaxMin", []() { return  _(L"查找气泡号最大和最小序号"); }, Commands::CommandFlags::PickRedraw, cmdCheckBalloonNumberMaxMin },
        { L"yxCheckDuplicateBalloonNumbers", []() { return  _(L"检查重复气泡号"); }, Commands::CommandFlags::PickRedraw, cmdCheckDuplicateBalloonNumbers },
        { L"yxCheckBalloonNumberBreakpoints", []() { return  _(L"检查气泡号断点"); }, Commands::CommandFlags::PickRedraw, cmdCheckBalloonNumberBreakpoints },
    };
}