module;
#include "StdAfx.h"
#include "resource.h"
#include <iomanip>

module Common;

namespace Common
{
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision)
	{
		//AcStringValue.format(L"%.*g", precision, doubleValue);
		acdbRToS(doubleValue, AcStringValue, Common::UnitMode::Decimal, precision);
	}

	bool setCharMapFontToGDT()
	{
		HKEY key = nullptr;

		LSTATUS status = RegOpenKeyExW(
			HKEY_CURRENT_USER,
			Common::CharMap::path,
			0,
			KEY_SET_VALUE,
			&key
		);

		if (status != ERROR_SUCCESS)
		{
			status = RegCreateKeyExW(
				HKEY_CURRENT_USER,
				Common::CharMap::path,
				0,
				nullptr,
				REG_OPTION_NON_VOLATILE,
				KEY_SET_VALUE,
				nullptr,
				&key,
				nullptr
			);
		}

		if (status == ERROR_SUCCESS)
		{
			DWORD valueSize = static_cast<DWORD>((wcslen(Common::CharMap::font) + 1) * sizeof(WCHAR));
			status = RegSetValueExW(
				key,
				Common::CharMap::key,
				0,
				REG_SZ,
				reinterpret_cast<const BYTE*>(Common::CharMap::font),
				valueSize
			);
			RegCloseKey(key);
		}
		return (status == ERROR_SUCCESS);
	}

	void startCharMapWithGDT()
	{
		CAcModuleResourceOverride resOverride;
		if (Common::setCharMapFontToGDT())
		{
			HINSTANCE inst = ShellExecuteW(
				nullptr,
				L"open",
				Common::CharMap::programName,
				nullptr,
				nullptr,
				SW_SHOWNORMAL
			);

			if ((INT_PTR)inst <= 32)
			{
				AfxMessageBox(Common::loadString(IDS_ERR_StartCharMapFailed), MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			AfxMessageBox(Common::loadString(IDS_ERR_SetCharMapFontGDTFailed), MB_OK | MB_ICONERROR);
		}
	}

	AcString wrapWithGdtFont(const wchar_t* input)
	{
		AcString result = L"";
		if (input == nullptr)
		{
			return result;
		}

		int len = (int)wcslen(input);
		for (int i = 0; i < len; i++)
		{
			AcString temp;
			// 拼接 AutoCAD MText 格式：{\FGDT;字符}
			// 注意：\\ 是为了转义反斜杠
			temp.format(L"{\\Famgdt;%c}", input[i]);
			result += temp;
		}

		return result;
	}

	void printClassHierarchy(const AcDbObjectId& objId)
	{
		CAcModuleResourceOverride resOverride;
		AcDbObject* pObj = Common::getObject<AcDbObject>(objId, AcDb::kForRead);
		if (pObj == nullptr)
		{
			return;
		}

		// 获取当前对象的类描述
		AcRxClass* pClass = pObj->isA();
		if (pClass == nullptr)
		{
			return;
		}

		// 存储继承链中的类名指针
		AcArray<const wchar_t*> hierarchy;
		AcRxClass* pTempClass = pClass;

		// 向上迭代获取所有父类名
		while (pTempClass != nullptr)
		{
			// AcRxClass::name() 在 Unicode 版 ARX 中返回 const wchar_t*
			hierarchy.append(pTempClass->name());
			pTempClass = pTempClass->myParent();
		}

		acutPrintf(Common::loadString(IDS_MSG_HierarchyHeader));

		// 从最高层级向当前层级打印
		for (int i = hierarchy.length() - 1; i >= 0; --i)
		{
			// 计算缩进量
			int indent = (hierarchy.length() - 1 - i) * 2;

			// 打印缩进和类名
			acutPrintf(L"\n%*s%s", indent, L"", hierarchy[i]);

			// 标记对象自身的最终类名
			if (i == 0)
			{
				acutPrintf(Common::loadString(IDS_MSG_CurrentClassNameMark));
			}
		}

		acutPrintf(L"\n----------------------------\n");
	}

	CString loadString(UINT nID)
	{
		CString tmp;
		tmp.LoadStringW(nID);
		return tmp;
	}

	bool getEntityCenter(AcDbEntity* pEnt, AcGePoint3d* pCenter)
	{
		if (pEnt == nullptr || pCenter == nullptr)
		{
			return false;
		}

		AcDbExtents extents;
		if (pEnt->getGeomExtents(extents) == Acad::eOk)
		{
			AcGePoint3d minPt = extents.minPoint();
			AcGePoint3d maxPt = extents.maxPoint();

			pCenter->set(
				(minPt.x + maxPt.x) * 0.5,
				(minPt.y + maxPt.y) * 0.5,
				(minPt.z + maxPt.z) * 0.5
			);
			return true;
		}

		return false;
	}

	double getTEXTSIZE()
	{
		resbuf rb;
		if (acedGetVar(L"TEXTSIZE", &rb) == RTNORM)
		{
			return rb.resval.rreal;
		}
		return -1;
	}

	CString getCurrPath(bool bDirectory)
	{
		CString result = L"";
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		if (pDb == nullptr)
		{
			return result;
		}

		const wchar_t* pszPath = nullptr;
		pDb->getFilename(pszPath);
		if (pszPath == nullptr)
		{
			return result;
		}

		result = pszPath;
		if (bDirectory)
		{
			int nIndex = result.ReverseFind(L'\\');
			if (nIndex != -1)
			{
				result = result.Left(nIndex + 1);
			}
		}
		return result;
	}

	CString getTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		std::time_t nowC = std::chrono::system_clock::to_time_t(now);
		std::tm nowTm = *std::localtime(&nowC);
		std::wostringstream woss;
		woss << std::put_time(&nowTm, L"%Y%m%d_%H%M%S");
		return woss.str().c_str();
	}
}
