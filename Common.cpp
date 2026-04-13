module;
#include "StdAfx.h"

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
				AfxMessageBox(L"启动字符映射表失败", MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			AfxMessageBox(L"设置字符映射表字体为 GDT 失败", MB_OK | MB_ICONERROR);
		}
	}

	AcString wrapWithGdtFont(const ACHAR* input)
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

	void printClassHierarchy(AcDbObjectId objId)
	{
		if (objId.isNull())
		{
			acutPrintf(L"\n错误：无效的 ObjectId。");
			return;
		}

		AcDbObjectPointer<AcDbObject> pObj(objId, AcDb::kForRead);
		if (pObj.openStatus() != Acad::eOk)
		{
			acutPrintf(L"\n错误：无法打开对象进行读取。");
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

		acutPrintf(L"\n--- 继承链 ---");

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
				acutPrintf(L"  <-- [当前类名]");
			}
		}

		acutPrintf(L"\n----------------------------\n");
	}

	CString ShowSaveFileDialog(const CString& title, const CString& defaultName, const CString& defExt, const CString& filter)
	{
		CFileDialog dlg(FALSE, defExt, defaultName, OFN_HIDEREADONLY, filter, AfxGetMainWnd());
		dlg.m_ofn.lpstrTitle = title;

		if (dlg.DoModal() == IDOK)
		{
			return dlg.GetPathName();
		}

		return L"";
	}

	CString loadString(UINT nID)
	{
		CString tmp;
		tmp.LoadStringW(nID);
		return tmp;
	}
}
