
// test2Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

typedef struct MSG_LEN	
{
	char* msg;
	int len;
}MSG_LEN;
typedef struct TEST_ITEM
{
	CString TestName;//测试名称
	CString SndCmd;//发出cmd
	CString CheckCmd;//接收cmd
	int Status;//-1 Just Go	0:Status	1:Hex	2:Ascii
	int WaitTime;//超时时间
	int StillTime;//接收后暂停

}TEST_ITEM;
typedef struct ITEM_SHOW
{
	int ItemPos;//条目位置
	CString StrSend;//实际发送Cmd
	CString StrReceive;//实际接收cmd
	CString ReturnValue;//返回数据
	CString Result;//结果
}ITEM_SHOW;
CString ConvertCharHex(CString a);
// Ctest2Dlg 对话框
class Ctest2Dlg : public CDialogEx
{
// 构造
public:
	Ctest2Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TEST2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonReadFile();
	CComboBox m_serial_port;
	CComboBox m_baud_rate;
	afx_msg void OnBnClickedButtonOpenCloseSerial();
	CButton m_open_close_serial;
	CListCtrl m_list_test_item;
	CListBox m_msg_list;
	afx_msg void OnBnClickedButtonClear();
	int ReadFile(CStdioFile);
	int AddTestItem(CStdioFile ItemFile);
	static UINT ThreadReceive(void *param);
	static UINT ThreadSend(void *param);
	afx_msg void OnBnClickedButtonStart();
	CButton m_start;
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	static MSG_LEN HandleSndMsg(CString data, int len);

	//static CString ConvertHexChar(BYTE*strByte ,int len);
	afx_msg void OnBnClickedButtonPause();
	CEdit m_msg_send;
	CButton m_btn_send;
	CButton m_btn_readfile;
	CStatic m_count_send;
	CStatic m_count_pass;
	CStatic m_count_faild;
	CStatic m_count_filename;
	afx_msg void OnEnSetfocusEditMsg();
	CButton m_btn_pause;
	CButton m_btn_stop;
	afx_msg void OnNMDblclkListTestItem(NMHDR *pNMHDR, LRESULT *pResult);
};

