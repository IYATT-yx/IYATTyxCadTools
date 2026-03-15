#include "StdAfx.h"
#include "Common.hpp"

namespace Common
{
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision)
	{
		//AcStringValue.format(L"%.*g", precision, doubleValue);
		acdbRToS(doubleValue, AcStringValue, Common::UnitMode::Decimal, precision);
	}
}