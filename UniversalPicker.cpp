module;
#include "stdafx.h"
#include "dbtrans.h"

module UniversalPicker;
import Commands;
import Common;

resbuf* UniversalPicker::buildFilter(UniversalPicker::AcRxClassVectorPtr arcv)
{
    if (arcv == nullptr || arcv->empty())
    {
        return nullptr;
    }

    resbuf* head = nullptr;
    resbuf* tail = nullptr;

    // 使用 <OR ... OR> 结构，这样即便向量里有多个不相关的类（如标注和形位公差），也能共存
    head = acutBuildList(-4, L"<OR", 0);
    tail = head;

    for (AcRxClass* cls : *arcv)
    {
        if (!cls)
        {
            continue;
        }

        const wchar_t* dxfName = cls->dxfName();
        if (!dxfName || !*dxfName)
        {
            continue;
        }

        // 构造当前类的过滤项
        resbuf* item = acutBuildList(RTDXF0, dxfName, 0);

        if (item)
        {
            tail->rbnext = item;
            // 移动到当前链表末端
            while (tail->rbnext)
            {
                tail = tail->rbnext;
            }
        }
    }

    // 闭合 OR 结构
    tail->rbnext = acutBuildList(-4, L"OR>", 0);

    return head;
}

void UniversalPicker::batchSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, SortMode defaultSortMode, bool isLocked, double sortTol)
{
    ads_name ss;
    resbuf* filterRb = buildFilter(arcv);

    if (acedSSGet(nullptr, nullptr, nullptr, filterRb, ss) != RTNORM)
    {
        UniversalPicker::freeFilter(filterRb);
        return;
    }

    Adesk::Int32 len = 0;
    acedSSLength(ss, &len);
    if (len <= 0)
    {
        acedSSFree(ss);
        UniversalPicker::freeFilter(filterRb);
        return;
    }

    // 确定排序模式（提前获取模式以决定后续路径）
    SortMode finalMode = defaultSortMode;
    if (!isLocked)
    {
        acedInitGet(0, L"RD RU LD LU DR DL UR UL None");
        wchar_t kword[20];
        AcString asKword;
        const wchar_t* prefix = L"\n排序模式 [右下(RD)/右上(RU)/左下(LD)/左上(LU)/下右(DR)/下左(DL)/上右(UR)/上左(UL)/无(None)] <";
        const wchar_t* suffix = L">: ";
        asKword.format(L"%s%s%s", prefix, UniversalPicker::SortModeToString(finalMode), suffix);
        int stat = acedGetKword(asKword.constPtr(), kword);

        if (stat == Acad::eNormal)
        {
            if (_wcsicmp(kword, L"RD") == 0)
            {
                finalMode = SortMode::RD;
            }
            else if (_wcsicmp(kword, L"RU") == 0)
            {
                finalMode = SortMode::RU;
            }
            else if (_wcsicmp(kword, L"LD") == 0)
            {
                finalMode = SortMode::LD;
            }
            else if (_wcsicmp(kword, L"LU") == 0)
            {
                finalMode = SortMode::LU;
            }
            else if (_wcsicmp(kword, L"DR") == 0)
            {
                finalMode = SortMode::DR;
            }
            else if (_wcsicmp(kword, L"DL") == 0)
            {
                finalMode = SortMode::DL;
            }
            else if (_wcsicmp(kword, L"UR") == 0)
            {
                finalMode = SortMode::UR;
            }
            else if (_wcsicmp(kword, L"UL") == 0)
            {
                finalMode = SortMode::UL;
            }
            else if (_wcsicmp(kword, L"None") == 0)
            {
                finalMode = SortMode::None;
            }
        }
        else if (stat == Acad::eNone)
        {
            finalMode = defaultSortMode;
        }
        else
        {
            acedSSFree(ss);
            UniversalPicker::freeFilter(filterRb);
            return;
        }
    }

    // 根据模式进入不同处理逻辑
    std::vector<EntityInfo> entities;
    std::vector<AcDbObjectId> processedIds;
    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
    AcDbTransactionManager* pTransMgr = pDb->transactionManager();
    AcTransaction* pTrans = pTransMgr->startTransaction();

    try
    {
        for (Adesk::Int32 i = 0; i < len; ++i)
        {
            ads_name ent;
            acedSSName(ss, i, ent);
            AcDbObjectId id;
            if (acdbGetObjectId(id, ent) != Acad::eOk)
            {
                continue;
            }

            if (finalMode == SortMode::None)
            {
                // 不排序，直接执行
                processor(id);
                processedIds.push_back(id);
            }
            else
            {
                // 排序路径：采集坐标信息
                AcGePoint3d pt = AcGePoint3d::kOrigin;
                AcDbEntityPointer pEnt(id, AcDb::kForRead);
                if (pEnt.openStatus() == Acad::eOk)
                {
                    AcDbBlockReference* pBlk = AcDbBlockReference::cast(pEnt);
                    if (pBlk)
                    {
                        pt = pBlk->position();
                    }
                    else
                    {
                        AcDbExtents ext;
                        if (pEnt->getGeomExtents(ext) == Acad::eOk)
                        {
                            pt = ext.minPoint() + (ext.maxPoint() - ext.minPoint()) * 0.5;
                        }
                    }
                    entities.push_back({ id, pt });
                }
            }
        }

        // 如果需要排序，则在此执行排序和处理
        if (finalMode != SortMode::None)
        {
            std::sort(entities.begin(), entities.end(), [finalMode, sortTol](const EntityInfo& a, const EntityInfo& b)
                {
                    return UniversalPicker::compareEntities(a, b, finalMode, sortTol);
                });

            for (const auto& item : entities)
            {
                processor(item.id);
                processedIds.push_back(item.id);
            }
        }

        if (pTrans)
        {
            pTransMgr->endTransaction();
        }

        // 统一刷新图形
        for (const auto& id : processedIds)
        {
            AcDbEntity* pEnt = Common::getObject<AcDbEntity>(id, AcDb::kForRead);
            if (pEnt != nullptr)
            {
                AcDbDimension* pDim = AcDbDimension::cast(pEnt);
                if (pDim)
                {
                    pDim->recomputeDimBlock();
                }
                pEnt->recordGraphicsModified();
                pEnt->draw();
            }
        }
        actrTransactionManager->flushGraphics();
        acedUpdateDisplay();
    }
    catch (...)
    {
        if (pTrans)
        {
            pTransMgr->abortTransaction();
        }
    }

    acedSSFree(ss);
    UniversalPicker::freeFilter(filterRb);
}

