
// RAFExtractorDlg.h : header file
//

#pragma once
#include "RAFReader.h"

#include "afxcmn.h"
#include "afxwin.h"

using namespace std;

struct treePointer
{
	const wstring* rArchive;
	const RAFFile* rFile;
};

bool sortByName(const pair<string, treePointer> &lhs, const pair<string, treePointer> &rhs);

// CRAFExtractorDlg dialog
class CRAFExtractorDlg : public CDialog
{
// Construction
public:
	CRAFExtractorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RAFEXTRACTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOpenfile();
	afx_msg void OnBnClickedOpendir();
	afx_msg void OnTvnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonsearch();
	afx_msg void OnBnClickedButtonclear();
	afx_msg void OnBnClickedExtract();
	DECLARE_MESSAGE_MAP()
private:
	int fileId;
	int folderId;
	vector<pair<wstring, RAFReader>> archives;
	list<treePointer> treePointers;
	wstring outDir;
	CTreeCtrl m_FileTreeView;
	CToolTipCtrl m_OpenFileToolTip;
	CToolTipCtrl m_OpenDirToolTip;
	CButton m_OpenFileButton;
	CButton m_OpenDirButton;
	CButton m_FilesRadio;
	CEdit m_SearchEdit;
	CProgressCtrl m_ProgressBar;
	CStatic m_ProgressInfo;
	BOOL PreTranslateMessage(MSG* pMsg);
	static UINT openDirWorkerThread(LPVOID lParam);
	bool openDirDlg(wstring& dir);
	void openDir();
	void addRootNodes();
	void setAllChildren(HTREEITEM hParent, HTREEITEM hCurrentItem);
	bool findFiles(wstring path, vector<pair<wstring, RAFReader>>& archives);
	void dynamicallyLoadChildren(HTREEITEM hSelected);
	static UINT searchWorkerThread(LPVOID lParam);
	void search();
	static UINT extractWorkerThread(LPVOID lParam);
	void iterateTree(HTREEITEM hItem = NULL);
	void extractItem(HTREEITEM hItem);
	void initLongProcess();
	void endLongProcess();
	void initShortProcess();
	void endShortProcess();
	void setProgressInfo(CString text);
	void clearProgressInfo();
	HTREEITEM addTreeItem(const HTREEITEM& hParent, const CString& name);
	HTREEITEM addTreeItem(const HTREEITEM& hParent, const CString& name, const treePointer& tp);
};
