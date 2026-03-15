#include "stdafx.h"
#include "UniversalPicker.hpp"
#include "dbtrans.h"

#include <unordered_set>

resbuf* UniversalPicker::buildFilter(const ACHAR* filterStr)
{
	if (!filterStr || std::wcslen(filterStr) == 0)
	{
		return nullptr;
	}
	return acutBuildList(RTDXF0, filterStr, RTNONE);
}
void UniversalPicker::batchSelect(const Options& options, EntityProcessor processor)
{
    ads_name ss;
    resbuf* filterRb = buildFilter(options.filter);

    if (acedSSGet(nullptr, nullptr, nullptr, filterRb, ss) != RTNORM)
    {
        UniversalPicker::freeFilter(filterRb);
        return;
    }

    // 使用事务管理器开始
    AcDbTransactionManager* pTransMgr = acdbHostApplicationServices()->workingDatabase()->transactionManager();
    AcTransaction* pTrans = pTransMgr->startTransaction();

    try 
    {
        Adesk::Int32 len = 0;
        acedSSLength(ss, &len);
        std::unordered_set<AcDbObjectId> seen;

        for (Adesk::Int32 i = 0; i < len; ++i)
        {
            ads_name ent;
            acedSSName(ss, i, ent);
            AcDbObjectId id;
            if (acdbGetObjectId(id, ent) != Acad::eOk)
            {
                continue;
            }

            if (!options.allowDuplicates)
            {
                if (!seen.insert(id).second)
                {
                    continue;
                }
            }
            processor(id);
        }
        if (pTrans) // 提交修改
        {
            pTransMgr->endTransaction();
        }
    }
    catch (...)
    {
        if (pTrans) pTransMgr->abortTransaction(); // 出错回滚
    }

    acedSSFree(ss);
    UniversalPicker::freeFilter(filterRb);
}

void UniversalPicker::immediateSelect(const Options& options, EntityProcessor processor)
{
    std::unordered_set<AcDbObjectId> seen;
    AcDbTransactionManager* pTransMgr = acdbHostApplicationServices()->workingDatabase()->transactionManager();
    resbuf* filterRb = buildFilter(options.filter);

    while (true)
    {
        ads_name ss;

        int rc = acedSSGet(L":S", nullptr, nullptr, filterRb, ss);
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

                // 防重复逻辑
                if (!options.allowDuplicates)
                {
                    if (!seen.insert(id).second)
                    {
                        acedSSFree(ss); // 别忘了释放当前选择集
                        continue;
                    }
                }

                // 执行业务逻辑（包裹在事务中）
                AcTransaction* pTrans = pTransMgr->startTransaction();
                if (pTrans != nullptr)
                {
                    try
                    {
                        processor(id); // 调用业务回调
                        pTransMgr->endTransaction();

                        // --- 实时刷新逻辑开始 ---
                        AcDbEntity* pEnt = nullptr;
                        // 事务提交后，只需以只读方式打开进行绘制触发
                        if (acdbOpenObject(pEnt, id, AcDb::kForRead) == Acad::eOk)
                        {
                            // 如果是标注类，必须重构其图形块
                            AcDbDimension* pDim = AcDbDimension::cast(pEnt);
                            if (pDim != nullptr)
                            {
                                pDim->recomputeDimBlock();
                            }

                            pEnt->draw(); // 将更新后的几何数据推送到缓冲区
                            pEnt->close();
                        }
                        // 强制图形系统立即渲染缓冲区中的改变，而非等待命令结束
                        actrTransactionManager->flushGraphics();
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


void UniversalPicker::run(const Options& options, EntityProcessor processor)
{
    acutPrintf(L"\n%s", options.prompt);

    if (options.mode == SelectMode::Batch)
    {
        batchSelect(options, processor);
    }
    else
    {
        immediateSelect(options, processor);
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
