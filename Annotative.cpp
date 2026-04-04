module;
#include "stdafx.h"
#include "dbAnnotativeObjectPE.h"
#include "dbObjectContextCollection.h"
#include "dbObjectContextManager.h"
#include "dbObjectContextInterface.h"

module Annotative;

namespace Annotative
{
	void applyCurrentAnnotativeScale(AcDbEntity* pEnt)
	{
        if (!pEnt)
        {
            return;
        }

        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
        AcDbAnnotativeObjectPE* pAnnoPE = AcDbAnnotativeObjectPE::cast(pEnt->queryX(AcDbAnnotativeObjectPE::desc()));
        if (pAnnoPE && pAnnoPE->annotative(pEnt))
        {
            AcDbObjectContextManager* pContextMgr = pDb->objectContextManager();
            if (pContextMgr)
            {
                AcDbObjectContextCollection* pCollection = pContextMgr->contextCollection(ACDB_ANNOTATIONSCALES_COLLECTION);
                if (pCollection)
                {
                    // 如果对象已在数据库中，传对象指针；如果在内存中，传 nullptr
                    AcDbObjectContext* pContext = pCollection->currentContext(pEnt->objectId().isNull() ? nullptr : pEnt);
                    AcDbObjectContextInterface* pContextIterface = AcDbObjectContextInterface::cast(
                        pEnt->queryX(AcDbObjectContextInterface::desc())
                    );
                    if (pContextIterface && pContext)
                    {
                        pContextIterface->addContext(pEnt, *pContext);
                    }
                }
            }
        }
	}

    Acad::ErrorStatus setObjAnnotative(AcDbObject* pObj, bool bAnnotative)
    {
        if (pObj == nullptr)
        {
            return Acad::eInvalidInput;
        }

        // 获取注释性协议扩展接口
        AcDbAnnotativeObjectPE* pAnnoPE = AcDbAnnotativeObjectPE::cast(
            pObj->queryX(AcDbAnnotativeObjectPE::desc())
        );

        if (pAnnoPE != nullptr)
        {
            return pAnnoPE->setAnnotative(pObj, bAnnotative);
        }

        return Acad::eNotApplicable;
    }
};