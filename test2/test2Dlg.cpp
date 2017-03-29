
// test2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "test2.h"
#include "test2Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
static CString ConvertHexChar(BYTE* strByte,int num);
static CListBox *MyListBox;
CSemaphore h_Semaphore(1,1);//信号量
HANDLE hCom;//COM口
CStdioFile f_ComConfig;
CStdioFile f_TestItem;
CList<TEST_ITEM> l_Test_Item;
BYTE TmpReceive[100] = {0};
int Status_Flag = 3;
bool First_Start = 1;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
bool CheckReceiveData(BYTE*,int num, CString CheckMsg);
CString FindReceiveHex(BYTE*, int,CString);
CString FindReceiveASCII(BYTE* TmpReceive,int wCount,CString strreceive);
CList<ITEM_SHOW> l_Item_Show;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
END_MESSAGE_MAP()


// Ctest2Dlg 对话框




Ctest2Dlg::Ctest2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Ctest2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT_FILE_NAME, m_filename);
	DDX_Control(pDX, IDC_COMBO_SERIAL_PORT, m_serial_port);
	DDX_Control(pDX, IDC_COMBO_BAUD_RATE, m_baud_rate);
	//DDX_Control(pDX, IDC_COMBO_DATA_BIT, m_data_bit);
	//DDX_Control(pDX, IDC_COMBO_PARITY, m_parity);
	//DDX_Control(pDX, IDC_COMBO_STOP_BIT, m_stop_bit);
	DDX_Control(pDX, IDC_BUTTON_OPEN_CLOSE_SERIAL, m_open_close_serial);
	DDX_Control(pDX, IDC_LIST_TEST_ITEM, m_list_test_item);
	DDX_Control(pDX, IDC_LIST_MSG_LIST, m_msg_list);
	DDX_Control(pDX, IDC_BUTTON_START, m_start);
	DDX_Control(pDX, IDC_EDIT_MSG, m_msg_send);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_btn_send);
	DDX_Control(pDX, IDC_BUTTON_READ_FILE, m_btn_readfile);
	DDX_Control(pDX, IDC_STATIC_SEND, m_count_send);
	DDX_Control(pDX, IDC_STATIC_PASS, m_count_pass);
	DDX_Control(pDX, IDC_STATIC_FAILD, m_count_faild);
	DDX_Control(pDX, IDC_STATIC_PROJECT_NAME, m_count_filename);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_btn_pause);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btn_stop);
}

BEGIN_MESSAGE_MAP(Ctest2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_READ_FILE, &Ctest2Dlg::OnBnClickedButtonReadFile)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CLOSE_SERIAL, &Ctest2Dlg::OnBnClickedButtonOpenCloseSerial)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &Ctest2Dlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_START, &Ctest2Dlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &Ctest2Dlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &Ctest2Dlg::OnBnClickedButtonSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &Ctest2Dlg::OnBnClickedButtonPause)
	ON_EN_SETFOCUS(IDC_EDIT_MSG, &Ctest2Dlg::OnEnSetfocusEditMsg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TEST_ITEM, &Ctest2Dlg::OnNMDblclkListTestItem)
END_MESSAGE_MAP()


// Ctest2Dlg 消息处理程序

