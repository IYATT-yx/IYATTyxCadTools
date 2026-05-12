/**
 * @file      Common.cpp
 * @brief     通用工具模块的实现文件。
 * @details   本文件实现了 Common 模块中声明的各类辅助工具函数。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "resource.h"
#include <iomanip>

module Common;

namespace Common
{
	void double2AcString(double doubleValue, AcString& AcStringValue, int precision, bool forcePlusSign, bool addSpaceIfZero)
	{
		acdbRToS(doubleValue, AcStringValue, Common::UnitMode::Decimal, precision);

		if (forcePlusSign && doubleValue > Common::Epsilon)
		{
			AcStringValue = L"+" + AcStringValue;
		}
		else if (addSpaceIfZero && std::abs(doubleValue) <= Common::Epsilon)
		{
			AcStringValue = L" " + AcStringValue;
		}
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

	AcDbObjectIdArray getNeighborsAtPoint(const AcGePoint3d& pt, resbuf* pFilter)
	{
		AcDbObjectIdArray neighbors;
		ads_point adsPt;
		adsPt[X] = pt.x;
		adsPt[Y] = pt.y;
		adsPt[Z] = pt.z;

		ads_name ss;
		// 在坐标点处进行 Crossing 检索
		if (acedSSGet(L"C", adsPt, adsPt, pFilter, ss) == RTNORM)
		{
			Adesk::Int32 length = 0;
			acedSSLength(ss, &length);

			for (long i = 0; i < length; ++i)
			{
				ads_name ent;
				acedSSName(ss, i, ent);

				AcDbObjectId objId;
				if (acdbGetObjectId(objId, ent) == Acad::eOk)
				{
					neighbors.append(objId);
				}
			}
			acedSSFree(ss);
		}
		return neighbors;
	}

	std::optional<std::filesystem::path> getAppSubFolder()
	{
		wchar_t* pathTmp = nullptr;

		// 获取 Roaming AppData 基础路径
		HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTmp);
		if (FAILED(hr))
		{
			return std::nullopt;
		}

		try
		{
			std::filesystem::path folderPath(pathTmp);
			CoTaskMemFree(pathTmp); // 释放系统分配内存
			folderPath /= Common::loadString(IDS_PROJNAME).GetString();

			// 递归创建目录（如果不存在）
			std::error_code ec;
			if (!std::filesystem::exists(folderPath))
			{
				if (!std::filesystem::create_directories(folderPath, ec))
				{
					return std::nullopt;
				}
			}

			return folderPath;
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

	Adesk::UInt16 getEntityActualColorIndex(const AcDbEntity* pEntity)
	{
		// 默认回退颜色为 7 (AutoCAD 默认黑白)
		Adesk::UInt16 colorIndex = 7;

		if (pEntity == nullptr)
		{
			return colorIndex;
		}

		AcCmColor cmColor = pEntity->entityColor();
		if (cmColor.isByLayer())
		{
			AcDbLayerTableRecord* pLayer = Common::getObject<AcDbLayerTableRecord>(pEntity->layerId(), AcDb::kForRead);
			if (pLayer != nullptr)
			{
				colorIndex = pLayer->color().colorIndex();
			}
		}
		else if (cmColor.isByBlock())
		{
		}
		else
		{
			colorIndex = cmColor.colorIndex();
		}

		if (colorIndex == 0 || colorIndex >= 256)
		{
			colorIndex = 7;
		}

		return colorIndex;
	}
}
