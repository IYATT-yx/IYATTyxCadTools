module;
#include "StdAfx.h"

export module Interface;

export class Interface
{
public:
	static void init();
	static void unload();
private:
	static void cmdSetByLayer();
	static void cmdHelp();
	static void cmdDimensionFix();
	static void cmdDimensionResume();
	static void cmdiAddSurroundingCharsForDimension();
	static void cmdiRemoveSurroundingCharsForDimension();
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
};

