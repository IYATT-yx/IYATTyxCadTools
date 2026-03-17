#pragma once
class Interface
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
};

