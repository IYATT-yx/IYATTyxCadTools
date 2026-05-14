/**
 * @file      Interface.ixx
 * @brief     接口模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
export module Interface;

export class Interface
{
public:
	static void init();
	static void unload();
private:
	static void cmdSetByLayer();
	static void cmdYx();
	static void info();
	static void cmdDimensionSolidify();
	static void cmdDimensionRelink();
	static void cmdAddSurroundingCharsForDimension();
	static void cmdRemoveSurroundingCharsForDimension();
	static void cmdSetBasicBox();
	static void cmdUnsetBasicBox();
	static void cmdSetRefDim();
	static void cmdUnsetRefDim();
	static void cmdInsertBalloonNumberBlockWithStartNumber();
	static void cmdPrintClassHierarchy();
	static void test();
	static void cmdExtractAnnotations();
	static void cmdUpdateBalloonNumberBlock();
	static void cmdImeAutoSwitch();
	static void cmdUnloadApp();
	static void cmdCloneText();
	static void cmdIntersect();
	static void cmdBalloonNumberOffset();
	static void cmdBalloonNumberFilter();
	static void cmdImportCsvToMTextMatrix();
	static void cmdSpatialTableExplorer();
	static void cmdCheckBalloonNumberMaxMin();
	static void cmdPasteClipImage();
	static void cmdLocateDrawing();
	static void cmdCheckDuplicateBalloonNumbers();
	static void cmdCheckBalloonNumberBreakpoints();
	static void cmdForceRemoveImage();
	static void cmdLocateSelf();
	static void cmdChainSelection();
	static void cmdDimensionTolerancePrecision();
	static void cmdCreateIntersectionPoints();
	static void cmdPrintConfigFilename();
	static void cmdDialogMiddleClickToOk();
	static void cmdCmdMiddleClickToEnter();
};

