/**
 * @file      UiGenericPairEditDlg.hpp
 * @brief     通用双编辑对话框
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
#pragma once
//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"
import std;

//-----------------------------------------------------------------------------
class UiGenericPairEditDlg : public CAcUiDialog {
	DECLARE_DYNAMIC (UiGenericPairEditDlg)

public:
	UiGenericPairEditDlg (CString title = L"通用双编辑对话框", CString label1 = L"编辑框1：", CString label2 = L"编辑框2：", bool singleMode = false, bool disableGdt = false, bool trim = false, CWnd* pParent = NULL, HINSTANCE hInstance = NULL);

	enum { IDD = IDD_GENERICPAIREDITDLG} ;

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CString title;
	CString label1;
    CString label2;
	bool singleMode;
	bool disableGdt;
	bool bTrim;

	CStatic staticText1; // 标签1变量
	CStatic staticText2; // 标签2变量
	CEdit editControl1; // 编辑框1变量
	CEdit editControl2; // 编辑框2变量
	CString edit1Result; // 编辑框1传出结果
    CString edit2Result; // 编辑框2传出结果
	CString csEdit1Input = ""; // 编辑框1传入结果
	CString csEdit2Input = ""; // 编辑框2传入结果

	CFont fontNormal; // 常规字体
	CFont fontGDT; // AutoCAD GDT 字体
	bool GdtCheckedStatus[2] = { false }; // GDT 复选框状态

public:
	using Validator = std::function < CString(const CString&, const CString&)>; // 验证函数类型
	static const CString ValidatorOk;
private:
	UiGenericPairEditDlg::Validator  validator = nullptr;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();

	/**
	 * @brief 读取编辑框1中的空格间隔的浮点数值列表
	 * @return 浮点数值列表。建议使用引用的方式取值，如 std::vector<double>& values = <ObjectName>.getEditResult();
	 */
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	/**
	 * @brief 获取 GDT 复选框状态
	 * @param idx 0 为第 1 行复选框；1 为第 2 行复选框
	 * @return true 选中；false 未选中
	 */
	bool getGdtCheckStatus(const int& idx);
	/**
	 * @brief 修改编辑框的值
	 * @param edit1Value 编辑框1的值
	 * @param edit2Value 编辑框2的值
	 */
	void modifyEditControl(const CString& edit1Value = L"", const CString& edit2Value = L"");

	/**
	 * @brief 设置验证函数，同时传出结果
	 * @param validator 验证函数
	 */
	void setValidatorAndParser(UiGenericPairEditDlg::Validator validator);
} ;
