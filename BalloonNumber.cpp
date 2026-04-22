module;
#include "StdAfx.h"
#include "resource.h"

module BalloonNumber;
import std;
import Common;
import Annotative;

namespace BalloonNumber
{
    BalloonNumberJig::BalloonNumberJig(unsigned int num) : mNum(num)
    {
        this->mCurPt = AcGePoint3d::kOrigin;

        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
        AcDbBlockTable* pBlockTable = nullptr;
        if (pDb->getBlockTable(pBlockTable, AcDb::kForRead) != Acad::eOk)
        {
            AfxMessageBox(L"获取块表失败！", MB_OK | MB_ICONERROR);
            throw std::runtime_error("获取块表失败！");
        }
        if (pBlockTable->getAt(Common::BalloonNumberBlock::blockName, this->mBlockDefineId) != Acad::eOk)
        {
            pBlockTable->close();
            AfxMessageBox(L"获取块定义失败！", MB_OK | MB_ICONERROR);
            throw std::runtime_error("获取块定义失败！");
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
                if (pAttDef && !pAttDef->isConstant() && AcString(pAttDef->tag()) == Common::BalloonNumberBlock::AttTag)
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
		if (!pBlockTable->has(Common::BalloonNumberBlock::blockName))
		{
			AcDbBlockTableRecord* pNewBTR = new AcDbBlockTableRecord();
			pNewBTR->setName(Common::BalloonNumberBlock::blockName);
			pNewBTR->setOrigin(AcGePoint3d::kOrigin);
            if (Annotative::setObjAnnotative(pNewBTR) != Acad::eOk)
            {
                AfxMessageBox(L"设置注释性失败！", MB_OK | MB_ICONERROR);
                return;
            }

			// 创建圆
			AcDbCircle* pCircle = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, Common::BalloonNumberBlock::defaultCircleRadius);
			pNewBTR->appendAcDbEntity(pCircle);
            pCircle->setColorIndex(3);
			pCircle->close();

			// 创建属性定义
			AcDbAttributeDefinition* pAttDef = new AcDbAttributeDefinition();
			pAttDef->setTag(Common::BalloonNumberBlock::AttTag);
			pAttDef->setPrompt(Common::BalloonNumberBlock::AttPrompt);
			pAttDef->setHeight(Common::BalloonNumberBlock::defaultTextHeight);
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
            AfxMessageBox(L"获取块表失败！", MB_OK | MB_ICONERROR);
            throw std::runtime_error("获取块表失败！");
            return;
        }

        if (pBlockTable->getAt(Common::BalloonNumberBlock::blockName, blockDefineId) != Acad::eOk)
        {
            pBlockTable->close();
            AfxMessageBox(L"获取块定义失败！", MB_OK | MB_ICONERROR);
            throw std::runtime_error("获取块定义失败！");
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
            AfxMessageBox(L"开始序号不能小于 0", MB_OK | MB_ICONERROR);
            return;
        }

        AcString asPrompt;
        while (true)
        {
            BalloonNumber::BalloonNumberJig jig(static_cast<unsigned int>(num));
            asPrompt.format(L"\n指定序号 %d 的插入点[退出(Esc)]：\n", num);
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
            if (AcString(pAtt->tag()) == Common::BalloonNumberBlock::AttTag)
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
                if (pAttDef && !pAttDef->isConstant() && AcString(pAttDef->tag()) == Common::BalloonNumberBlock::AttTag)
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
            if (AcString(pBlockName) == Common::BalloonNumberBlock::blockName)
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
                if (AcString(pAtt->tag()) == Common::BalloonNumberBlock::AttTag)
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

        acutPrintf(Common::loadString(IDS_MSG_OffsetResultInfo_FMT), currVal.constPtr());
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
                acutPrintf(L"\n%s", Common::loadString(IDS_WARN_OffsetLessZero));
            }
            acutPrintf(Common::loadString(IDS_MSG_OffsetTargetInfo_FMT), newVal);
            BalloonNumber::updateBalloonNumberBlock(id, newVal);
        }
        catch (...)
        {
            acutPrintf(L"\n%s", Common::loadString(IDS_Err_BalloonNumberOffsetFail));
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