BOOL Ctest2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	/////////////////////////////////////////////////////////////
	//init serial port dlg//
	m_msg_list.SetHorizontalExtent(500);
	CString str_com = 0;
	CString str_baud = 0;
	CString str_size = 0;
	CString str_parity = 0;
	CString str_stop = 0;
	if(f_ComConfig.Open(_T("comconfig.txt"),CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate))
	{
		f_ComConfig.ReadString(str_com);
		f_ComConfig.ReadString(str_baud);
		f_ComConfig.ReadString(str_size);
		f_ComConfig.ReadString(str_parity);
		f_ComConfig.ReadString(str_stop);
		//MessageBox(_T("READ FILE OK"));
		TRACE("%d\n", sizeof(str_baud));
		TRACE("%s, %s, %s, %s, %s\n",str_com, str_baud, str_size, str_parity, str_stop);
		//f_ComConfig.Close();
	
	}
	bool flag = 0;
	f_ComConfig.SeekToBegin();

	CString strSerialPort;
	for(int i = 0; i < 20; i ++)
	{
		strSerialPort.Format(_T("COM%d"),i+1);
		m_serial_port.InsertString(i, strSerialPort);
	}
	//char a = str_com.GetAt(3);
	if(_ttoi(str_com)>= 0 && _ttoi(str_com) <= 12)
		m_serial_port.SetCurSel(_ttoi(str_com)-1);
	else
		m_serial_port.SetCurSel(0);
	//init baud rate dlg//
	CString strBaudRate[] = {
		_T("300"),_T("600"),_T("1200"),_T("2400"),_T("4800"),
		_T("9600"),_T("19200"),_T("38400"),_T("43000"),_T("56000"),_T("57600"),_T("115200")};
	for(int i = 0; i < 12; i ++)
	{

		if(strBaudRate[i] == str_baud)
		{
			//TRACE("hello\n");
			m_baud_rate.InsertString(i, strBaudRate[i]);
			m_baud_rate.SetCurSel(i);
			flag = 1;
		}else
			m_baud_rate.InsertString(i, strBaudRate[i]);
		
	}
	if(flag == 0)
		m_baud_rate.SetCurSel(11);
	m_list_test_item.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	UpdateData(FALSE);

	//init test item//
	m_list_test_item.InsertColumn(0,_T("Step"),LVCFMT_CENTER,40);
	m_list_test_item.InsertColumn(1,_T("TimeStart"),LVCFMT_CENTER,88);
	m_list_test_item.InsertColumn(2,_T("TimeUsed"),LVCFMT_CENTER,67);
	m_list_test_item.InsertColumn(3,_T("Result"),LVCFMT_CENTER,45);
	m_list_test_item.InsertColumn(4,_T("TestName"),LVCFMT_CENTER,200);
	m_list_test_item.InsertColumn(5,_T("Output"),LVCFMT_CENTER,150);
	///////////////////////////////////////////////////////////
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Ctest2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ctest2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Ctest2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Ctest2Dlg::OnBnClickedButtonReadFile()
{
	// TODO: 在此添加控件通知处理程序代码
	l_Test_Item.RemoveAll();
	m_list_test_item.DeleteAllItems();
    // 设置过滤器   
    TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");   
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
	CString strFileName;
    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
		strFileName = fileDlg.GetFileName();
		strFileName.Delete(strFileName.GetLength() - 4,4);
		strFilePath = fileDlg.GetPathName();
		
		//m_filename.SetWindowTextW(strFileName);  
    } 
	//MessageBox(strFilePath);
	int ok = 0;
	ok = f_TestItem.Open(strFilePath,CFile::modeRead);
	if(!ok)
	{
		return;
	}
	
	CString ReadStr;
	while(f_TestItem.ReadString(ReadStr))
	{
		if(ReadStr.Find(_T("_PROJECT_NAME")) == 0)
		{
			//ReadStr.Replace(_T(" "),_T(""));
			CString ProjectName;
			ProjectName = ReadStr.Right(ReadStr.GetLength() - 13);
			//AfxMessageBox(ProjectName);
			m_count_filename.SetWindowTextW(ProjectName);
		}

		if(ReadStr.Find(_T("_START")) == 0)
		{
			while(f_TestItem.ReadString(ReadStr))
			{
				if(ReadStr.IsEmpty()||ReadStr.Find(_T("_END")) == 0)
				{
					continue;
				}
				int pos;
				//find test item
				TEST_ITEM tmpitem;
				pos = ReadStr.Find(_T(":"));
				tmpitem.TestName = ReadStr.Left(pos);
				ReadStr.Delete(0,pos+1);
				//find sndcmd
				pos = ReadStr.Find(_T("|"));
				tmpitem.SndCmd = ReadStr.Left(pos);
				ReadStr.Delete(0,pos+1);

				//find checkcmd
				pos = ReadStr.Find(_T(","));
				tmpitem.CheckCmd = ReadStr.Left(pos);
				ReadStr.Delete(0,pos+1);

				//find status
				pos = ReadStr.Find(_T("@"));
				tmpitem.Status = _ttoi(ReadStr.Left(pos));
				ReadStr.Delete(0,pos+1);
				//find wait time
				pos = ReadStr.Find(_T("#"));
				tmpitem.WaitTime = _ttoi(ReadStr.Left(pos));
				ReadStr.Delete(0,pos+1);
				//find still time
				tmpitem.StillTime = _ttoi(ReadStr);
				l_Test_Item.AddTail(tmpitem);
			}
		}
	}
			//TRACE("%d\n",__LINE__);

	POSITION pos = l_Test_Item.GetHeadPosition();
	unsigned int i = 0;
	while(pos != NULL)
	{
		CString putstr;
		putstr.Format(L"%02d",i+1);
		m_list_test_item.InsertItem(i,putstr);
		m_list_test_item.SetItemText(i,4,l_Test_Item.GetNext(pos).TestName);
		i++;
	}
	
	m_start.EnableWindow(TRUE);
	m_btn_send.EnableWindow(TRUE);
	f_TestItem.Close();
		
}
int Ctest2Dlg::ReadFile(CStdioFile ItemFile)
{
	return -1;
}
int Ctest2Dlg::AddTestItem(CStdioFile ItemFile)
{
	return -1;
}
void Ctest2Dlg::OnBnClickedButtonOpenCloseSerial()
{
	CString status;
	m_open_close_serial.GetWindowTextW(status);
	
	if(status == "Open Serial")
	{
		CString m_port;
		m_serial_port.GetWindowTextW(m_port);
		CString m_baud;
		m_baud_rate.GetWindowTextW(m_baud);

#if 1
		hCom = CreateFile(m_port,//COM1口  
		GENERIC_READ|GENERIC_WRITE, //允许读和写  
		0, //独占方式  
		NULL,  
		OPEN_EXISTING, //打开而不是创建  
		0, //同步方式  
		NULL); 
		if(hCom==(HANDLE)-1)   
		{ 
		   AfxMessageBox(_T("打开COM失败!"));  
	    
		}else	//配置COM口
		{
			m_open_close_serial.SetWindowTextW(_T("Close Serial"));
			if(!SetupComm(hCom,100,100))
				AfxMessageBox(L"SetupComm Error");//输入输出缓冲区都为100
			COMMTIMEOUTS TimeOuts;  
			//设定读超时  
			TimeOuts.ReadIntervalTimeout=MAXDWORD;  
			TimeOuts.ReadTotalTimeoutMultiplier=0;  
			TimeOuts.ReadTotalTimeoutConstant=0;  
			//在读一次输入缓冲区的内容后读操作就立即返回，  
			//而不管是否读入了要求的字符。  
			//设定写超时  
			TimeOuts.WriteTotalTimeoutMultiplier=0;  
			TimeOuts.WriteTotalTimeoutConstant=0;  
			SetCommTimeouts(hCom,&TimeOuts); //设置超时

			DCB dcb;  
			if(!GetCommState(hCom, &dcb))
			{
				AfxMessageBox(L"GetCommState Error");
			}


			dcb.fBinary = TRUE;
			dcb.BaudRate = _ttoi(m_baud); //波特率 
			dcb.ByteSize = 8; //每个字节有8位  
			dcb.Parity = NOPARITY;//parity; //无奇偶校验位  
			dcb.StopBits = ONESTOPBIT;//_ttoi(m_stop); //1个停止位  
			if(!SetCommState(hCom, &dcb))
			{
				AfxMessageBox(L"Set CommState Error");
				return;	
			}
			PurgeComm(hCom, PURGE_TXCLEAR | PURGE_TXABORT); //清空缓冲区
			PurgeComm(hCom, PURGE_RXCLEAR | PURGE_RXABORT);
			//写入配置文件
			f_ComConfig.SetLength(0);					
			f_ComConfig.WriteString(m_port.Right(m_port.GetLength() - 3));
			f_ComConfig.WriteString(_T("\n"));
			f_ComConfig.WriteString(m_baud);

#endif
			//MessageBox(_T("WRITE CONFIG OK"));
			m_btn_readfile.EnableWindow(TRUE);

			//m_btn_send.EnableWindow(TRUE);
		}

	}else	//Close Serial
	{
		m_start.EnableWindow(FALSE);
		m_open_close_serial.SetWindowTextW(_T("Open Serial"));
		CloseHandle(hCom);
	}

	// TODO: 在此添加控件通知处理程序代码
}


