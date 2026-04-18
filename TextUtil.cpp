module;
#include "StdAfx.h"
#include "resource.h"
#include <regex>

module TextUtil;
import Common;
import UniversalPicker;
import Annotative;

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

	bool readMText(const AcDbObjectId& id, AcString& text, bool isRawContents)
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
		return true;
	}

	bool readDText(const AcDbObjectId& id, AcString& text, bool isRawContents)
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
        return true;
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
		if (TextUtil::readMText(entId, content, true))
		{
			return true;
		}
		else if (TextUtil::readDText(entId, content, true))
		{
            return true;
		}
        return false;
	}

	void updateTextEntityContent(const AcDbObjectId& id, const AcString& content)
	{
		AcDbMText* pText = Common::getObject<AcDbMText>(id, AcDb::kForWrite);
		if (pText != nullptr)
		{
			pText->setContents(content);
			return;
		}
		AcDbText* pDText = Common::getObject<AcDbText>(id, AcDb::kForWrite);
		if (pDText != nullptr)
		{
            pDText->setTextString(content);
            return;
		}
	}

	void createMTextMatrix(double colWidth, double colStep, double rowStep, const std::vector<std::vector<AcString>>& matrixData, AcGePoint3d topLeftPt, double dLineSpacingFactor)
	{
		if (dLineSpacingFactor < 0.25 || dLineSpacingFactor > 4.0)
		{
			AfxMessageBox(Common::loadString(IDS_Err_LineSpacingFactorOutOfRange),MB_OK | MB_ICONERROR);
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
			acutPrintf(Common::loadString(IDS_Err_GetCurrentScaleValue_FMT), s);
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
					pMText->setTextHeight(Common::defaultTextHeight * s);
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
}