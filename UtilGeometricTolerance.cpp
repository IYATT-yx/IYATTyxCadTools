/**
 * @file      UtilGeometricTolerance.cpp
 * @brief     机械版几何公差模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

module UtilGeometricTolerance;
import UtilCommon;
import UtilText;

namespace UtilGeometricTolerance
{
	void readFcf(const AcDbObjectId& id, GeometricToleranceData& data)
	{
		AcmFCF* pFcf = UtilCommon::getObject<AcmFCF>(id, AcDb::kForRead);
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
		UtilGeometricTolerance::GeometricToleranceRow& row0 = data.rows[0];
		row0.gdtSymbolType = pFcf->symbol();
		row0.name = UtilGeometricTolerance::getGdtNameMap().at(row0.gdtSymbolType);
		row0.gdtSymbol = UtilGeometricTolerance::gdtSymbolMap[row0.gdtSymbolType];
		row0.value = pFcf->value(Acm::FCFTolerance);
		row0.primary = pFcf->value(Acm::FCFDatumPrimary);
        row0.secondary = pFcf->value(Acm::FCFDatumSecondary);
        row0.tertiary = pFcf->value(Acm::FCFDatumTertiary);

		// 第 2 个
		UtilGeometricTolerance::GeometricToleranceRow& row1 = data.rows[1];
		row1.gdtSymbolType = pFcf->symbol2();
		if (row1.gdtSymbolType != Acm::kNoType)
		{
            row1.name = UtilGeometricTolerance::getGdtNameMap().at(row1.gdtSymbolType);
            row1.gdtSymbol = UtilGeometricTolerance::gdtSymbolMap[row1.gdtSymbolType];
			row1.value = pFcf->value(Acm::FCFTolerance2);
            row1.primary = pFcf->value(Acm::FCFDatumPrimary2);
            row1.secondary = pFcf->value(Acm::FCFDatumSecondary2);
            row1.tertiary = pFcf->value(Acm::FCFDatumTertiary2);

			// 第 3 个
			UtilGeometricTolerance::GeometricToleranceRow& row2 = data.rows[2];
			row2.gdtSymbolType = pFcf->symbol3();
			if (row2.gdtSymbolType != Acm::kNoType)
			{
                row2.name = UtilGeometricTolerance::getGdtNameMap().at(row2.gdtSymbolType);
                row2.gdtSymbol = UtilGeometricTolerance::gdtSymbolMap[row2.gdtSymbolType];
                row2.value = pFcf->value(Acm::FCFTolerance3);
                row2.primary = pFcf->value(Acm::FCFDatumPrimary3);
                row2.secondary = pFcf->value(Acm::FCFDatumSecondary3);
                row2.tertiary = pFcf->value(Acm::FCFDatumTertiary3);
			}
		}

		UtilGeometricTolerance::resolveData(data);

        data.status = true;
	}

	void resolveData(UtilGeometricTolerance::GeometricToleranceData& data)
	{
		for (int i = 0; i < UtilGeometricTolerance::GeometricToleranceDataLen; ++i)
		{
			UtilGeometricTolerance::GeometricToleranceRow& row = data.rows[i];
			if (row.gdtSymbolType != Acm::kNoType)
			{
				UtilText::resolveControlCodes(row.value);
				UtilText::resolveControlCodes(row.primary);
                UtilText::resolveControlCodes(row.secondary);
				UtilText::resolveControlCodes(row.tertiary);
			}
			else
			{
				break;
			}
		}
	}
}