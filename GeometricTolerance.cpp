/**
 * @file      GeometricTolerance.cpp
 * @brief     机械版几何公差模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module GeometricTolerance;
import Common;
import TextUtil;

namespace GeometricTolerance
{
	void readFcf(const AcDbObjectId& id, GeometricToleranceData& data)
	{
		AcmFCF* pFcf = Common::getObject<AcmFCF>(id, AcDb::kForRead);
		if (pFcf == nullptr)
		{
			data.status = false;
			return;
		}

		// 句柄
		AcDbHandle handle = nullptr;
		pFcf->getAcDbHandle(handle);
		data.u64handle = handle;

		// 第 1 个
		GeometricTolerance::GeometricToleranceRow& row0 = data.rows[0];
		row0.gdtSymbolType = pFcf->symbol();
		row0.name = GeometricTolerance::gdtNameMap[row0.gdtSymbolType];
		row0.gdtSymbol = GeometricTolerance::gdtSymbolMap[row0.gdtSymbolType];
		row0.value = pFcf->value(Acm::FCFTolerance);
		row0.primary = pFcf->value(Acm::FCFDatumPrimary);
        row0.secondary = pFcf->value(Acm::FCFDatumSecondary);
        row0.tertiary = pFcf->value(Acm::FCFDatumTertiary);

		// 第 2 个
		GeometricTolerance::GeometricToleranceRow& row1 = data.rows[1];
		row1.gdtSymbolType = pFcf->symbol2();
		if (row1.gdtSymbolType != Acm::kNoType)
		{
            row1.name = GeometricTolerance::gdtNameMap[row1.gdtSymbolType];
            row1.gdtSymbol = GeometricTolerance::gdtSymbolMap[row1.gdtSymbolType];
			row1.value = pFcf->value(Acm::FCFTolerance2);
            row1.primary = pFcf->value(Acm::FCFDatumPrimary2);
            row1.secondary = pFcf->value(Acm::FCFDatumSecondary2);
            row1.tertiary = pFcf->value(Acm::FCFDatumTertiary2);

			// 第 3 个
			GeometricTolerance::GeometricToleranceRow& row2 = data.rows[2];
			row2.gdtSymbolType = pFcf->symbol3();
			if (row2.gdtSymbolType != Acm::kNoType)
			{
                row2.name = GeometricTolerance::gdtNameMap[row2.gdtSymbolType];
                row2.gdtSymbol = GeometricTolerance::gdtSymbolMap[row2.gdtSymbolType];
                row2.value = pFcf->value(Acm::FCFTolerance3);
                row2.primary = pFcf->value(Acm::FCFDatumPrimary3);
                row2.secondary = pFcf->value(Acm::FCFDatumSecondary3);
                row2.tertiary = pFcf->value(Acm::FCFDatumTertiary3);
			}
		}

		GeometricTolerance::resolveData(data);

        data.status = true;
	}

	void resolveData(GeometricTolerance::GeometricToleranceData& data)
	{
		for (int i = 0; i < GeometricTolerance::GeometricToleranceDataLen; ++i)
		{
			GeometricTolerance::GeometricToleranceRow& row = data.rows[i];
			if (row.gdtSymbolType != Acm::kNoType)
			{
				TextUtil::resolveControlCodes(row.value);
				TextUtil::resolveControlCodes(row.primary);
                TextUtil::resolveControlCodes(row.secondary);
				TextUtil::resolveControlCodes(row.tertiary);
			}
			else
			{
				break;
			}
		}
	}
}