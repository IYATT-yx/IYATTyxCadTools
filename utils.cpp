/**
 * @file      utils.cpp
 * @brief     工具
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "Resource.h"
#include "MainBar.hpp"

module utils;
import std;
import  UniversalPicker;
import Commands;
import Translator;
import Common;
import CsvModule;
import FileDialog;
import Dimension;
import TextUtil;
import GeometricTolerance;
import ConfigManager;
import DocCloseInterceptor;
import Interface;
import ActivityInsightsManager;

// 注册
namespace
{
	void cmdChainSelection()
	{
		UniversalPicker::AcRxClassVector arcv = { AcDbArc::desc(), AcDbPolyline::desc(), AcDbLine::desc(), AcDbSpline::desc() };
		resbuf* pFilter = UniversalPicker::buildFilter(&arcv);

		UniversalPicker::run(
			&arcv,
			[&](const AcDbObjectId& id, bool& bBreak)
			{
				// 局部容器：确保每次点击都是独立的搜索过程
				std::queue<AcDbObjectId> waitingQueue;
				std::set<AcDbObjectId> processedIds;
				AcDbObjectIdArray resultIds;

				waitingQueue.push(id);
				processedIds.insert(id);

				// 广度优先搜索 (BFS)
				while (!waitingQueue.empty())
				{
					AcDbObjectId currentId = waitingQueue.front();
					waitingQueue.pop();
					resultIds.append(currentId);

					AcDbCurve* pCurve = Common::getObject<AcDbCurve>(currentId, AcDb::kForRead);
					if (pCurve == nullptr)
					{
						continue;
					}

					AcGePoint3d startPt, endPt;
					if (pCurve->getStartPoint(startPt) == Acad::eOk && pCurve->getEndPoint(endPt) == Acad::eOk)
					{
						AcGePoint3d checkPts[2] = { startPt, endPt };

						for (const auto& pt : checkPts)
						{
							AcDbObjectIdArray neighbors = Common::getNeighborsAtPoint(pt, pFilter);

							for (int i = 0; i < neighbors.length(); ++i)
							{
								AcDbObjectId nId = neighbors[i];

								// 严格去重：
								// 1. 跳过当前正在处理的实体本身 (nId != currentId)
								// 2. 跳过已经进入过队列或处理过的实体 (processedIds.find == end)
								if (nId != currentId && processedIds.find(nId) == processedIds.end())
								{
									processedIds.insert(nId);
									waitingQueue.push(nId);
								}
							}
						}
					}
				}

				if (resultIds.length() > 0)
				{
					UniversalPicker::setSelection(resultIds);
					acutPrintf(_(L"\n自动链式选择成功：共选中 %d 条线条实体。"), resultIds.length());
					bBreak = true;
				}
			},
			_(L"选中实体后自动链式选择"),
			UniversalPicker::SelectMode::Immediate,
			true
		);

		UniversalPicker::freeFilter(pFilter);
	}

    void cmdExtractAnnotations()
    {
        FileDialog::FileDialogFilterBuilder fileFilterBuilder;
        CString strFileFilter = fileFilterBuilder.addFilter(_(L"CSV 文件"), { L"*.csv" }).build();
        CString filePath = FileDialog::ShowSaveFileDialog(_(L"保存 CSV 文件到"), _(L"数据文件.csv"), L"csv", strFileFilter);
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

        UniversalPicker::AcRxClassVector filter = { AcmFCF::desc(), AcDbMText::desc(), AcDbText::desc() };
        filter.insert(filter.end(), Common::DimensionSubClasses.begin(), Common::DimensionSubClasses.end());
        UniversalPicker::run(
            &filter,
            [&csv](AcDbObjectId objId)
            {
                Dimension::DimensionData dimData{};
                Dimension::readDim(objId, dimData);
                GeometricTolerance::GeometricToleranceData gtData{};
                GeometricTolerance::readFcf(objId, gtData);
                AcString asMText, asDText;

                if (dimData.status) // 尺寸
                {
                    // 名义值
                    AcString asMeasuredValue;
                    double dMeasuredValue = dimData.dimensionValue();
                    Common::double2AcString(dMeasuredValue, asMeasuredValue, dimData.precision);

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
                            asTol.format(L"%s%.*f", Common::SymbolCodes::PlusMinus, dimData.tolerancePrecision, dAbsTol);
                            asTolUpper.format(L"%.*f", dimData.tolerancePrecision, dAbsTol);
                            asTolLower.format(L"-%.*f", dimData.tolerancePrecision, dAbsTol);
                        }
                        else // 极限偏差
                        {

                            Common::double2AcString(dimData.upperDeviation, asTolUpper, dimData.tolerancePrecision);
                            Common::double2AcString(dimData.lowerDeviation, asTolLower, dimData.tolerancePrecision);
                            asTol.format(L"+%s/%s", asTolUpper.constPtr(), asTolLower.constPtr());
                            asTolUpper.format(L"%.*f", dimData.tolerancePrecision, dimData.upperDeviation);
                            asTolLower.format(L"%.*f", dimData.tolerancePrecision, dimData.lowerDeviation);
                        }
                    }

                    AcString asDimText = dimData.prefix + asMeasuredValue + asTol + dimData.suffix;
                    TextUtil::resolveControlCodes(asDimText);
                    acutPrintf(_(L"\n尺寸：%s"), asDimText.constPtr());
                    std::vector<AcString> row = { dimData.text, asDimText, asMeasuredValue, asTolUpper, asTolLower }; // 完整尺寸内容、尺寸文本、名义值、上极限偏差、下极限偏差
                    csv.writeRow(row);
                }
                else if (gtData.status) // 几何公差
                {
                    for (int i = 0; i < GeometricTolerance::GeometricToleranceDataLen; ++i)
                    {
                        GeometricTolerance::GeometricToleranceRow row = gtData.rows[i];
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
                else if (TextUtil::readMText(objId, asMText))
                {
                    acutPrintf(_(L"\n多行文本：%s"), asMText.constPtr());
                    std::vector<AcString> rows = { asMText };
                    csv.writeRow(rows);
                }
                else if (TextUtil::readDText(objId, asDText))
                {
                    acutPrintf(_(L"\n单行文本：%s"), asDText.constPtr());
                    std::vector<AcString> rows = { asDText };
                    csv.writeRow(rows);
                }
            },
            _(L"提取标注到CSV文件"),
            UniversalPicker::SelectMode::Immediate,
            false,
            UniversalPicker::SortMode::RD,
            false
        );
    }

    void cmdIntersect()
    {
        Commands::CommandList pszCmdList =
        {
            L"FILLET",
            L"M",
            L"R",
            L"0"
        };
        Commands::executeCommand(pszCmdList);
    }

    void cmdLocateDrawing()
    {
        CString drawingPath = Common::getCurrPath();
        if (drawingPath.IsEmpty())
        {
            AfxMessageBox(_(L"图纸未保存"), MB_OK | MB_ICONERROR);
            return;
        }

        FileDialog::locateFileInExplorer(drawingPath);
    }

    void cmdLocateSelf()
    {
        const wchar_t* appName = acedGetAppName();
        FileDialog::locateFileInExplorer(appName);
    }

    void cmdPrintConfigFilename()
    {
        auto& manager = ConfigManager::getInstance();
        std::wstring configFilename = manager.getConfigFilename();
        FileDialog::locateFileInExplorer(configFilename.c_str());
    }

    void cmdPrintClassHierarchy()
    {
        UniversalPicker::run(nullptr, Common::printClassHierarchy, _(L"打印类层次结构"), UniversalPicker::SelectMode::Immediate, true);
    }


    void cmdUnloadApp()
    {
        const wchar_t* appName = acedGetAppName();
        Commands::CommandList pszCmdList =
        {
            L"ARX",
            L"U",
            appName
        };
        Commands::executeCommand(pszCmdList);
    }

    void cmdRestartApp()
    {
        std::wstring lispPath = std::filesystem::path(acedGetAppName()).generic_wstring();
        Interface::unload();
        std::wstring lispCmd = L"(progn (arxunload \"" + lispPath + L"\" nil) (arxload \"" + lispPath + L"\")(princ))";
        Commands::CommandList pszCmdList =
        {
            lispCmd.c_str()
        };
        Commands::executeCommand(pszCmdList, false);
    }

    void test()
    {
        ActivityInsightsManager manager;
        std::vector<CadHistory::ActivityItem> vecRawHistory = manager.fetchHistory(CadHistory::OpFilter::OpAll);

        if (vecRawHistory.empty())
        {
            acutPrintf(L"\n[IYATT-yx] 未检测到任何有效的文件活动日志。\n");
            return;
        }

        // 同路径去重处理
        std::vector<CadHistory::ActivityItem> vecUniqueHistory;
        std::set<std::wstring> setProcessedPaths;

        for (const auto& item : vecRawHistory)
        {
            std::wstring wstrUpperPath = item.wstrDstPath;
            std::transform(wstrUpperPath.begin(), wstrUpperPath.end(), wstrUpperPath.begin(), ::towupper);

            if (setProcessedPaths.find(wstrUpperPath) == setProcessedPaths.end())
            {
                vecUniqueHistory.push_back(item);
                setProcessedPaths.insert(wstrUpperPath);
            }
        }

        acutPrintf(L"\n[IYATT-yx] ===== 开始打印图纸活动日志 =====");

        for (const auto& item : vecUniqueHistory)
        {
            // 直接调用结构体封装的方法获取时间字符串！干净纯粹！
            std::wstring wstrDisplayTime = item.toTimeString();

            acutPrintf(
                L"\n[%s] 动作: %-12s | 产品: %-25s \n  路径: %s",
                wstrDisplayTime.c_str(),
                item.wstrOp.c_str(),
                item.wstrProduct.c_str(),
                item.wstrDstPath.c_str()
            );
        }

        acutPrintf(L"\n[IYATT-yx] ===== 图纸日志打印结束 (共 %d 项) =====\n", vecUniqueHistory.size());
    }

    void yx()
    {
        Interface::cmdYx();
    }

	Commands::AutoRegister ar =
	{
		{ L"yxChainSelection", []() { return _(L"选中实体后自动链式选择"); }, Commands::CommandFlags::PickRedraw, cmdChainSelection },
		{ L"yxExtractAnnotations", []() { return _(L"提取标注到CSV文件"); }, Commands::CommandFlags::Base, cmdExtractAnnotations },
        { L"yxIntersect", []() { return _(L"将两条直线延伸或修剪至其交点"); }, Commands::CommandFlags::Base, cmdIntersect },
        { L"yxLocateDrawing", []() { return _(L"打开图纸路径"); }, Commands::CommandFlags::Base, cmdLocateDrawing },
        { L"yxLocateSelf", []() { return _(L"打开本工具路径"); }, Commands::CommandFlags::Base, cmdLocateSelf },
        { L"yxPrintConfigFilename", []() { return _(L"打开配置路径"); }, Commands::CommandFlags::Base, cmdPrintConfigFilename },
        { L"yxPrintClassHierarchy", []() { return _(L"打印类层次结构"); }, Commands::CommandFlags::Base, cmdPrintClassHierarchy },
        { L"yxRestart", []() { return _(L"重启本插件"); }, Commands::CommandFlags::Base, cmdRestartApp },
        { L"yxUnload", []() { return _(L"关闭本插件"); }, Commands::CommandFlags::Base, cmdUnloadApp },
        { L"yxTest", []() { return _(L"测试"); }, Commands::CommandFlags::Base, test },
        { L"yx", []() { return _(L"显示或隐藏命令菜单"); }, Commands::CommandFlags::Base, yx }
	};
}