#include "stdafx.h"
#include "Dimension.hpp"
#include "Common.hpp"

namespace Dimension
{
	void dimensionFix(AcDbObjectId objId)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}

		// 获取当前的标注文本
		AcString dimensionOriginText;
		pDim->dimensionText(dimensionOriginText);

		// 获取标注的实体实际尺寸值
		double measurementValue = .0;
		if (pDim->measurement(measurementValue) != Acad::eOk)
		{
			return;
		 }

		AcString measurementValueText;

		if (pDim->isKindOf(AcDb2LineAngularDimension::desc())) // 角度
		{
			int precision = pDim->dimadec();
			measurementValue = Common::rad2deg(measurementValue);
			Common::double2AcString(measurementValue, measurementValueText, precision);
			measurementValueText += Common::DimSymbol::Degree;
		}
		else
		{
			int precision = pDim->dimdec();
			Common::double2AcString(measurementValue, measurementValueText, precision);

			if (pDim->isKindOf(AcDbDiametricDimension::desc())) // 直径
			{
				measurementValueText = Common::DimSymbol::Diameter + measurementValueText;
			}
			else if (pDim->isKindOf(AcDbRadialDimension::desc())) // 半径
			{
				measurementValueText = Common::DimSymbol::Radius + measurementValueText;
			}
		}

		if (dimensionOriginText.isEmpty()) // 没有手改过标注内容的情况
		{
			pDim->setDimensionText(measurementValueText);
		}
		else
		{
			if (dimensionOriginText.find(Common::ACDB_DIM_TEXT_DEFAULT) == -1)
			{
				// 已经是固定文本，跳过处理
				return;
			}
			else
			{
				dimensionOriginText.replace(Common::ACDB_DIM_TEXT_DEFAULT, measurementValueText.kACharPtr());
				pDim->setDimensionText(dimensionOriginText.kACharPtr());
			}
		}
	}

	void dimensionResume(AcDbObjectId objId)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}
		pDim->setDimensionText(L"");
	}

	void addSurroundingCharsForDimension(AcDbObjectId objId, const ACHAR* left, const ACHAR* right)
	{
        AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
            return;
		}
		AcString dimensionText;
        pDim->dimensionText(dimensionText);

		AcString dimensionNewText;
		if (dimensionText.empty())
		{
			dimensionNewText.format(L"%s%s%s", left, Common::ACDB_DIM_TEXT_DEFAULT, right);
		}
		else
		{
			dimensionNewText.format(L"%s%s%s", left, dimensionText.kACharPtr(), right);
		}
        pDim->setDimensionText(dimensionNewText.kACharPtr());
	}

	void removeSurroundingCharsForDimension(AcDbObjectId objId, const ACHAR* left, const ACHAR* right)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}

		AcString text;
		pDim->dimensionText(text);

		if (text.isEmpty())
		{
			return;
		}

		// 使用 static_cast 明确转换类型，消除编译器关于类型缩减的警告
		int nTextLen = static_cast<int>(text.length());
		int nLeftLen = (left != nullptr) ? static_cast<int>(wcslen(left)) : 0;
		int nRightLen = (right != nullptr) ? static_cast<int>(wcslen(right)) : 0;

		// 检查是否匹配左侧
		bool bLeftMatch = (nLeftLen == 0);
		if (nLeftLen > 0 && nTextLen >= nLeftLen)
		{
			if (text.find(left) == 0)
			{
				bLeftMatch = true;
			}
		}

		// 检查是否匹配右侧
		bool bRightMatch = (nRightLen == 0);
		if (nRightLen > 0 && nTextLen >= nRightLen)
		{
			// 计算索引时也确保类型一致
			if (text.findLast(right) == (nTextLen - nRightLen))
			{
				bRightMatch = true;
			}
		}

		// 只有在参数要求的部分都匹配时才执行截取
		if (bLeftMatch && bRightMatch)
		{
			int nMidLen = nTextLen - nLeftLen - nRightLen;

			if (nMidLen >= 0)
			{
				AcString newText = text.mid(nLeftLen, nMidLen);

				// 如果结果为默认占位符或为空，则清空覆盖文字以恢复测量值显示
				if (newText == Common::ACDB_DIM_TEXT_DEFAULT || newText.isEmpty())
				{
					pDim->setDimensionText(L"");
				}
				else
				{
					pDim->setDimensionText(newText.kACharPtr());
				}
			}
		}
	}
}