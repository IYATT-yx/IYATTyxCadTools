module;
#include "stdafx.h"
#include <atlimage.h>

module Image;

namespace Image
{
	bool clipboardHasImage()
	{
		bool result = false;
		if (OpenClipboard(adsw_acadMainWnd()))
		{
			result = IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_BITMAP);
			CloseClipboard();
		}
		return result;
	}

    bool saveClipboardBitmapToFile(CString& filename)
    {
        CImage image;
        bool success = false;
        if (OpenClipboard(adsw_acadMainWnd()))
        {
            HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
            if (hBitmap)
            {
                image.Attach(hBitmap);
                HRESULT hr = image.Save(filename);
                image.Detach();
                success = SUCCEEDED(hr);
            }
            CloseClipboard();
        }
        return success;
    }

    bool copyFileToClipboard(const CString& filename)
    {
        if (!OpenClipboard(adsw_acadMainWnd()))
        {
            return false;
        }

        EmptyClipboard();

        // 构造 DROPFILES 结构体
        // 该结构体后面紧跟着文件路径字符串列表，以双 NULL 结尾
        size_t size = sizeof(DROPFILES) + (filename.GetLength() + 2) * sizeof(TCHAR);
        HGLOBAL hGlobal = GlobalAlloc(GHND, size);
        if (!hGlobal)
        {
            CloseClipboard();
            return false;
        }

        DROPFILES* pDropFiles = (DROPFILES*)GlobalLock(hGlobal);
        if (pDropFiles)
        {
            pDropFiles->pFiles = sizeof(DROPFILES);
            pDropFiles->fWide = TRUE; // 使用宽字符

            // 拷贝路径
            wchar_t* pData = (TCHAR*)((BYTE*)pDropFiles + sizeof(DROPFILES));
            lstrcpy(pData, filename);
            // 确保以两个 NULL 结尾（GHND 已经初始化为 0，但手动处理更安全）
            pData[filename.GetLength() + 1] = L'\0';

            GlobalUnlock(hGlobal);

            // 设置剪贴板数据格式为 CF_HDROP
            if (SetClipboardData(CF_HDROP, hGlobal))
            {
                CloseClipboard();
                return true;
            }
        }

        GlobalFree(hGlobal);
        CloseClipboard();
        return false;
    }
};