void Ctest2Dlg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_msg_list.ResetContent();
}
UINT Ctest2Dlg::ThreadSend(LPVOID v)
{
	return FALSE;
}
UINT Ctest2Dlg::ThreadReceive(LPVOID v)
{
	Ctest2Dlg *msg_list = (Ctest2Dlg*)v;
	typedef struct count
	{
		int total;
		int pass;
		int faild;
	}COUNT;
	COUNT item_count;
	ITEM_SHOW ShowItem;
	item_count.faild = 0;
	item_count.pass = 0;
	item_count.total = 0;
	CString strsend;
	CString strreceive;
	int waittime;
	int status;
	DWORD wCount;//读取的字节数
	BOOL bReadStat;
	BOOL bWriteStat;

	int stilltime;
	
	BYTE cs;
	POSITION pos;
	int num = 0;
	pos = l_Test_Item.GetHeadPosition();
	while(pos!= NULL)
	{
		switch (Status_Flag)
		{
			case 0://stop
				return 0;
				break;
			case 1://start
				break;
			case 2://pause
				continue;
				break;
			default:
				break;
		}
		msg_list->m_list_test_item.EnsureVisible(num,TRUE);
		//msg_list->m_list_test_item.SetCheck(num);
		SYSTEMTIME st;
		GetLocalTime(&st);
		CString cur_time;
		cur_time.Format(_T("%02d:%02d:%02d.%03d"),st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		msg_list->m_list_test_item.SetItemText(num,1,cur_time);
		BYTE TmpSend[100];
		strsend = l_Test_Item.GetAt(pos).SndCmd;
		strreceive = l_Test_Item.GetAt(pos).CheckCmd;
		waittime = l_Test_Item.GetAt(pos).WaitTime;
		stilltime = l_Test_Item.GetAt(pos).StillTime;
		status = l_Test_Item.GetAt(pos).Status;
		TRACE(_T("sndCmd:%s, checkCmd:%s, waitTime:%d, stillTime:%d\n"), strsend, strreceive, waittime, stilltime);
		//处理发送和接收的消息  CString2Hex

		strsend.Replace(L" ",L"");
		strsend.Replace(L"\r\n",L"");
			
		//TRACE(_T("strsend:%s\n"),strsend);
		BYTE length = strsend.GetLength()/2+1;
		//TRACE("length:%02X\n",length);
		cs = length;
		if(length > 1)
		{
			CString StrHex = ConvertCharHex(strsend);
			BYTE *Send_Buff = NULL;
			Send_Buff = new BYTE[StrHex.GetLength() + 2];

			Send_Buff[0] = length;
			for(int i = 0; i < length ; i ++)
			{
				Send_Buff[i+1] = (BYTE)StrHex.GetAt(i);
				cs ^= Send_Buff[i+1];
			}
			Send_Buff[length] = cs;

			//TRACE("%d\n",sizeof(Send_Buff));
			bWriteStat = ::WriteFile(hCom,Send_Buff,length+1,&wCount,NULL);
			if(!bWriteStat)
			{
				AfxMessageBox(_T("Write COM Error"));
			}
			CString ins_msg_list;
			ins_msg_list.Append(_T("S: "));
			ins_msg_list.Append(ConvertHexChar(Send_Buff,wCount));

			int iReturn = msg_list->m_msg_list.AddString(ins_msg_list);
			
			msg_list->m_msg_list.SetTopIndex(iReturn);
			TRACE("Write bytes: %d",wCount);
			TRACE("Send: ");
			for(int i = 0; i < length+1; i ++)
			{
				TRACE("%02X ",Send_Buff[i]);
			}
			TRACE("\n");
			ShowItem.ItemPos = num;
			ShowItem.StrSend = ConvertHexChar(Send_Buff,wCount);
			delete []Send_Buff;
			

		}
		else
		{
			ShowItem.ItemPos = num;
			ShowItem.StrSend = "";
		}
		if(waittime <= 0)
		{
			continue;
		}

		long TimeStart = GetTickCount();
		long TimeEnd = TimeStart;
		//开始计时
		long TimeUsed = TimeEnd - TimeStart;
		while(1)
		{		
			CString putstr;
			putstr.Format(_T("%dms"),TimeUsed);
			bReadStat=::ReadFile(hCom,TmpReceive,100,&wCount,NULL);
			if(!bReadStat)
			{
				AfxMessageBox(_T("Read Com Error\n") );
				return FALSE;
			}
			if(wCount!= 0)
			{
				TRACE("ReadNum: %d\t",wCount);
				TRACE("Read: ");
				for(unsigned int i = 0; i < wCount; i ++)
				{
					TRACE("%02X ", TmpReceive[i]);
				}
				TRACE("\n");
				
				CString mystr;
				mystr.Append(_T("R: "));
				mystr.Append(ConvertHexChar(TmpReceive,wCount));
				int iReturn = msg_list->m_msg_list.AddString(mystr);
				msg_list->m_list_test_item.SetItemText(num,2,putstr);
				msg_list->m_msg_list.SetTopIndex(iReturn);
	
				ShowItem.StrReceive = ConvertHexChar(TmpReceive,wCount);
				bool flag = CheckReceiveData(TmpReceive, wCount,strreceive);
				if(flag)
				{
					switch (status)
					{
					case -1:
						{
							msg_list->m_list_test_item.SetItemText(num,3,_T("PASS"));
				
							ShowItem.Result = _T("PASS");
							ShowItem.ReturnValue = "";
							++item_count.pass;
						}
						break;
					case 0:
						if(TmpReceive[wCount-2] == 0x1)
						{
							msg_list->m_list_test_item.SetItemText(num,3,_T("PASS"));

							ShowItem.Result = _T("PASS");
							ShowItem.ReturnValue = "01";
							++item_count.pass;
						}
						else
						{
							msg_list->m_list_test_item.SetItemText(num,3,_T("FAILD"));

							ShowItem.Result = _T("FAILD");
							ShowItem.ReturnValue = "00";
							++item_count.faild;
						}
							TRACE("%02X\n",TmpReceive[wCount-2]);
						break;
					case 1:
						{
							CString ReceiveStr = FindReceiveHex(TmpReceive,wCount, strreceive);
							msg_list->m_list_test_item.SetItemText(num,3,_T("PASS"));
							msg_list->m_list_test_item.SetItemText(num,5,ReceiveStr);

							ShowItem.Result = _T("PASS");
							ShowItem.ReturnValue = ReceiveStr;
							++item_count.pass;
						}
						break;
					case 2:
						{
							CString ReceiveStr = FindReceiveASCII(TmpReceive,wCount,strreceive);
							msg_list->m_list_test_item.SetItemText(num,3,_T("PASS"));
							msg_list->m_list_test_item.SetItemText(num, 5,ReceiveStr);

							ShowItem.Result = _T("PASS");
							ShowItem.ReturnValue = ReceiveStr;
							++item_count.pass;
						}
						break;
						
					default:
						break;
					}
				}
				else
				{
				
					msg_list->m_list_test_item.SetItemText(num,2,_T("ERROR DATA"));
					//l_Test_Item.GetAt(pos).Result = _T("ERROR DATA");
					//l_Test_Item.GetAt(pos).ReturnValue = "";
					ShowItem.Result = _T("ERROR DATA");
					ShowItem.ReturnValue = "";
					++item_count.faild;
				}
				l_Test_Item.GetNext(pos);
				num++;
				CString strFaild;
				CString strPass;
				CString strTotal;
				strFaild.Format(_T("%d"),item_count.faild);
				strPass.Format(_T("%d"),item_count.pass);
				strTotal.Format(_T("%d"),num);
				msg_list->m_count_faild.SetWindowTextW(strFaild);
				msg_list->m_count_pass.SetWindowTextW(strPass);
				msg_list->m_count_send.SetWindowTextW(strTotal);
				l_Item_Show.AddTail(ShowItem);
				//msg_list->UpdateData(TRUE);
				Sleep(stilltime);
				break;
				
			}
			
			Sleep(10);
			TimeEnd = GetTickCount();
			TimeUsed = TimeEnd - TimeStart;
			if(TimeUsed >= waittime)
			{
				msg_list->m_list_test_item.SetItemText(num,2,_T("TIMEOUT"));
				ShowItem.Result = _T("TIME OUT");
				ShowItem.ReturnValue = "";
				l_Item_Show.AddTail(ShowItem);
				num++;
				break;
			}
		}
		if(pos == NULL)
		{
			//msg_list.Ctest2Dlg::OnBnClickedButtonStop();
			msg_list->OnBnClickedButtonStop();
		}


	}
	return TRUE;
}

void Ctest2Dlg::OnBnClickedButtonStart()
{
	if(l_Test_Item.GetCount() <= 0)
	{
		return;
	}
	Status_Flag = 1;
	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_TXABORT); //清空缓冲区
	PurgeComm(hCom, PURGE_RXCLEAR | PURGE_RXABORT);
	l_Item_Show.RemoveAll();
	AfxBeginThread(ThreadSend,0);
	TRACE("status:%d\n",Status_Flag);
	if(First_Start == 1)
	{
		for(int i = 0; i < m_list_test_item.GetItemCount(); i ++)
		{
			m_list_test_item.SetItemText(i,1,L"");
			m_list_test_item.SetItemText(i,2,L"");
			m_list_test_item.SetItemText(i,3,L"");
			m_list_test_item.SetItemText(i,5,L"");
		}
		First_Start = 0;
		AfxBeginThread(ThreadReceive,this,NULL);
	}
	m_start.EnableWindow(FALSE);
	m_btn_pause.EnableWindow(TRUE);
	m_btn_stop.EnableWindow(TRUE);
	SetTimer(1,1000,NULL);

	// TODO: 在此添加控件通知处理程序代码

}


