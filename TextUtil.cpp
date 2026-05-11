/**
 * @file      TextUtil.cpp
 * @brief     文本模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "resource.h"

module TextUtil;
import Common;
import UniversalPicker;
import Annotative;
import CsvModule;
import std;
import AcadVarUtil;

namespace TextUtil
{
	void parseDimensionTolerance(const AcString& dimText, double& tolUpper, double& tolLower)
	{
		tolUpper = 0;
        tolLower = 0;
		std::wsmatch match;
		std::wstring text(dimText.constPtr());

		// 优先解析 \s 格式的极限偏差
		std::wregex re(LR"(\\S([+-]?[0-9]*\.?[0-9]+)°?\^([+-]?[0-9]*\.?[0-9]+)°?;)");
		auto begin = std::wsregex_iterator(text.begin(), text.end(), re);
        auto end = std::wsregex_iterator();
		if (begin != end)
		{
			std::wsmatch lastMatch;
			for (auto it = begin; it != end; ++it)
			{
				lastMatch = *it;
			}
			tolUpper = _wtof(lastMatch[1].str().c_str());
			tolLower = _wtof(lastMatch[2].str().c_str());
			return;
		}

		// 对称极限偏差
		std::wregex re2(LR"(%%[pP]\s*([0-9]*\.?[0-9]+))");
		if (std::regex_search(text, match, re2))
		{
			double tol = _wtof(match[1].str().c_str());
			tolUpper = tol;
			tolLower = -tol;
			return;
		}
	}

	bool readMText(const AcDbObjectId& id, AcString& text, bool isRawContents, AcGePoint3d* pPos)
	{
		AcDbMText* pMText = Common::getObject<AcDbMText>(id, AcDb::kForRead);
		if (pMText == nullptr)
		{
			return false;
		}

		if (isRawContents)
		{
			pMText->contents(text);
		}
		else
		{
			pMText->text(text);
		}

		if (pPos != nullptr)
		{
			Common::getEntityCenter(pMText, pPos);
		}

		return true;
	}

	bool readDText(const AcDbObjectId& id, AcString& text, bool isRawContents, AcGePoint3d* pPos)
	{
		AcDbText* pText = Common::getObject<AcDbText>(id, AcDb::kForRead);
		if (pText == nullptr)
		{
			return false;
		}

		pText->textString(text);
		if (!isRawContents)
		{
			TextUtil::resolveControlCodes(text);
		}

		if (pPos != nullptr)
		{
			Common::getEntityCenter(pText, pPos);
		}

		return true;
	}

	bool readTextField(const AcDbObjectId& id, AcString& text)
	{
		AcDbEntity* pText = Common::getObject<AcDbEntity>(id, AcDb::kForRead);
		if (pText == nullptr)
		{
			return false;
		}
		if (!pText->isKindOf(AcDbMText::desc()) && !pText->isKindOf(AcDbText::desc()))
		{
			return false;
		}

		AcDbField* pField = nullptr;
		if (pText->getField(L"TEXT", pField, AcDb::kForRead) == Acad::eOk)
		{
			if (pField != nullptr)
			{
                pField->getFieldCode(text, AcDbField::kAddMarkers);
				pField->close();
				return true;
			}
		}
		return false;
	}

	void resolveControlCodes(AcString& text)
	{
		AcDbMText mtext;
		mtext.setContents(text);
		mtext.text(text);
	}

	bool getSelectedTextRawContent(AcString& content)
	{
		AcDbObjectId srcId;
		resbuf* filterRb = UniversalPicker::buildFilter(&TextUtil::textClassList);

		AcDbObjectId entId = UniversalPicker::getSelectedSingleEntityId(&TextUtil::textClassList);
		if (TextUtil::readTextField(entId, content))
		{
			return true;
		}
		if (TextUtil::readMText(entId, content, true))
		{
			return true;
		}
		if (TextUtil::readDText(entId, content, true))
		{
			return true;
		}
        return false;
	}

	void updateTextEntityContent(const AcDbObjectId& id, const AcString& content)
	{
		AcDbEntity* pText = Common::getObject<AcDbEntity>(id, AcDb::kForWrite);
		if (pText == nullptr)
		{
			return;
		}
		AcDbMText* pMText = AcDbMText::cast(pText);
		AcDbText* pDText = AcDbText::cast(pText);
		if (pMText == nullptr && pDText == nullptr)
		{
			return;
		}

		auto setTextContent = [&]()
			{
				if (pMText != nullptr)
				{
					pMText->setContents(content.constPtr());
				}
				else if (pDText != nullptr)
				{
					pDText->setTextString(content.constPtr());
				}
			};


		if (content.find(L"AcObjProp") != -1 || content.find(L"AcVar") != -1)
		{
			AcDbField* pField = new AcDbField();
			if (pField != nullptr)
			{
				AcDbField::FieldCodeFlag fFlag = static_cast<AcDbField::FieldCodeFlag>(static_cast<int>(AcDbField::kFieldCode) | static_cast<int>(AcDbField::kTextField));
				pField->setFieldCode(content, fFlag);

				AcDbField::EvalOption eOpt = static_cast<AcDbField::EvalOption>(static_cast<int>(AcDbField::kOnRegen) | static_cast<int>(AcDbField::kOnSave));
				pField->setEvaluationOption(eOpt);

				if (pField->evaluate(AcDbField::kDemand, pText->database()) == Acad::eOk)
				{
					AcDbObjectId newFieldId;
					pText->setField(L"TEXT", pField, newFieldId);
				}
				else
				{
					setTextContent();
				}
				pField->close();
			}
		}
		else
		{
			setTextContent();
		}

		pText->recordGraphicsModified();
	}

	void createMTextMatrix(double colWidth, double colStep, double rowStep, const CsvModule::AcStringMatrix& matrixData, AcGePoint3d topLeftPt, double dLineSpacingFactor)
	{
		if (dLineSpacingFactor < 0.25 || dLineSpacingFactor > 4.0)
		{
			AfxMessageBox(Common::loadString(IDS_ERR_LineSpacingFactorOutOfRange),MB_OK | MB_ICONERROR);
			return;
		}

		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		if (!pDb)
		{
			return;
		}

		double s = Annotative::getCurrentScaleValue();
		if (s < 0)
		{
			s = 1.0;
			acutPrintf(Common::loadString(IDS_ERR_GetCurrentScaleValue_FMT), s);
		}

		for (size_t row = 0; row < matrixData.size(); ++row)
		{
			for (size_t col = 0; col < matrixData[row].size(); ++col)
			{
				const AcString& cellText = matrixData[row][col];
				if (cellText.isEmpty())
				{
					continue;
				}

				AcDbMText* pMText = new AcDbMText();
				if (pMText)
				{
					pMText->setDatabaseDefaults(pDb);
					pMText->setContents(cellText);
					pMText->setAttachment(AcDbMText::kTopLeft);
					pMText->setWidth(colWidth * s);
					double TEXTSIZE;
					if (!AcadVarUtil::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
					{
						AfxMessageBox(Common::loadString(IDS_ERR_GetAcadVar), MB_OK | MB_ICONERROR);
						return;
					}
					pMText->setTextHeight(TEXTSIZE * s);
					pMText->setLineSpacingFactor(dLineSpacingFactor);

					AcGePoint3d currentPt;
					currentPt.x = topLeftPt.x + (col * colStep);
					currentPt.y = topLeftPt.y - (row * rowStep);
					currentPt.z = topLeftPt.z;
					pMText->setLocation(currentPt);

					AcDbBlockTableRecord* pBTR = Common::getObject<AcDbBlockTableRecord>(pDb->currentSpaceId(), AcDb::kForWrite);
					if (pBTR != nullptr)
					{
						if (pBTR->appendAcDbEntity(pMText) == Acad::eOk)
						{
							Annotative::setObjAnnotative(pMText, true);
							Annotative::applyCurrentAnnotativeScale(pMText);
							pMText->close();
						}
					}
					else
					{
						delete pMText;
					}
				}
			}
		}
	}

	void structureTextToAcStringMatrix(const TextUtil::TextEntityDataList& elements, double xTol, double yTol, CsvModule::AcStringMatrix& matrix)
	{
		matrix.clear();
		if (elements.empty())
		{
			return;
		}

		// --- 1. 纵向聚类（行识别） ---
		// 先按 Y 坐标降序排序，确保从上往下扫描
		std::vector<TextEntityData> sortedY = elements;
		std::sort(sortedY.begin(), sortedY.end(), [](const TextEntityData& a, const TextEntityData& b)
			{
				return a.pos.y > b.pos.y;
			});

		std::vector<std::vector<TextEntityData>> rows;
		for (const auto& el : sortedY)
		{
			bool found = false;
			for (auto& row : rows)
			{
				if (std::fabs(row[0].pos.y - el.pos.y) <= yTol)
				{
					row.push_back(el);
					found = true;
					break;
				}
			}
			if (!found)
			{
				rows.push_back({ el });
			}
		}

		// --- 2. 识别全局列基准（参考点） ---
		// 依然提取全局 X 分布，但我们需要确保列的数量是稳定的
		std::vector<double> allX;
		for (const auto& el : elements)
		{
			allX.push_back(el.pos.x);
		}
		std::sort(allX.begin(), allX.end());

		std::vector<double> colAnchors;
		if (!allX.empty())
		{
			double clusterSum = allX[0];
			int clusterCount = 1;
			for (size_t i = 1; i < allX.size(); ++i)
			{
				if (allX[i] - allX[i - 1] <= xTol)
				{
					clusterSum += allX[i];
					clusterCount++;
				}
				else
				{
					colAnchors.push_back(clusterSum / clusterCount);
					clusterSum = allX[i];
					clusterCount = 1;
				}
			}
			colAnchors.push_back(clusterSum / clusterCount);
		}

		// --- 3. 映射到矩阵 ---
		matrix.resize(rows.size());
		for (size_t i = 0; i < rows.size(); ++i)
		{
			matrix[i].assign(colAnchors.size(), L"");

			// 关键修正：在每一行内部，也将元素按 X 坐标从小到大排序
			// 这样可以确保在该行内，元素在物理上的先后顺序是确定的
			std::sort(rows[i].begin(), rows[i].end(), [](const TextEntityData& a, const TextEntityData& b)
				{
					return a.pos.x < b.pos.x;
				});

			for (const auto& el : rows[i])
			{
				// 寻找最匹配的全局列索引
				size_t bestCol = 0;
				double minDiff = (std::numeric_limits<double>::max)();
				for (size_t c = 0; c < colAnchors.size(); ++c)
				{
					double diff = std::fabs(el.pos.x - colAnchors[c]);
					if (diff < minDiff)
					{
						minDiff = diff;
						bestCol = c;
					}
				}

				if (matrix[i][bestCol].isEmpty())
				{
					matrix[i][bestCol] = el.text;
				}
				else
				{
					matrix[i][bestCol] += L" " + el.text;
				}
			}
		}
	}
}