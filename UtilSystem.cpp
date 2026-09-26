/**
 * @file      UtilSystem.cpp
 * @brief     系统集成模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"
#include "resource.h"

module UtilSystem;
import std;
import UtilString;
import UtilConstants;
import UtilGeom;
import FrameworkTranslator;
import FrameworkTranslator;

namespace UtilSystem
{
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
			folderPath /= UtilString::loadString(IDS_PROJNAME).GetString();

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

	CString getTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		std::time_t nowC = std::chrono::system_clock::to_time_t(now);
		std::tm nowTm = *std::localtime(&nowC);
		std::wostringstream woss;
		woss << std::put_time(&nowTm, L"%Y%m%d_%H%M%S");
		return woss.str().c_str();
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

	bool setCharMapFontToGDT()
	{
		HKEY key = nullptr;

		LSTATUS status = RegOpenKeyExW(
			HKEY_CURRENT_USER,
			UtilConstants::CharMap::path,
			0,
			KEY_SET_VALUE,
			&key
		);

		if (status != ERROR_SUCCESS)
		{
			status = RegCreateKeyExW(
				HKEY_CURRENT_USER,
				UtilConstants::CharMap::path,
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
			DWORD valueSize = static_cast<DWORD>((wcslen(UtilConstants::CharMap::font) + 1) * sizeof(WCHAR));
			status = RegSetValueExW(
				key,
				UtilConstants::CharMap::key,
				0,
				REG_SZ,
				reinterpret_cast<const BYTE*>(UtilConstants::CharMap::font),
				valueSize
			);
			RegCloseKey(key);
		}
		return (status == ERROR_SUCCESS);
	}

	void startCharMapWithGDT()
	{
		CAcModuleResourceOverride resOverride;
		if (UtilSystem::setCharMapFontToGDT())
		{
			HINSTANCE inst = ShellExecuteW(
				nullptr,
				L"open",
				UtilConstants::CharMap::programName,
				nullptr,
				nullptr,
				SW_SHOWNORMAL
			);

			if ((INT_PTR)inst <= 32)
			{
				AfxMessageBox(_(L"启动字符映射表失败"), MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			AfxMessageBox(_(L"设置字符映射表字体为 GDT 失败"), MB_OK | MB_ICONERROR);
		}
	}
}