AcDbObjectId UniversalPicker::getSelectedSingleEntityId(UniversalPicker::AcRxClassVectorPtr arcv)
{
    AcDbObjectId selectedId = AcDbObjectId::kNull;
    ads_name ss;
    resbuf* filterRb = buildFilter(arcv);

    while (true)
    {
        int rc = acedSSGet(L":S:E", nullptr, nullptr, filterRb, ss);
        if (rc == RTCAN) // 用户按了 ESC
        {
            break;
        }
        if (rc != RTNORM) // 没选中实体或其它情况
        {
            continue;
        }

        Adesk::Int32 len = 0;
        acedSSLength(ss, &len);

        if (len > 0)
        {
            ads_name ent;
            if (acedSSName(ss, 0, ent) == RTNORM) // 获取第一个实体的 ads_name
            {
                acdbGetObjectId(selectedId, ent);
            }
        }
        
        acedSSFree(ss);
        if (!selectedId.isNull())
        {
            break;
        }
    }
    UniversalPicker::freeFilter(filterRb);
    return selectedId;
}

void UniversalPicker::immediateSelect(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor)
{
    AcDbTransactionManager* pTransMgr = acdbHostApplicationServices()->workingDatabase()->transactionManager();
    resbuf* filterRb = buildFilter(arcv);
    ads_name ss;

    while (true)
    {
        int rc = acedSSGet(L":S:E", nullptr, nullptr, filterRb, ss);
        if (rc == RTCAN) // 用户按了 ESC
        {
            break;
        }
        if (rc != RTNORM) // 没选中实体或其它情况
        {
            continue;
        }

        // 从选择集中提取第一个实体的 ads_name
        ads_name ent;
        if (acedSSName(ss, 0, ent) == RTNORM)
        {
            AcDbObjectId id;
            if (acdbGetObjectId(id, ent) == Acad::eOk)
            {
                // 执行业务逻辑（包裹在事务中）
                AcTransaction* pTrans = pTransMgr->startTransaction();
                if (pTrans != nullptr)
                {
                    try
                    {
                        processor(id); // 调用业务回调
                        pTransMgr->endTransaction();

                        // --- 实时刷新逻辑开始 ---
                        // 事务提交后，只需以只读方式打开进行绘制触发
                        AcDbEntity* pEnt = Common::getObject<AcDbEntity>(id, AcDb::kForRead);
                        if (pEnt != nullptr)
                        {
                            // 如果是标注类，必须重构其图形块
                            AcDbDimension* pDim = AcDbDimension::cast(pEnt);
                            if (pDim != nullptr)
                            {
                                pDim->recomputeDimBlock();
                            }

                            pEnt->draw(); // 将更新后的几何数据推送到缓冲区
                        }
                        // 强制图形系统立即渲染缓冲区中的改变，而非等待命令结束
                        //actrTransactionManager->flushGraphics();
                        actrTransactionManager->queueForGraphicsFlush();
                        acedUpdateDisplay();
                        // --- 实时刷新逻辑结束 ---
                    }
                    catch (...)
                    {
                        pTransMgr->abortTransaction();
                    }
                }
            }
        }
        acedSSFree(ss);
    }
    UniversalPicker::freeFilter(filterRb);
}


