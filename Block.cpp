module;
#include "StdAfx.h"
#include <stdexcept>

module Block;
import Common;

namespace Block
{
    BalloonNumberJig::BalloonNumberJig(unsigned int num, double dScale) : mNum(num), mdScale(dScale)
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
        this->mpBlockReference->setScaleFactors(AcGeScale3d(this->mdScale));

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
        AcDbBlockTableRecord* pBlockDefinition = nullptr;
        if (acdbOpenObject(pBlockDefinition, this->mBlockDefineId, AcDb::kForRead) == Acad::eOk)
        {
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
            pBlockDefinition->close();
            pBlockDefinition = nullptr;
        }
    }
}

namespace Block
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

    void insertBalloonNumber(AcGePoint3d insPt, unsigned int num, double dScale)
    {
        std::wstring wsNumber = std::to_wstring(num);
        const ACHAR* numStr = wsNumber.c_str();

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

        // 创建块参照
        AcDbBlockReference* pBlkRef = new AcDbBlockReference(insPt, blockDefineId);
        // 修改比例
        pBlkRef->setScaleFactors(AcGeScale3d(dScale));

        AcDbBlockTableRecord* pBlockDef = nullptr;
        if (acdbOpenObject(pBlockDef, blockDefineId, AcDb::kForRead) == Acad::eOk)
        {
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
                        pAtt->setAttributeFromBlock(pAttDef, pBlkRef->blockTransform());
                        pAtt->setTextString(numStr);

                        pBlkRef->appendAttribute(pAtt);
                        pAtt->close();
                    }
                    pEnt->close();
                }
            }
            delete pIt;
            pBlockDef->close();
        }

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

    void insertBalloonNumberBlockWithStartNumber(int num, double dScale)
    {
        if (num < 0)
        {
            AfxMessageBox(L"开始序号不能小于 0", MB_OK | MB_ICONERROR);
            return;
        }

        if (dScale <= 0)
        {
            AfxMessageBox(L"比例必须大于 0", MB_OK | MB_ICONERROR);
            return;
        }

        AcString asPrompt;

        while (true)
        {
            Block::BalloonNumberJig jig(static_cast<unsigned int>(num), dScale);
            asPrompt.format(L"\n指定序号 %d 的插入点[退出(Esc)]：\n", num);
            jig.setDispPrompt(asPrompt);

            if (jig.drag() == AcEdJig::kNormal)
            {
                Block::insertBalloonNumber(jig.getPoint(), static_cast<unsigned int>(num), dScale);
                ++num;
            }
            else
            {
                break;
            }
        }
    }

    bool updateBalloonNumberBlock(AcDbObjectId blockRefId, unsigned int newNum)
    {
        AcDbBlockReference* pBlkRef = nullptr;
        bool bSuccess = false;

        // 以写模式打开块参照
        if (acdbOpenObject(pBlkRef, blockRefId, AcDb::kForWrite) == Acad::eOk)
        {
            // 遍历块参照携带的属性
            AcDbObjectIterator* pAttIt = pBlkRef->attributeIterator();
            for (pAttIt->start(); !pAttIt->done(); pAttIt->step())
            {
                AcDbObjectId attId = pAttIt->objectId();
                AcDbAttribute* pAtt = nullptr;

                if (acdbOpenObject(pAtt, attId, AcDb::kForWrite) == Acad::eOk)
                {
                    // 检查标签是否匹配
                    if (AcString(pAtt->tag()) == Common::BalloonNumberBlock::AttTag)
                    {
                        // 修改文本内容
                        pAtt->setTextString(std::to_wstring(newNum).c_str());
                        bSuccess = true;
                    }
                    pAtt->close();
                }
            }
            delete pAttIt;

            // 如果修改了位置或比例，建议调用记录更新
            //pBlkRef->recordGraphicsModified();
            pBlkRef->close();
        }
        return bSuccess;
    }
}