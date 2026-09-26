/**
 * @file      UtilCmdDimension.ixx
 * @brief     尺寸标注相关命令
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "UiGenericPairEditDlg.hpp"

export module UtilCmdDimension;
import FrameworkCommands;
import UiUniversalPicker;
import UtilConstants;
import UtilDimension;
import FrameworkTranslator;
import UtilIoCsvModule;
import UiFileDialog;
import UtilGeometricTolerance;
import UtilText;
import UtilString;

namespace
{
	void cmdDimensionSolidify()
	{
		UiUniversalPicker::run(&UtilConstants::DimensionSubClasses, UtilDimension::dimensionSolidify, _(L"尺寸固化"));
	}

	void cmdDimensionRelink()
	{
		UiUniversalPicker::run(&UtilConstants::DimensionSubClasses, UtilDimension::dimensionRelink, _(L"尺寸恢复关联"));
	}

	void cmdDimensionTolerancePrecision()
	{
		CAcModuleResourceOverride resOverride;
		UiGenericPairEditDlg dlg(_(L"设置尺寸标注的主单位精度和公差精度"), _(L"主单位精度"), _(L"公差精度"), false, true, true);
		// 设置 -1 表示不修改精度 
		CString strDimPrec = L"-1";
		CString strTolPrec = L"-1";
		dlg.modifyEditControl(strDimPrec, strTolPrec);

		int iDimPrec = -1;
		int iTolPrec = -1;
		dlg.setValidatorAndParser([&](const CString& val1, const CString& val2) -> CString
			{
				try
				{
					size_t pos;
					iDimPrec = std::stoi(val1.GetString(), &pos);
					if (pos != val1.GetLength())
					{
						throw std::exception();
					}
					if (iDimPrec < 0 && iDimPrec != -1)
					{
						throw std::exception();
					}
					if (iDimPrec > 8)
					{
						throw std::exception();
					}

					iTolPrec = std::stoi(val2.GetString(), &pos);
					if (pos != val2.GetLength())
					{
						throw std::exception();
					}
					if (iTolPrec < 0 && iTolPrec != -1)
					{
						throw std::exception();
					}
					if (iTolPrec > 8)
					{
						throw std::exception();
					}
				}
				catch (...)
				{
					return _(L"精度值只能是 0 至 8 的整数，输入 -1 时不修改精度。");
				}
				return UiGenericPairEditDlg::ValidatorOk;
			});

		if (dlg.DoModal() != IDOK)
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		UiUniversalPicker::run(
			&UtilConstants::DimensionSubClasses,
			[&](const AcDbObjectId& id)
			{
				UtilDimension::setDimensionTolerancePreccision(id, iDimPrec, iTolPrec);
			},
			_(L"设置尺寸标注的主单位精度和公差精度"),
			UiUniversalPicker::SelectMode::Immediate,
			false,
			UiUniversalPicker::SortMode::None,
			true
		);
	}

	void cmdAddSurroundingCharsForDimension()
	{
		CAcModuleResourceOverride resOverride;
		UiGenericPairEditDlg dlg(_(L"为标注添加前后缀"), _(L"前缀符号"), _(L"后缀符号"));

		CString left, right;
		dlg.setValidatorAndParser([&](const CString& value1, const CString& value2) -> CString
			{
				left = value1;
				right = value2;
				return UiGenericPairEditDlg::ValidatorOk;
			});
		if (dlg.DoModal() != IDOK)
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		bool isLGdt = dlg.getGdtCheckStatus(0);
		bool isRGdt = dlg.getGdtCheckStatus(1);
		UiUniversalPicker::run(
			&UtilConstants::DimensionSubClasses,
			[&](AcDbObjectId objId)
			{
				UtilDimension::addSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
			},
			_(L"为标注添加前后缀"),
			UiUniversalPicker::SelectMode::Immediate
		);
	}

	void cmdRemoveSurroundingCharsForDimension()
	{
		CAcModuleResourceOverride resOverride;
		UiGenericPairEditDlg dlg(_(L"为标注移除前后缀"), _(L"前缀符号"), _(L"后缀符号"));

		CString left, right;
		dlg.setValidatorAndParser([&](const CString& value1, const CString& value2) -> CString
			{
				left = value1;
				right = value2;
				return UiGenericPairEditDlg::ValidatorOk;
			});
		if (dlg.DoModal() != IDOK)
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		bool isLGdt = dlg.getGdtCheckStatus(0);
		bool isRGdt = dlg.getGdtCheckStatus(1);
		UiUniversalPicker::run(
			&UtilConstants::DimensionSubClasses,
			[&](AcDbObjectId objId)
			{
				UtilDimension::removeSurroundingCharsForDimension(objId, left, right, isLGdt, isRGdt);
			},
			_(L"为标注移除前后缀"),
			UiUniversalPicker::SelectMode::Immediate
		);
	}

	void cmdSetBasicBox()
	{
		UiUniversalPicker::run(
			&UtilConstants::DimensionSubClasses,
			[](AcDbObjectId objId)
			{
				UtilDimension::setAndUnsetBasicBox(objId, true);
			},
			_(L"设置理论尺寸框"),
			UiUniversalPicker::SelectMode::Immediate
		);
	}

	void cmdUnsetBasicBox()
	{
		UiUniversalPicker::run(
			&UtilConstants::DimensionSubClasses,
			[](AcDbObjectId objId)
			{
				UtilDimension::setAndUnsetBasicBox(objId, false);
			},
			_(L"取消理论尺寸框"),
			UiUniversalPicker::SelectMode::Immediate
		);
	}

	void cmdSetRefDim()
	{
		UiUniversalPicker::run(
			&UtilConstants::DimensionSubClasses,
			[](AcDbObjectId objId)
			{
				UtilDimension::setAndUnsetRefDim(objId, true);
			},
			_(L"设置参考尺寸括号"),
			UiUniversalPicker::SelectMode::Immediate
		);
	}

	void cmdUnsetRefDim()
	{
		UiUniversalPicker::run(
			&UtilConstants::DimensionSubClasses,
			[](AcDbObjectId objId)
			{
				UtilDimension::setAndUnsetRefDim(objId, false);
			},
			_(L"取消理论尺寸括号"),
			UiUniversalPicker::SelectMode::Immediate
		);
	}

	void cmdExtractAnnotations()
	{
		UiFileDialog::FileDialogFilterBuilder fileFilterBuilder;
		CString strFileFilter = fileFilterBuilder.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
		CString filePath = UiFileDialog::ShowSaveFileDialog(_(L"保存 CSV 文件到"), _(L"数据文件.csv"), L"csv", strFileFilter);
		if (filePath.IsEmpty())
		{
			acutPrintf(_(L"取消操作"));
			return;
		}

		CsvWriter csv(filePath);
		if (!csv.isValid())
		{
			AfxMessageBox(_(L"文件路径打开失败，请检查是否被占用或路径无效"), MB_OK | MB_ICONERROR);
			return;
		}

		UiUniversalPicker::AcRxClassVector filter = { AcmFCF::desc(), AcDbMText::desc(), AcDbText::desc() };
		filter.insert(filter.end(), UtilConstants::DimensionSubClasses.begin(), UtilConstants::DimensionSubClasses.end());
		UiUniversalPicker::run(
			&filter,
			[&csv](AcDbObjectId objId)
			{
				UtilDimension::DimensionData dimData{};
				UtilDimension::readDim(objId, dimData);
				UtilGeometricTolerance::GeometricToleranceData gtData{};
				UtilGeometricTolerance::readFcf(objId, gtData);
				AcString asMText, asDText;

				if (dimData.status) // 尺寸
				{
					// 名义值
					AcString asMeasuredValue;
					double dMeasuredValue = dimData.dimensionValue();
				    UtilString::double2AcString(dMeasuredValue, asMeasuredValue, dimData.precision);

					// 公差
					AcString asTol, asTolUpper, asTolLower;
					if (dimData.upperDeviation == 0 && dimData.lowerDeviation == 0) // 无公差
					{
						asTol = L"";
						asTolUpper = L"0";
						asTolLower = L"0";
					}
					else
					{
						if (abs(dimData.upperDeviation + dimData.lowerDeviation) < 1e-6) // 对称偏差
						{
							double dAbsTol = abs(dimData.upperDeviation);
							asTol.format(L"%s%.*f", UtilConstants::SymbolCodes::PlusMinus, dimData.tolerancePrecision, dAbsTol);
							asTolUpper.format(L"%.*f", dimData.tolerancePrecision, dAbsTol);
							asTolLower.format(L"-%.*f", dimData.tolerancePrecision, dAbsTol);
						}
						else // 极限偏差
						{

							UtilString::double2AcString(dimData.upperDeviation, asTolUpper, dimData.tolerancePrecision);
							UtilString::double2AcString(dimData.lowerDeviation, asTolLower, dimData.tolerancePrecision);
							asTol.format(L"+%s/%s", asTolUpper.constPtr(), asTolLower.constPtr());
							asTolUpper.format(L"%.*f", dimData.tolerancePrecision, dimData.upperDeviation);
							asTolLower.format(L"%.*f", dimData.tolerancePrecision, dimData.lowerDeviation);
						}
					}

					AcString asDimText = dimData.prefix + asMeasuredValue + asTol + dimData.suffix;
					UtilText::resolveControlCodes(asDimText);
					acutPrintf(_(L"\n尺寸：%s"), asDimText.constPtr());
					std::vector<AcString> row = { dimData.text, asDimText, asMeasuredValue, asTolUpper, asTolLower }; // 完整尺寸内容、尺寸文本、名义值、上极限偏差、下极限偏差
					csv.writeRow(row);
				}
				else if (gtData.status) // 几何公差
				{
					for (int i = 0; i < UtilGeometricTolerance::GeometricToleranceDataLen; ++i)
					{
						UtilGeometricTolerance::GeometricToleranceRow row = gtData.rows[i];
						if (row.gdtSymbolType != Acm::kNoType)
						{
							AcString name = row.name;
							AcString value = row.value;
							AcString primary = row.primary;
							AcString secondary = row.secondary;
							AcString tertiary = row.tertiary;
							AcString strRow = row.toString();
							acutPrintf(_(L"\n几何公差：%s"), strRow.constPtr());
							std::vector<AcString> asvRow = { strRow, name, value, primary, secondary, tertiary };
							csv.writeRow(asvRow);
						}
						else
						{
							break;
						}
					}
				}
				else if (UtilText::readMText(objId, asMText))
				{
					acutPrintf(_(L"\n多行文本：%s"), asMText.constPtr());
					std::vector<AcString> rows = { asMText };
					csv.writeRow(rows);
				}
				else if (UtilText::readDText(objId, asDText))
				{
					acutPrintf(_(L"\n单行文本：%s"), asDText.constPtr());
					std::vector<AcString> rows = { asDText };
					csv.writeRow(rows);
				}
			},
			_(L"提取标注到CSV文件"),
			UiUniversalPicker::SelectMode::Immediate,
			false,
			UiUniversalPicker::SortMode::RD,
			false
		);
	}

	FrameworkCommands::AutoRegister ar =
	{
		{ L"yxDimensionSolidify", []() { return  _(L"尺寸固化"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdDimensionSolidify },
		{ L"yxDimensionReslink", []() { return  _(L"尺寸恢复关联"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdDimensionRelink },
		{ L"yxDimensionTolerancePrecision", []() { return _(L"设置尺寸标注的主单位精度和公差精度"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdDimensionTolerancePrecision },
		{ L"yxAddSurroundingCharsForDimension", []() { return _(L"为标注添加前后缀"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdAddSurroundingCharsForDimension },
		{ L"yxRemoveSurroundingCharsForDimension", []() { return _(L"为标注移除前后缀"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdRemoveSurroundingCharsForDimension },
		{ L"yxSetBasicBox", []() { return _(L"设置理论尺寸框"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdSetBasicBox },
		{ L"yxUnsetBasicBox", []() { return _(L"取消理论尺寸框"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdUnsetBasicBox },
		{ L"yxSetRefDim", []() { return _(L"设置参考尺寸括号"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdSetRefDim },
		{ L"yxUnsetRefDim", []() { return _(L"取消参考尺寸括号"); }, FrameworkCommands::CommandFlags::PickRedraw, cmdUnsetRefDim },
		{ L"yxExtractAnnotations", []() { return _(L"提取标注到CSV文件"); }, FrameworkCommands::CommandFlags::Base, cmdExtractAnnotations },
	};
}