void Ctest2Dlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	Status_Flag = 0;
	First_Start = 1;
	m_start.EnableWindow(TRUE);
}


void Ctest2Dlg::OnBnClickedButtonSend()
{	
	CString SendStr;
	m_msg_send.GetWindowTextW(SendStr);
		
	SendStr.Replace(L" ",L"");
	if(SendStr.GetLength() <= 0)
		return;
	BYTE *b_Send = NULL;
	
	b_Send = new BYTE[SendStr.GetLength()/2 + 2];
	b_Send[0] = SendStr.GetLength()/2 +1;
	CString SendHex = ConvertCharHex(SendStr);
	BYTE cs = b_Send[0];
	
	for(int i = 0; i < SendHex.GetLength(); i ++)
	{
		b_Send[i+1] = SendHex.GetAt(i);
		cs ^= b_Send[i+1];
	}
	
	b_Send[SendHex.GetLength() +1] = cs;
	bool bWriteStat;
	DWORD wCount = 0;
	bWriteStat = ::WriteFile(hCom,b_Send,SendHex.GetLength()+2,&wCount,NULL);
	if(!bWriteStat)
	{
		AfxMessageBox(_T("Write COM Error"));
	}
	
	CString ins_msg_list;
	ins_msg_list.Append(_T("S: "));
	ins_msg_list.Append(ConvertHexChar(b_Send,wCount));
	m_msg_list.AddString(ins_msg_list);
	//delete []b_Send;
	bool bReadStat;

	Sleep(100);

	bReadStat=::ReadFile(hCom,TmpReceive,100,&wCount,NULL);
	if(!bReadStat)
	{
		AfxMessageBox(_T("Read Com Error\n") );
		return ;
	}
	if(wCount!= 0)
	{
		TRACE("ReadNum: %d\t",wCount);
		TRACE("Read: ");
		for(unsigned int i = 0; i < wCount; i ++)
		{
			TRACE("%02X ", TmpReceive[i]);
		}
		TRACE("\n");
				
		CString mystr;
		mystr.Append(_T("R: "));
		mystr.Append(ConvertHexChar(TmpReceive,wCount));
		m_msg_list.AddString(mystr);
	}
	// TODO: 在此添加控件通知处理程序代码

}



