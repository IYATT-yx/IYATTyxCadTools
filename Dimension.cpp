/**
 * @file      Dimension.cpp
 * @brief     尺寸模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "stdafx.h"
#include "resource.h"

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
		acutPrintf(Common::loadString(IDS_MSG_OpBefore_FMT), dimData.dimensionText.constPtr());
		if (dimData.angle)
		{
			Common::double2AcString(
				Common::rad2deg(dimData.measured),
				measurementValueText,
				dimData.precision
			);
			measurementValueText += dimData.suffix;
		}
		else
		{
			Common::double2AcString(
				dimData.measured,
				measurementValueText,
				dimData.precision
			);
			measurementValueText = dimData.prefix + measurementValueText + dimData.suffix;
		}

		if (dimData.dimensionText.empty())
		{
			pDim->setDimensionText(measurementValueText);
			acutPrintf(Common::loadString(IDS_MSG_OpAfter_FMT), measurementValueText.constPtr());
		}
		else
		{
			if (dimData.dimensionText.find(Common::measValuePlaceholder) == -1)
			{
				// 已经是固定文本，无处理
			}
			else
			{
				dimData.dimensionText.replace(Common::measValuePlaceholder, measurementValueText.constPtr());
                pDim->setDimensionText(dimData.dimensionText.constPtr());
			}
            acutPrintf(Common::loadString(IDS_MSG_OpAfter_FMT), dimData.dimensionText.constPtr());
		}
	}

	// 当前方案不能完全还原尺寸文本
	// 除了公差的自定义内容会丢失
	void dimensionRelink(const AcDbObjectId& objId)
	{
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(objId, AcDb::kForWrite);
		if (pDim == nullptr)
		{
			return;
		}

		double upperDeviation = pDim->dimtp();
		double lowerDeviation = pDim->dimtm() * -1;
		const wchar_t* dimensionText = pDim->dimensionText();
		Adesk::UInt16 colorIndex = Common::getEntityActualColorIndex(pDim);

		acutPrintf(Common::loadString(IDS_MSG_OpBefore_FMT), dimensionText);
		AcString resultText;
		if (upperDeviation == 0 && lowerDeviation == 0)
		{
			resultText = L"";
		}
		else
		{
			int tolerancePrecision = pDim->dimtdec();
			AcString strUpperDeviation, strLowerDeviation;
			AcString units = L"";
			if (pDim->isKindOf(AcDb2LineAngularDimension::desc()))
			{
				units = L"%%D";
			}
			if (upperDeviation + lowerDeviation == 0)
			{
				
				Common::double2AcString(fabs(upperDeviation), strUpperDeviation, tolerancePrecision);
				resultText.format(L"%s{}{\\C%d;%%%%P%s%s}", Common::measValuePlaceholder, colorIndex, strUpperDeviation.constPtr(), units.constPtr());
			}
			else
			{
				Common::double2AcString(upperDeviation, strUpperDeviation, tolerancePrecision, true, true);
				Common::double2AcString(lowerDeviation, strLowerDeviation, tolerancePrecision, true, true);
				resultText.format(L"%s{}{\\H0.71x;\\C%d;\\S%s%s^%s%s;}", Common::measValuePlaceholder, colorIndex, strUpperDeviation.constPtr(), units.constPtr(), strLowerDeviation.constPtr(), units.constPtr());
			}
		}
		pDim->setDimensionText(resultText.constPtr());
		acutPrintf(Common::loadString(IDS_MSG_OpAfter_FMT), resultText.constPtr());
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
			dimensionNewText.format(L"%s%s%s", leftNew, Common::measValuePlaceholder, rightNew);
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
				if (newText == Common::measValuePlaceholder || newText.isEmpty())
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
		AcDbDimension* pDim = Common::getObject<AcDbDimension>(id, AcDb::kForWrite);
		if (pDim == nullptr)
		{
            data.status = false;
			return;
		}

		pDim->measurement(data.measured);
        pDim->dimensionText(data.dimensionText);

		// AutoCAD Mechanical 通过增强尺寸标注设置的公差最终会生成控制字符
		// 如果文字替代中没有检测到大括号包裹的 \S 或 %%P 控制字符，则认为没有设置公差
		// 只是 CAD 内部设置了初始正向偏差和负向偏差为 0.1，需要将其置 0，避免后续错误处理。
		if (!Dimension::isEnhancedToleranceApplied(data.dimensionText))
		{
			pDim->setDimtp(0);
			pDim->setDimtm(0);
		}
		data.upperDeviation = pDim->dimtp(); // 上极限偏差 = 正向偏差
		data.lowerDeviation = pDim->dimtm() * -1.0; // 下极限偏差 = -1 * 负向偏差

		if (pDim->isKindOf(AcDb2LineAngularDimension::desc())) // 角度精度
		{
			data.precision = pDim->dimadec();
			data.angle = true;
			data.suffix = Common::SymbolCodes::Degree;
            TextUtil::resolveControlCodes(data.suffix);
		}
		else // 线性精度
		{
			data.precision = pDim->dimdec();
			data.angle = false;
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
		data.tolerancePrecision = pDim->dimtdec(); // 极限偏差精度
		data.status = true;

		// 解析后的尺寸内容
		if (data.dimensionText.isEmpty())
		{
			data.text = Common::measValuePlaceholder;
		}
		else
		{
			data.text = data.dimensionText;
		}
		AcString newValue, strMeasurement;
		double measured = data.dimensionValue();
		Common::double2AcString(measured, strMeasurement, data.precision);
		newValue.format(L"%s%s%s", data.prefix.constPtr(), strMeasurement.constPtr(), data.suffix.constPtr());
        data.text.replace(Common::measValuePlaceholder, newValue.constPtr());
		TextUtil::resolveControlCodes(data.text);
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
			pDim->setDimdec(iDimPrec); // 线型尺寸
			pDim->setDimadec(iDimPrec); // 角度尺寸
		}
		if (iTolPrec >= 0)
		{
			pDim->setDimtdec(iTolPrec);
			double dUpper = pDim->dimtp();
            double dLower = pDim->dimtm() * -1.0; // 注意内部符号是相反的，所以这里取相反数
			std::wstring dimText = pDim->dimensionText();

			std::wregex stackRegex(L"\\\\S([^\\^;]+)\\^([^;]+);");
			std::wregex symRegex(L"%%P([0-9]+\\.?[0-9]*)", std::regex_constants::icase);
			std::wsmatch match;

			if (std::regex_search(dimText, match, stackRegex))
			{
				AcString acStrUpper, acStrLower;
				// 上偏差
				Common::double2AcString(dUpper, acStrUpper, iTolPrec, true, true);
				// 下偏差
				Common::double2AcString(dLower, acStrLower, iTolPrec, true, true);
				// 构造新的堆叠字段并执行替换
				std::wstring newStack = std::wstring(L"\\S") + acStrUpper.constPtr() + L"^" + acStrLower.constPtr() + L";";
				dimText.replace(match.position(0), match.length(0), newStack);
				pDim->setDimensionText(dimText.c_str());
			}
			else if (std::regex_search(dimText, match, symRegex))
			{
				if (std::abs(dUpper + dLower) < Common::Epsilon)
				{
					AcString acStrSym;
					Common::double2AcString(std::abs(dUpper), acStrSym, iTolPrec, false, false);
					std::wstring newSym = std::wstring(L"%%P") + acStrSym.constPtr();
					dimText.replace(match.position(0), match.length(0), newSym);
					pDim->setDimensionText(dimText.c_str());
				}
				else
				{
					acutPrintf(Common::loadString(IDS_WARN_NotStandardDimensionText));
				}
			}
		}
	}

	bool isEnhancedToleranceApplied(const AcString& text)
	{
		if (text.isEmpty())
		{
			return false;
		}

		int nStartSearch = 0;

		// 遍历所有可能存在的大括号对（处理嵌套或多段格式组）
		while (true)
		{
			int nOpen = text.find(L"{", nStartSearch);
			if (nOpen == -1)
			{
				break;
			}

			int nClose = text.find(L"}", nOpen);
			if (nClose == -1)
			{
				break;
			}

			// 截取当前大括号对内部的子字符串
			// mid 参数2为长度：nClose - nOpen + 1 包含括号本身
			AcString sInner = text.mid(nOpen, nClose - nOpen + 1);

			// 判定该格式组内是否存在堆叠符或正负号
			if (sInner.findNoCase(L"\\S") != -1 || sInner.findNoCase(L"%%P") != -1 || sInner.findNoCase(L"%%p") != -1)
			{
				return true;
			}

			// 更新搜索起点，继续寻找下一对大括号
			nStartSearch = nClose + 1;
		}
		return false;
	}
}