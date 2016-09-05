
// MotionDetectDlg.h : 头文件
//

#pragma once


// CMotionDetectDlg 对话框
class CMotionDetectDlg : public CDialogEx
{
// 构造
public:
	CMotionDetectDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MOTIONDETECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	void SetNoVideo(CWnd *p);
	/*
	 *	Description : current selected channel
	 */
	int nCurChl;

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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnPlayWndMenu(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPlayFile();
	afx_msg void OnPlayStream();
};