void Ctest2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		case 1:
			long st;
			st = GetTickCount();
			break;
		default:
			break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
MSG_LEN Ctest2Dlg::HandleSndMsg(CString data, int len)
{
	data.Replace(L" ",L"");
	TRACE("sizeof data:%d\n",data.GetLength());
	for(int i = 0; i < len ; i ++)
	{

	}
	TRACE("data:");
	
	//TRACE("%08X\n",data);
	char *b = "0";
	
	MSG_LEN a;
	
	return a;
}
CString ConvertCharHex(CString str)
{
	CString tar;
	for(int i = 0; i < str.GetLength(); i+=2)
	{
		BYTE h_ch,l_ch,ch;
		h_ch = (BYTE)str.GetAt(i);
		l_ch = (BYTE)str.GetAt(i+1);
		if(l_ch >= '0'&& l_ch <= '9')
			ch = l_ch - 0X30;
		else if(l_ch >= 'A' && l_ch <= 'Z')
			ch = l_ch - 'A' + 10;
		else if(l_ch >= 'a' && l_ch <= 'z')
			ch = l_ch - 'a' +10;
		else
			ch = -1;

		if(h_ch >= '0'&& h_ch <= '9')
			ch |= (h_ch - 0X30) << 4;
		else if(h_ch >= 'A' && h_ch <= 'Z')
			ch |= (h_ch - 'A' + 10) << 4;
		else if(h_ch >= 'a' && h_ch <= 'z')
			ch |= (h_ch - 'a' +10) << 4;
		else
			ch = -1 << 4;
		//TRACE("%02X ",ch);
		tar.AppendChar(ch);
	}
	for(int i = 0; i < tar.GetLength(); i ++)
	{
		//TRACE("%02X ",tar.GetAt(i));
	}

	return tar;
}
CString ConvertHexChar(BYTE* strByte,int num)
{
	BYTE ByteHigh;
	BYTE ByteLow;
	CString returnStr;
	for(int i = 0; i < num; i ++)
	{
		ByteHigh = (strByte[i] & 0xF0) >> 4;
		ByteLow = (strByte[i] & 0xF) >> 0;
		if(ByteHigh >= 0x0 && ByteHigh <= 0x9)
			returnStr.AppendChar(ByteHigh +0x30);
		else if(ByteHigh >= 0xA && ByteHigh <= 0xF)
			returnStr.AppendChar(ByteHigh +0x37);
		else if(ByteHigh >=0xa && ByteHigh <= 0xf)
			returnStr.AppendChar(ByteHigh + 0x37);
		else 
			returnStr.AppendChar('F');

		if(ByteLow >= 0x0 && ByteLow <= 0x9)
			returnStr.AppendChar(ByteLow +0x30);
		else if(ByteLow >= 0xA && ByteLow <= 0xF)
			returnStr.AppendChar(ByteLow +0x37);
		else if(ByteLow >=0xa && ByteLow <= 0xf)
			returnStr.AppendChar(ByteLow + 0x37);
		else 
			returnStr.AppendChar('F');
		returnStr.AppendChar(' ');
	}
	return returnStr;
}

