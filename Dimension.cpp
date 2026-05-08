module;
#include "stdafx.h"

module Dimension;
import Common;
import TextUtil;
import std;

namespace Dimension
{
	void dimensionSolidify(const AcDbObjectId& objId)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}

		Dimension::DimensionData dimData{}; // 存储标注数据
		AcString measurementValueText; // 存储测量值文本（带符号、单位）

		Dimension::readDim(objId, dimData);
		if (dimData.isAngle)
		{
			Common::double2AcString(
				Common::rad2deg(dimData.measuredValue),
				measurementValueText,
				dimData.measuredValuePrecision
			);
			measurementValueText += dimData.suffix;
		}
		else
		{
			Common::double2AcString(
				dimData.measuredValue,
				measurementValueText,
				dimData.measuredValuePrecision
			);
			measurementValueText = dimData.prefix + measurementValueText + dimData.suffix;
		}

		if (dimData.dimText.empty())
		{
			pDim->setDimensionText(measurementValueText);
		}
		else
		{
			if (dimData.dimText.find(Common::ACDB_DIM_TEXT_DEFAULT) == -1)
			{
				// 已经是固定文本，跳过处理
				return;
			}
			else
			{
				dimData.dimText.replace(Common::ACDB_DIM_TEXT_DEFAULT, measurementValueText.constPtr());
                pDim->setDimensionText(dimData.dimText.constPtr());
			}
		}
	}

	void dimensionRelink(const AcDbObjectId& objId)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}
		pDim->setDimensionText(L"");
	}

	void addSurroundingCharsForDimension(const AcDbObjectId& objId, const wchar_t* left, const wchar_t* right, bool isLGdt, bool isRGdt)
	{
        AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
            return;
		}
		AcString dimensionText;
        pDim->dimensionText(dimensionText);

		// 处理 GDT 字体包裹
		AcString leftWrapedString = isLGdt ? Common::wrapWithGdtFont(left) : AcString(left);
		const wchar_t* leftNew = leftWrapedString.constPtr();
		AcString rightWrapedString = isRGdt ? Common::wrapWithGdtFont(right) : AcString(right);
        const wchar_t* rightNew = rightWrapedString.constPtr();

		AcString dimensionNewText;
		if (dimensionText.empty())
		{
			dimensionNewText.format(L"%s%s%s", leftNew, Common::ACDB_DIM_TEXT_DEFAULT, rightNew);
		}
		else
		{
			dimensionNewText.format(L"%s%s%s", leftNew, dimensionText.constPtr(), rightNew);
		}
        pDim->setDimensionText(dimensionNewText.constPtr());
	}

	void removeSurroundingCharsForDimension(const AcDbObjectId& objId, const wchar_t* left, const wchar_t* right, bool isLGdt, bool isRGdt)
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

		// 处理 GDT 字体包裹
		AcString leftWrapedString = isLGdt ? Common::wrapWithGdtFont(left) : AcString(left);
		const wchar_t* leftNew = leftWrapedString.constPtr();
		AcString rightWrapedString = isRGdt ? Common::wrapWithGdtFont(right) : AcString(right);
		const wchar_t* rightNew = rightWrapedString.constPtr();

		// 使用 static_cast 明确转换类型，消除编译器关于类型缩减的警告
		int nTextLen = static_cast<int>(text.length());
		int nLeftLen = (leftNew != nullptr) ? static_cast<int>(wcslen(leftNew)) : 0;
		int nRightLen = (rightNew != nullptr) ? static_cast<int>(wcslen(rightNew)) : 0;

		// 检查是否匹配左侧
		bool bLeftMatch = (nLeftLen == 0);
		if (nLeftLen > 0 && nTextLen >= nLeftLen)
		{
			if (text.find(leftNew) == 0)
			{
				bLeftMatch = true;
			}
		}

		// 检查是否匹配右侧
		bool bRightMatch = (nRightLen == 0);
		if (nRightLen > 0 && nTextLen >= nRightLen)
		{
			// 计算索引时也确保类型一致
			if (text.findLast(rightNew) == (nTextLen - nRightLen))
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
					pDim->setDimensionText(newText.constPtr());
				}
			}
		}
	}

	void setAndUnsetBasicBox(const AcDbObjectId& objId, bool isSet)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}

		double absDimgap = abs(pDim->dimgap());
		if (isSet)
		{
			pDim->setDimgap(-absDimgap);
		}
		else
		{
			pDim->setDimgap(absDimgap);
		}
	}

	void setAndUnsetRefDim(const AcDbObjectId& objId, bool isSet)
	{
		const wchar_t* left = L"(";
        const wchar_t* right = L")";
		if (isSet)
		{
			Dimension::addSurroundingCharsForDimension(objId, left, right);
		}
		else
		{
            Dimension::removeSurroundingCharsForDimension(objId, left, right);
		}
	}

	void readDim(const AcDbObjectId& id, DimensionData& data)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(id, AcDb::kForRead);
		if (pDim == nullptr)
		{
            data.status = false;
			return;
		}

		pDim->measurement(data.measuredValue);
        pDim->dimensionText(data.dimText);
		TextUtil::parseDimensionTolerance(data.dimText, data.tolUpper, data.tolLower);

		if (pDim->isKindOf(AcDb2LineAngularDimension::desc())) // 角度精度
		{
			data.measuredValuePrecision = pDim->dimadec();
			data.isAngle = true;
			data.suffix = Common::SymbolCodes::Degree;
            TextUtil::resolveControlCodes(data.suffix);
		}
		else // 线性精度
		{
			data.measuredValuePrecision = pDim->dimdec();
			data.isAngle = false;
			if (pDim->isKindOf(AcDbDiametricDimension::desc()))
			{
                data.prefix = Common::SymbolCodes::Diameter;
			}
			else if (pDim->isKindOf(AcDbRadialDimension::desc()))
			{
				data.prefix = Common::SymbolCodes::Radius;
			}
			TextUtil::resolveControlCodes(data.prefix);
		}
		data.tolPrecision = pDim->dimtdec(); // 极限偏差精度
		data.status = true;

		// 纯文本标注内容
		if (data.dimText.isEmpty())
		{
			data.plainText = Common::ACDB_DIM_TEXT_DEFAULT;
		}
		else
		{
			data.plainText = data.dimText;
		}
		AcString newValue, strMeasurement;
		double measuredValue = data.measuredValue;
		if (data.isAngle)
		{
			measuredValue = data.degreeValue();
		}
		Common::double2AcString(measuredValue, strMeasurement, data.measuredValuePrecision);
		newValue.format(L"%s%s%s", data.prefix.constPtr(), strMeasurement.constPtr(), data.suffix.constPtr());
        data.plainText.replace(Common::ACDB_DIM_TEXT_DEFAULT, newValue.constPtr());
		TextUtil::resolveControlCodes(data.plainText);
	}

	void setDimensionTolerancePreccision(const AcDbObjectId& id, const int& iDimPrec, const int& iTolPrec)
	{
        AcDbDimension* pDim = Common::getObject<AcDbDimension>(id, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}

		if (iDimPrec >= 0)
		{
			pDim->setDimdec(iDimPrec);
		}
		if (iTolPrec >= 0)
		{
			pDim->setDimtdec(iTolPrec);
			double dUpper = pDim->dimtp();
            double dLower = pDim->dimtm() * -1.0; // 注意内部符号是相反的，所以这里取相反数
			std::wstring dimText = pDim->dimensionText();

			// 定位文本替代里的 \S 公差字段
			// 正则解释：匹配 \S 段落直至分号结束
			std::wregex tolRegex(L"\\\\S([^\\^;]+)\\^([^;]+);");
			std::wsmatch match;

			if (std::regex_search(dimText, match, tolRegex))
			{
				AcString acStrUpper, acStrLower;

				// 上偏差
				Common::double2AcString(dUpper, acStrUpper, iTolPrec);
				std::wstring newUpper = acStrUpper.constPtr();
				if (dUpper > 0)
				{
					newUpper = L"+" + newUpper;
				}
				else if (fabs(dUpper) <= Common::Epsilon)
				{
					newUpper = L" " + newUpper;
				}

				// 下偏差
				Common::double2AcString(dLower, acStrLower, iTolPrec);
				std::wstring newLower = acStrLower.constPtr();
				if (dLower > 0)
				{
					newLower = L"+" + newLower;
				}
				else if(fabs(dLower) <= Common::Epsilon)
				{
                    newLower = L" " + newLower;
				}

				// 构造新的堆叠字段并执行替换
				std::wstring newStack = L"\\S" + newUpper + L"^" + newLower + L";";
				dimText.replace(match.position(0), match.length(0), newStack);
				pDim->setDimensionText(dimText.c_str());
			}
		}
	}
}