void UniversalPicker::run(UniversalPicker::AcRxClassVectorPtr arcv, UniversalPicker::EntityProcessor processor, const wchar_t* prompt, UniversalPicker::SelectMode defaultSelectMode, bool lockSelectMode, SortMode defaultSortMode, bool lockSortMode, double sortTol)
{
    if (prompt != nullptr)
    {
        acutPrintf(L"\n%s\n", prompt);
    }

    // 模式选择提示
    const wchar_t* modePrompt = (defaultSelectMode == UniversalPicker::SelectMode::Batch) ?
        L"\n选择模式 [批量(B)/立即(I)]<B>：" :
        L"\n选择模式 [批量(B)/立即(I)]<I>：";

    UniversalPicker::SelectMode inputMode = defaultSelectMode;
    wchar_t keyword[2] = { 0 };
    bool validInput = false;

    if (!lockSelectMode)
    {
        do
        {
            acedInitGet(0, L"B I"); // 关键词输入规则，允许空回车和输入 B 或 I
            int stat = acedGetKword(modePrompt, keyword);
            if (stat == RTNORM)
            {
                if (_wcsicmp(keyword, L"B") == 0)
                {
                    inputMode = UniversalPicker::SelectMode::Batch;
                    validInput = true;
                }
                else if (_wcsicmp(keyword, L"I") == 0)
                {
                    inputMode = UniversalPicker::SelectMode::Immediate;
                    validInput = true;
                }
            }
            else if (stat == RTNONE)
            {
                inputMode = defaultSelectMode;
                validInput = true;
            }
            else if (stat == RTCAN)
            {
                return;
            }
            // 其它情况不处理，继续循环
        } while (!validInput);
    }

    if (inputMode == UniversalPicker::SelectMode::Batch)
    {
        UniversalPicker::batchSelect(arcv, processor, defaultSortMode, lockSortMode, sortTol);
    }
    else
    {
        UniversalPicker::immediateSelect(arcv, processor);
    }
}