bool CheckReceiveData(BYTE* DataReceive,int num, CString CheckMsg)
{
	CString DataString = ConvertHexChar(DataReceive, num);
	CString CheckHex = ConvertCharHex(CheckMsg);
	DataString.Replace(L" ",L"");
	DataString.Replace(L"\r\n",L"");
	CheckMsg.Replace(L" ",L"");
	BYTE DataLen = DataReceive[0];

	//DataLen += (DataLen/10)*8;
	BYTE cs = DataReceive[0];
	
	for(int i = 1; i < num - 1; i ++)
	{
		cs ^= DataReceive[i];
	}
	/*TRACE(_T("num - 1 = %02X,"),num - 1);
	TRACE(_T("DataLen:%02X,cs:%02X,DataString:%s,CheckMsg:%s,DataReceive[num-1]:%02X\n"),DataLen,cs,DataString,CheckMsg,DataReceive[num -1]);*/
	if((DataString.Find(CheckMsg) == 2) &&
		(cs == DataReceive[num - 1]) &&
		(DataLen == num - 1))
		return TRUE;
	else
		return FALSE;
}

CString FindReceiveASCII(BYTE* DataReceive,int wCount,CString strreceive)
{
	CString DataString = ConvertHexChar(DataReceive,wCount);
	DataString.Replace(L" ",L"");
	strreceive.Replace(L" ",L"");
	int len = DataString.Find(strreceive);
	DataString.Delete(0,strreceive.GetLength()+2);
	DataString.Delete(DataString.GetLength()-2,2);
	CString AscStr = ConvertCharHex(DataString);
	AscStr.Format(L"%s",AscStr);
	//TRACE(_T("ASCII:%s,length:%d\n"),AscStr,AscStr.GetLength());

	return AscStr;
}
CString FindReceiveHex(BYTE* DataReceive, int wCount, CString strreceive)
{
	CString DataHex = ConvertHexChar(DataReceive,wCount);
	DataHex.Replace(L" ",L"");
	strreceive.Replace(L" ",L"");
	int len = DataHex.Find(strreceive);
	DataHex.Delete(0,strreceive.GetLength() + 2);
	DataHex.Delete(DataHex.GetLength() - 2, 2);
	//CString AscStr = ConvertCharHex(DataHex);
	//CString rt = _T("HELLO");
	return DataHex;
}

