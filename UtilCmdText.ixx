/**
 * @file      UtilCmdText.ixx
 * @brief     文本命令
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "UiGenericPairEditDlg.hpp"

export module UtilCmdText;
import FrameworkTranslator;
import UiUniversalPicker;
import UiFileDialog;
import UtilIoCsvModule;
import UtilAcadVar;
import UtilText;
import FrameworkCommands;
import UtilCommon;
import UtilAnnotative;

namespace
{
	void cmdCloneText()
	{
		AcString asSrcTextContent;
		acutPrintf(_(L"\n请选择要复制的源文本对象"));
		if (!UtilText::getSelectedTextRawContent(asSrcTextContent) || asSrcTextContent.isEmpty())
		{
			acutPrintf(_(L"取消操作"));
			return;
		}
		acutPrintf(_(L"\n读取到：%s\n"), asSrcTextContent.constPtr());

		acutPrintf(_(L"请选择要粘贴到的文本对象"));
		UiUniversalPicker::run(
			&UtilText::textClassList,
			[&asSrcTextContent](const AcDbObjectId& id)
			{
				UtilText::updateTextEntityContent(id, asSrcTextContent);
			},
			_(L"将多行/单行文本内容复制给其它多行/单行文本"),
			UiUniversalPicker::SelectMode::Immediate,
			false,
			UiUniversalPicker::SortMode::None,
			true
		);
	}

	void cmdImportCsvToMTextMatrix()
	{
		UiFileDialog::FileDialogFilterBuilder fileFilterBuilter;
		CString strFileFilter = fileFilterBuilter.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
		CString strFilePath = UiFileDialog::ShowOpenFileDialog(_(L"选择要导入的文件"), L"csv", strFileFilter);
		if (strFilePath.IsEmpty())
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		UtilIoCsvModule::AcStringMatrix matrixData;
		UtilIoCsvModule::readCsvToAcStringMatrix(strFilePath, matrixData);

		CAcModuleResourceOverride resOverride;
		UiGenericPairEditDlg dlg(_(L"从 CSV 文件导入数据生成多行文本矩阵"), _(L"参数"), _(L"使用提示"), false, true, true);
		CString strTipMTextMatrixParameter;
		double TEXTSIZE;
		if (!UtilAcadVar::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
		{
			AfxMessageBox(_(L"获取变量失败！"), MB_OK | MB_ICONERROR);
			return;
		}
		double scale = UtilAnnotative::getCurrentScaleValue();
		strTipMTextMatrixParameter.Format(_(L"输入3个不小于0的数，使用空格分隔，分别为：列宽、列步长、行步长。显示文字高度 = TEXTSIZE变量值%g × 注释比例缩放值%g = %g"), TEXTSIZE, scale, TEXTSIZE * scale);
		dlg.modifyEditControl(L"", strTipMTextMatrixParameter);

		std::vector<double> params;
		dlg.setValidatorAndParser([&](const CString& edit1, const CString& _) -> CString
			{
				const int paramsNumber = 3;
				if (!UtilCommon::parse(edit1, paramsNumber, [](double v) { return v > 0; }, params))
				{
					return strTipMTextMatrixParameter;
				}
				return UiGenericPairEditDlg::ValidatorOk;
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

		UtilText::createMTextMatrix(params[0], params[1], params[2], matrixData, asPnt3d(pt));
	}

	void cmdSpatialTableExplorer()
	{
		CAcModuleResourceOverride resOverride;
		CString title = _(L"将多行/单行文本按空间位置表格化导出到 CSV 文件");
		UiGenericPairEditDlg dlg(title, _(L"参数"), _(L"使用提示"), false, true, true);

		// 默认列容差和行容差
		// 字高默认使用 TEXTSIZE 变量值，列容差默认按字高的 3 倍，行容差默认按字高的 1 倍（考虑注释比例缩放值）
		CString strInitParameter;
		double scale = UtilAnnotative::getCurrentScaleValue();
		double TEXTSIZE;
		if (!UtilAcadVar::getVar(AcadVarName::TEXTSIZE, TEXTSIZE))
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
				if (!UtilCommon::parse(edit1, paramsNumber, [](double v) { return v > 0; }, params))
				{
					return _(L"输入2个不小于0的数，使用空格分隔，分别为：列容差、行容差。文本距离超过容差视为不同列或行。");
				}
				return UiGenericPairEditDlg::ValidatorOk;
			});


		if (dlg.DoModal() != IDOK)
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		UiFileDialog::FileDialogFilterBuilder fileFilterBuilter;
		CString strFileFilter = fileFilterBuilter.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
		CString strFilePath = UiFileDialog::ShowSaveFileDialog(_(L"保存 CSV 文件到"), _(L"数据文件.csv"), L"csv", strFileFilter);
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

		UtilText::TextEntityDataList elements;
		UiUniversalPicker::run(
			&UtilText::textClassList,
			[&](const AcDbObjectId& id)
			{
				UtilText::TextEntityData data;
				data.id = id;
				if (UtilText::readMText(id, data.text, false, &data.pos))
				{
					acutPrintf(_(L"\n(%g,%g,%g)多行文本：%s"), data.pos.x, data.pos.y, data.pos.z, data.text.constPtr());
				}
				else if (UtilText::readDText(id, data.text, false, &data.pos))
				{
					acutPrintf(_(L"\n(%g,%g,%g)单行文本：%s"), data.text.constPtr());
				}
				elements.push_back(data);
			},
			title,
			UiUniversalPicker::SelectMode::Batch,
			true,
			UiUniversalPicker::SortMode::None,
			true
		);

		UtilIoCsvModule::AcStringMatrix matrixData;
		UtilText::structureTextToAcStringMatrix(elements, params[0], params[1], matrixData);


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

	FrameworkCommands::AutoRegister ar =
	{
		{ L"yxCloneText", []() { return _(L"将多行/单行文本内容复制给其它多行/单行文本"); }, FrameworkCommands::CommandFlags::Base, cmdCloneText },
		{ L"yxImportCsvToMTextMatrix", []() { return _(L"从 CSV 文件导入数据生成多行文本矩阵"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdImportCsvToMTextMatrix },
		{ L"yxSpatialTableExplorer", []() { return _(L"将多行/单行文本按空间位置表格化导出到 CSV 文件"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdSpatialTableExplorer },
	};
}