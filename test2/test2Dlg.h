
// test2Dlg.h : ͷ�ļ�
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
	CString TestName;//��������
	CString SndCmd;//����cmd
	CString CheckCmd;//����cmd
	int Status;//-1 Just Go	0:Status	1:Hex	2:Ascii
	int WaitTime;//��ʱʱ��
	int StillTime;//���պ���ͣ

}TEST_ITEM;
typedef struct ITEM_SHOW
{
	int ItemPos;//��Ŀλ��
	CString StrSend;//ʵ�ʷ���Cmd
	CString StrReceive;//ʵ�ʽ���cmd
	CString ReturnValue;//��������
	CString Result;//���
}ITEM_SHOW;
CString ConvertCharHex(CString a);
// Ctest2Dlg �Ի���
class Ctest2Dlg : public CDialogEx
{
// ����
public:
	Ctest2Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TEST2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

