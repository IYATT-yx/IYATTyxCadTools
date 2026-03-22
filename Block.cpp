module;
#include "StdAfx.h"
#include <stdexcept>

module Block;

import Common;

namespace Block
{
    //[AcDbDatabase(当前工作数据库)]
    //|
    //|-- 1. getBlockTable(kForWrite) -- > [AcDbBlockTable * pBlockTable]
    //|
    //|-- 2. 检查 pBlockTable 是否存在块名 Common::SerialNumberCircleBlock::blockName
    //|      |
    //|      |--[不存在] -- > 3. 创建[AcDbBlockTableRecord * pNewBTR](块定义)
    //|            |
    //|            |-- 4. pNewBTR->setName(...) 设置名称
    //|            |-- 5. pNewBTR->setOrigin(...) 设置基点
    //|            |
    //|            |-- 6. 创建[AcDbCircle * pCircle]
    //|            |      |--pCircle->setRadius(...)
    //|            |      |--pNewBTR->appendAcDbEntity(pCircle) -- > [插入到块定义]
    //|            |      |--pCircle->close() -- > (关闭圆对象)
    //|            |
    //|            |-- 7. 创建[AcDbAttributeDefinition * pAttDef]
    //|            |      |--pAttDef->setTag / setHeight / setAlignmentPoint(...)[修改属性]
    //|            |      |--pNewBTR->appendAcDbEntity(pAttDef) -- > [插入到块定义]
    //|            |      |--pAttDef->close() -- > (关闭属性定义)
    //|            |
    //|            |-- 8. pBlockTable->add(pNewBTR) -- > [将块定义插入块表]
    //|            |-- 9. pNewBTR->close() -- > (关闭块定义对象)
    //|
    //|-- 10. pBlockTable->close() -- > (关闭块表对象)
	void createSerialNumberBlock()
	{
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		AcDbBlockTable* pBlockTable;
		pDb->getBlockTable(pBlockTable, AcDb::kForWrite);

		// 检查块是否存在
		if (!pBlockTable->has(Common::SerialNumberCircleBlock::blockName))
		{
			AcDbBlockTableRecord* pNewBTR = new AcDbBlockTableRecord();
			pNewBTR->setName(Common::SerialNumberCircleBlock::blockName);
			pNewBTR->setOrigin(AcGePoint3d::kOrigin);

			// 创建圆
			AcDbCircle* pCircle = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, Common::SerialNumberCircleBlock::defaultCircleRadius);
			pNewBTR->appendAcDbEntity(pCircle);
            pCircle->setColorIndex(3);
			pCircle->close();

			// 创建属性定义
			AcDbAttributeDefinition* pAttDef = new AcDbAttributeDefinition();
			pAttDef->setTag(Common::SerialNumberCircleBlock::AttTag);
			pAttDef->setPrompt(Common::SerialNumberCircleBlock::AttPrompt);
			pAttDef->setHeight(Common::SerialNumberCircleBlock::defaultTextHeight);
			pAttDef->setHorizontalMode(AcDb::kTextCenter);
			pAttDef->setVerticalMode(AcDb::kTextVertMid);
			pAttDef->setAlignmentPoint(AcGePoint3d::kOrigin);
            pAttDef->setColorIndex(3);

			pNewBTR->appendAcDbEntity(pAttDef);
			pAttDef->close();
			pBlockTable->add(pNewBTR);
			pNewBTR->close();
		}
		pBlockTable->close();
	}

    //[AcDbDatabase]
    //|
    //|-- 1. getBlockTable(kForRead) -- > [AcDbBlockTable * pBlockTable]
    //|      |--getAt(...) -- > 获取[AcDbObjectId blockDefineId]
    //|      |--pBlockTable->close() (及时关闭)
    //|
    //|-- 2. 创建[AcDbBlockReference * pBlkRef](块参照)
    //|      |--传入(insPt, blockDefineId)
    //|
    //|-- 3. acdbOpenObject(kForRead) -- > [AcDbBlockTableRecord * pBlockDef](打开块定义)
    //|      |
    //|      |-- 4. 创建[AcDbBlockTableRecordIterator * pIt](迭代器)
    //|      |      |--[循环块定义内的实体]
    //|      |      |--getEntity(kForRead) -- > [AcDbEntity * pEnt]
    //|      |      |      |
    //|      |      |-- 5. cast 为[AcDbAttributeDefinition * pAttDef]
    //|      |      |      |--[如果是属性定义且非恒定]
    //|      |      |      |-- 6. 创建[AcDbAttribute * pAtt](属性实例)
    //|      |      |      |-- 7. pAtt->setAttributeFromBlock(pAttDef, ...)[克隆定义属性]
    //|      |      |      |-- 8. pAtt->setTextString(numStr)[修改文本属性]
    //|      |      |      |-- 9. pBlkRef->appendAttribute(pAtt) -- > [将属性插入块参照]
    //|      |      |      |-- 10. pAtt->close() (关闭属性实例)
    //|      |      |      |
    //|      |      |--pEnt->close() (关闭迭代到的实体)
    //|      |
    //|      |-- 11. delete pIt(销毁迭代器)
    //|      |-- 12. pBlockDef->close() (关闭块定义)
    //|
    //|-- 13. pDb->getBlockTable(kForWrite) -- > [AcDbBlockTable * pBT]
    //|      |--getAt(ACDB_MODEL_SPACE) -- > [AcDbBlockTableRecord * pMs](打开模型空间)
    //|      |-- 14. pMs->appendAcDbEntity(pBlkRef) -- > [将块参照插入模型空间]
    //|      |-- 15. pMs->close()
    //|      |-- 16. pBT->close()
    //|
    //|-- 17. pBlkRef->close() -- > (关闭块参照对象)
    void insertSerialNumber(AcGePoint3d insPt, unsigned int num, double dScale)
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

        if (pBlockTable->getAt(Common::SerialNumberCircleBlock::blockName, blockDefineId) != Acad::eOk)
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
                    if (pAttDef && !pAttDef->isConstant() && AcString(pAttDef->tag()) == Common::SerialNumberCircleBlock::AttTag)
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

    void insertSerialNumberBlockWithStartNumber(int num, double dScale)
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
            Block::SerialNumberJig jig(static_cast<unsigned int>(num), dScale);
            asPrompt.format(L"\n指定序号 %d 的插入点[退出(Esc)]：\n", num);
            jig.setDispPrompt(asPrompt);

            if (jig.drag() == AcEdJig::kNormal)
            {
                Block::insertSerialNumber(jig.getPoint(), static_cast<unsigned int>(num), dScale);
                ++num;
            }
            else
            {
                break;
            }
        }
    }

    SerialNumberJig::SerialNumberJig(unsigned int num, double dScale) : mNum(num), mdScale(dScale)
    {
        this->mCurPt = AcGePoint3d::kOrigin;

        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
        AcDbBlockTable* pBlockTable = nullptr;
        if (pDb->getBlockTable(pBlockTable, AcDb::kForRead) != Acad::eOk)
        {
            AfxMessageBox(L"获取块表失败！", MB_OK | MB_ICONERROR);
            throw std::runtime_error("获取块表失败！");
        }
        if (pBlockTable->getAt(Common::SerialNumberCircleBlock::blockName, this->mBlockDefineId) != Acad::eOk)
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

    SerialNumberJig::~SerialNumberJig()
    {
        if (this->mpBlockReference)
        {
            this->mpBlockReference->close();
            this->mpBlockReference = nullptr;
        }
    }

    AcEdJig::DragStatus SerialNumberJig::sampler()
    {
        return acquirePoint(this->mCurPt);
    }

    Adesk::Boolean SerialNumberJig::update()
    {
        // 更新块参照位置
        this->mpBlockReference->setPosition(this->mCurPt);

        // 获取块参照的变换矩阵
        AcGeMatrix3d mat = this->mpBlockReference->blockTransform();

        // 强制所有属性跟随快定义的相对位置变换
        AcDbObjectIterator* pAttIt = this->mpBlockReference->attributeIterator();
        if (pAttIt)
        {
            for (; !pAttIt->done(); pAttIt->step())
            {
                AcDbAttribute* pAtt = AcDbAttribute::cast(pAttIt->entity());
                if (pAtt)
                {
                    AcGePoint3d basePt = AcGePoint3d::kOrigin;
                    pAtt->setPosition(basePt.transformBy(mat));

                    if (pAtt->isDefaultAlignment() == Adesk::kFalse)
                    {
                        pAtt->setAlignmentPoint(basePt.transformBy(mat));
                    }
                }
            }
            delete pAttIt;
            pAttIt = nullptr;
        }
        return Adesk::kTrue;
    }

    AcDbEntity* SerialNumberJig::entity() const
    {
        return this->mpBlockReference;
    }

    AcGePoint3d SerialNumberJig::getPoint() const
    {
        return this->mCurPt;
    }

    void SerialNumberJig::setupAttributes()
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
                    if (pAttDef && !pAttDef->isConstant() && AcString(pAttDef->tag()) == Common::SerialNumberCircleBlock::AttTag)
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