void UniversalPicker::freeFilter(resbuf* filterRb)
{
    if (filterRb != nullptr)
    {
        acutRelRb(filterRb);
        filterRb = nullptr;
    }
}

bool UniversalPicker::compareEntities(const UniversalPicker::EntityInfo& a, const UniversalPicker::EntityInfo& b, UniversalPicker::SortMode mode, double sortTol)
{
    switch (mode)
    {
        case UniversalPicker::SortMode::RD:
        {
            if (fabs(a.pos.y - b.pos.y) > sortTol)
            {
                return a.pos.y > b.pos.y;
            }
            return a.pos.x < b.pos.x;
        }

        case UniversalPicker::SortMode::RU:
        {
            if (fabs(a.pos.y - b.pos.y) > sortTol)
            {
                return a.pos.y < b.pos.y;
            }
            return a.pos.x < b.pos.x;
        }

        case UniversalPicker::SortMode::LD:
        {
            if (fabs(a.pos.y - b.pos.y) > sortTol)
            {
                return a.pos.y > b.pos.y;
            }
            return a.pos.x > b.pos.x;
        }

        case UniversalPicker::SortMode::LU:
        {
            if (fabs(a.pos.y - b.pos.y) > sortTol)
            {
                return a.pos.y < b.pos.y;
            }
            return a.pos.x > b.pos.x;
        }

        case UniversalPicker::SortMode::DR:
        {
            if (fabs(a.pos.x - b.pos.x) > sortTol)
            {
                return a.pos.x < b.pos.x;
            }
            return a.pos.y > b.pos.y;
        }

        case UniversalPicker::SortMode::DL:
        {
            if (fabs(a.pos.x - b.pos.x) > sortTol)
            {
                return a.pos.x > b.pos.x;
            }
            return a.pos.y > b.pos.y;
        }

        case UniversalPicker::SortMode::UR:
        {
            if (fabs(a.pos.x - b.pos.x) > sortTol)
            {
                return a.pos.x < b.pos.x;
            }
            return a.pos.y < b.pos.y;
        }

        case UniversalPicker::SortMode::UL:
        {
            if (fabs(a.pos.x - b.pos.x) > sortTol)
            {
                return a.pos.x > b.pos.x;
            }
            return a.pos.y < b.pos.y;
        }

        default:
        {
            return false;
        }
    }
}

const wchar_t* UniversalPicker::SortModeToString(UniversalPicker::SortMode mode)
{
    switch (mode)
    {
        case SortMode::RD:
        {
            return L"RD";
        }
        case SortMode::RU:
        {
            return L"RU";
        }
        case SortMode::LD:
        {
            return L"LD";
        }
        case SortMode::LU:
        {
            return L"LU";
        }
        case SortMode::DR:
        {
            return L"DR";
        }
        case SortMode::DL:
        {
            return L"DL";
        }
        case SortMode::UR:
        {
            return L"UR";
        }
        case SortMode::UL:
        {
            return L"UL";
        }
        case SortMode::None:
        {
            return L"None";
        }
        default:
        {
            return L"None";
        }
    }
}

void UniversalPicker::setSelection(const AcDbObjectIdArray& idArray)
{
    if (idArray.isEmpty())
    {
        acedSSSetFirst(NULL, NULL);
        return;
    }

    ads_name ss;
    if (acedSSAdd(NULL, NULL, ss) != RTNORM)
    {
        return;
    }

    for (int i = 0; i < idArray.length(); i++)
    {
        ads_name entName;
        if (acdbGetAdsName(entName, idArray[i]) == Acad::eOk)
        {
            acedSSAdd(entName, ss, ss);
        }
    }

    int result = acedSSSetFirst(ss, ss);

    acedSSFree(ss);

    Commands::CommandList pszCmdList = { L"SELECT" };
    Commands::executeCommand(pszCmdList);
}