module;
#include "StdAfx.h"
#include <regex>

module TextUtil;
import Common;
import UniversalPicker;

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

	bool readMText(AcDbObjectId& id, AcString& text, bool isRawContents)
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

	bool readDText(AcDbObjectId& id, AcString& text, bool isRawContents)
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
}