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
    // 记录处理过的 ID，用于后续批量刷新图形
    std::vector<AcDbObjectId> processedIds;

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
            processedIds.push_back(id);
        }
        if (pTrans) // 提交修改
        {
            pTransMgr->endTransaction();
        }

        // --- 批量强制刷新逻辑 ---
        for (const auto& id : processedIds)
        {
            AcDbDimension* pDim = nullptr;
            // 以写模式打开，强制触发标注块重绘
            if (acdbOpenObject(pDim, id, AcDb::kForWrite) == Acad::eOk)
            {
                pDim->recomputeDimBlock();      // 重新生成带方框的匿名块
                pDim->recordGraphicsModified(); // 标记图形已更新
                pDim->draw();                   // 显式推送至渲染管线
                pDim->close();
            }
        }

        // 刷新屏幕缓冲区并更新显示
        actrTransactionManager->flushGraphics();
        acedUpdateDisplay();
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


void UniversalPicker::run(const Options& options, EntityProcessor processor, const ACHAR* prompt, UniversalPicker::SelectMode defaultSelectMode)
{
    acutPrintf(L"\n%s", prompt);

    // 模式选择提示
    const ACHAR* modePrompt = (defaultSelectMode == UniversalPicker::SelectMode::Batch) ?
        L"\n选择模式 [批量(B)/立即(I)]<B>：" :
        L"\n选择模式 [批量(B)/立即(I)]<I>：";

    UniversalPicker::SelectMode inputMode = defaultSelectMode;
    ACHAR keyword[2] = { 0 };
    bool validInput = false;

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
    }
    while (!validInput);

    if (inputMode == UniversalPicker::SelectMode::Batch)
    {
        UniversalPicker::batchSelect(options, processor);
    }
    else
    {
        UniversalPicker::immediateSelect(options, processor);
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