void Ctest2Dlg::OnBnClickedButtonPause()
{
	Status_Flag = 2;
	m_start.EnableWindow(TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


void Ctest2Dlg::OnEnSetfocusEditMsg()
{
	m_msg_send.ShowBalloonTip(L"",L"请输入十六进制，不包括数据长度和校验位。\n例如：FF 00 FF FF");
	// TODO: 在此添加控件通知处理程序代码
}


void Ctest2Dlg::OnNMDblclkListTestItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW *pNMListView=(NM_LISTVIEW *)pNMHDR;
	int nItem=pNMListView->iItem;//行号
    if(nItem>=0 && nItem<m_list_test_item.GetItemCount())//判断双击位置是否在有数据的列表项上面
    {
		//AfxMessageBox(_T("HELLO"));
		CString str;
		str = m_list_test_item.GetItemText(nItem,4);
		POSITION pos;
		pos = l_Item_Show.GetHeadPosition();
		while(pos != NULL)
		{
			TRACE(L"HELLO\n");
			if(l_Item_Show.GetAt(pos).ItemPos == nItem)
			{
				CString ShowStr;
				ShowStr.Format(_T("TestName:%s\nResult:%s\nSendMsg:%s\nReceiveMsg:%s\nReceiveValue:%s"),
					str,l_Item_Show.GetAt(pos).Result, l_Item_Show.GetAt(pos).StrSend,
					l_Item_Show.GetAt(pos).StrReceive, l_Item_Show.GetAt(pos).ReturnValue);
				AfxMessageBox(ShowStr);
			}
			l_Item_Show.GetNext(pos);
		}
		//AfxMessageBox(str);
		
    }
	*pResult = 0;
}
