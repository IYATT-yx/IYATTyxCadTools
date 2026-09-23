/**
 * @file      TextUtil.cpp
 * @brief     文本模块实现。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "GenericPairEditDlg.hpp"

module TextUtil;
import Common;
import UniversalPicker;
import Annotative;
import CsvModule;
import std;
import AcadVarUtil;
import Translator;
import Commands;
import FileDialog;

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
			AfxMessageBox(_(L"行距比例不可小于0.25，且不可大于4.0。"), MB_OK | MB_ICONERROR);
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
			acutPrintf(_(L"\n获取注释比例缩放系数失败，已重置为：%g"), s);
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
						AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
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

namespace
{
	void cmdCloneText()
	{
		AcString asSrcTextContent;
		acutPrintf(_(L"\n请选择要复制的源文本对象"));
		if (!TextUtil::getSelectedTextRawContent(asSrcTextContent) || asSrcTextContent.isEmpty())
		{
			acutPrintf(_(L"取消操作"));
			return;
		}
		acutPrintf(_(L"\n读取到：%s\n"), asSrcTextContent.constPtr());

		acutPrintf(_(L"请选择要粘贴到的文本对象"));
		UniversalPicker::run(
			&TextUtil::textClassList,
			[&asSrcTextContent](const AcDbObjectId& id)
			{
				TextUtil::updateTextEntityContent(id, asSrcTextContent);
			},
			_(L"将多行/单行文本内容复制给其它多行/单行文本"),
			UniversalPicker::SelectMode::Immediate,
			false,
			UniversalPicker::SortMode::None,
			true
		);
	}

	void cmdImportCsvToMTextMatrix()
	{
		FileDialog::FileDialogFilterBuilder fileFilterBuilter;
		CString strFileFilter = fileFilterBuilter.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
		CString strFilePath = FileDialog::ShowOpenFileDialog(_(L"选择要导入的文件"), L"csv", strFileFilter);
		if (strFilePath.IsEmpty())
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		CsvModule::AcStringMatrix matrixData;
		CsvModule::readCsvToAcStringMatrix(strFilePath, matrixData);

		CAcModuleResourceOverride resOverride;
		GenericPairEditDlg dlg(_(L"从 CSV 文件导入数据生成多行文本矩阵"), _(L"参数"), _(L"使用提示"), false, true, true);
		CString strTipMTextMatrixParameter;
		double TEXTSIZE;
		if (!AcadVarUtil::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
		{
			AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
			return;
		}
		double scale = Annotative::getCurrentScaleValue();
		strTipMTextMatrixParameter.Format(_(L"输入3个不小于0的数，使用空格分隔，分别为：列宽、列步长、行步长。显示文字高度 = TEXTSIZE变量值%g × 注释比例缩放值%g = %g"), TEXTSIZE, scale, TEXTSIZE * scale);
		dlg.modifyEditControl(L"", strTipMTextMatrixParameter);

		std::vector<double> params;
		dlg.setValidatorAndParser([&](const CString& edit1, const CString& _) -> CString
			{
				const int paramsNumber = 3;
				if (!Common::parse(edit1, paramsNumber, [](double v) { return v > 0; }, params))
				{
					return strTipMTextMatrixParameter;
				}
				return GenericPairEditDlg::ValidatorOk;
			});

		if (dlg.DoModal() != IDOK)
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		ads_point pt{};
		if (acedGetPoint(nullptr, _(L"请选择位置"), pt) != RTNORM)
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		TextUtil::createMTextMatrix(params[0], params[1], params[2], matrixData, asPnt3d(pt));
	}

	void cmdSpatialTableExplorer()
	{
		CAcModuleResourceOverride resOverride;
		CString title = _(L"将多行/单行文本按空间位置表格化导出到 CSV 文件");
		GenericPairEditDlg dlg(title, _(L"参数"), _(L"使用提示"), false, true, true);

		// 默认列容差和行容差
		// 字高默认使用 TEXTSIZE 变量值，列容差默认按字高的 3 倍，行容差默认按字高的 1 倍（考虑注释比例缩放值）
		CString strInitParameter;
		double scale = Annotative::getCurrentScaleValue();
		double TEXTSIZE;
		if (!AcadVarUtil::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
		{
			AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
			return;
		}
		strInitParameter.Format(L"%g %g", TEXTSIZE * scale * 3, TEXTSIZE * scale * 1);
		dlg.modifyEditControl(strInitParameter, _(L"输入2个不小于0的数，使用空格分隔，分别为：列容差、行容差。文本距离超过容差视为不同列或行。"));

		std::vector<double> params;
		dlg.setValidatorAndParser([&](const CString& edit1, const CString& _2) -> CString
			{
				const int paramsNumber = 2;
				if (!Common::parse(edit1, paramsNumber, [](double v) { return v > 0; }, params))
				{
					return _(L"输入2个不小于0的数，使用空格分隔，分别为：列容差、行容差。文本距离超过容差视为不同列或行。");
				}
				return GenericPairEditDlg::ValidatorOk;
			});


		if (dlg.DoModal() != IDOK)
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		FileDialog::FileDialogFilterBuilder fileFilterBuilter;
		CString strFileFilter = fileFilterBuilter.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
		CString strFilePath = FileDialog::ShowSaveFileDialog(_(L"保存 CSV 文件到"), _(L"数据文件.csv"), L"csv", strFileFilter);
		if (strFilePath.IsEmpty())
		{
			acutPrintf(_(L"取消操作"));
			return;
		}
		CsvWriter writer(strFilePath);
		if (!writer.isValid())
		{
			AfxMessageBox(_(L"文件路径打开失败，请检查是否被占用或路径无效"), MB_OK | MB_ICONERROR);
			return;
		}

		TextUtil::TextEntityDataList elements;
		UniversalPicker::run(
			&TextUtil::textClassList,
			[&](const AcDbObjectId& id)
			{
				TextUtil::TextEntityData data;
				data.id = id;
				if (TextUtil::readMText(id, data.text, false, &data.pos))
				{
					acutPrintf(_(L"\n(%g,%g,%g)多行文本：%s"), data.pos.x, data.pos.y, data.pos.z, data.text.constPtr());
				}
				else if (TextUtil::readDText(id, data.text, false, &data.pos))
				{
					acutPrintf(_(L"\n(%g,%g,%g)单行文本：%s"), data.text.constPtr());
				}
				elements.push_back(data);
			},
			title,
			UniversalPicker::SelectMode::Batch,
			true,
			UniversalPicker::SortMode::None,
			true
		);

		CsvModule::AcStringMatrix matrixData;
		TextUtil::structureTextToAcStringMatrix(elements, params[0], params[1], matrixData);


		for (const auto& row : matrixData)
		{
			writer.writeRow(row);
			acutPrintf(L"\n");
			for (const auto& field : row)
			{
				acutPrintf(L"%s\t", field.constPtr());
			}
		}

		acutPrintf(_(L"\n文件位置：%s"), strFilePath);
	}

	Commands::AutoRegister ar =
	{
		{ L"yxCloneText", []() { return _(L"将多行/单行文本内容复制给其它多行/单行文本"); }, Commands::CommandFlags::Base, cmdCloneText },
		{ L"yxImportCsvToMTextMatrix", []() { return _(L"从 CSV 文件导入数据生成多行文本矩阵"); }, Commands::CommandFlags::PickRedraw, cmdImportCsvToMTextMatrix },
		{ L"yxSpatialTableExplorer", []() { return _(L"将多行/单行文本按空间位置表格化导出到 CSV 文件"); }, Commands::CommandFlags::PickRedraw, cmdSpatialTableExplorer },
	};
}