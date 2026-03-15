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
	static void cmdAddSurroundingCharsForDimension();
	static void cmdRemoveSurroundingCharsForDimension();
};

