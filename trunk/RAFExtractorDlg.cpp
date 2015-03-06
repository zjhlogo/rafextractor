
// RAFExtractorDlg.cpp : implementation file
//

#include "stdafx.h"

#include "RAFError.h"
#include "RAFReader.h"
#include "RAFDataReader.h"
#include "RAFExtractor.h"
#include "RAFExtractorDlg.h"

#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// CRAFExtractorDlg dialog



CRAFExtractorDlg::CRAFExtractorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRAFExtractorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRAFExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_FileTreeView);
	DDX_Control(pDX, IDC_OPENFILE, m_OpenFileButton);
	DDX_Control(pDX, IDC_OPENDIR, m_OpenDirButton);
	DDX_Control(pDX, IDC_RADIOFILES, m_FilesRadio);
	DDX_Control(pDX, IDC_EDITSEARCH, m_SearchEdit);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressBar);
	DDX_Control(pDX, IDC_PROGRESSINFO, m_ProgressInfo);
}

BEGIN_MESSAGE_MAP(CRAFExtractorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CRAFExtractorDlg::OnNMClickTree1)
	ON_BN_CLICKED(IDC_OPENFILE, &CRAFExtractorDlg::OnBnClickedOpenfile)
	ON_BN_CLICKED(IDC_OPENDIR, &CRAFExtractorDlg::OnBnClickedOpendir)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE1, &CRAFExtractorDlg::OnTvnItemexpandingTree1)
	ON_BN_CLICKED(IDC_BUTTONSEARCH, &CRAFExtractorDlg::OnBnClickedButtonsearch)
	ON_BN_CLICKED(IDC_BUTTONCLEAR, &CRAFExtractorDlg::OnBnClickedButtonclear)
	ON_BN_CLICKED(IDC_EXTRACT, &CRAFExtractorDlg::OnBnClickedExtract)
END_MESSAGE_MAP()


// CRAFExtractorDlg message handlers

BOOL CRAFExtractorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// Load images for tree control

	wchar_t windowsDir[MAX_PATH];
	GetWindowsDirectory(windowsDir, MAX_PATH);

	SHFILEINFO sfit;

	SHGetFileInfo(_T("FILE"), FILE_ATTRIBUTE_NORMAL, &sfit, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SHELLICONSIZE | SHGFI_SMALLICON);
	fileId = sfit.iIcon;

	HIMAGELIST hImageList = (HIMAGELIST)SHGetFileInfo(windowsDir, FILE_ATTRIBUTE_NORMAL, &sfit, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SHELLICONSIZE | SHGFI_SMALLICON);
	folderId = sfit.iIcon;

	CImageList imageList;
	m_FileTreeView.SetImageList(imageList.FromHandle(hImageList), TVSIL_NORMAL);

	// Load images buttons

	SHFILEINFO sfib;

	SHGetFileInfo(_T("FILE"), FILE_ATTRIBUTE_NORMAL, &sfib, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);
	m_OpenFileButton.SetIcon(sfib.hIcon);

	m_OpenFileToolTip.Create(this);
	m_OpenFileToolTip.AddTool(&m_OpenFileButton, _T("Open a single Riot Archive File."));
	m_OpenFileToolTip.Activate(TRUE);

	SHGetFileInfo(windowsDir, FILE_ATTRIBUTE_NORMAL, &sfib, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_OPENICON);
	m_OpenDirButton.SetIcon(sfib.hIcon);

	m_OpenDirToolTip.Create(this);
	m_OpenDirToolTip.AddTool(&m_OpenDirButton, _T("Open all Riot Archive Files in a directory."));
	m_OpenDirToolTip.Activate(TRUE);

	m_FilesRadio.SetCheck(1);

	m_ProgressBar.SetState(PBST_PAUSED);
	m_ProgressBar.SetMarquee(TRUE, 1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRAFExtractorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRAFExtractorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CRAFExtractorDlg::PreTranslateMessage(MSG* pMsg)
{
	m_OpenFileToolTip.RelayEvent(pMsg);
	m_OpenDirToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CRAFExtractorDlg::setAllChildren(HTREEITEM hParent, HTREEITEM hCurrentItem)
{
	HTREEITEM hNextItem = m_FileTreeView.GetChildItem(hCurrentItem);

	while (hNextItem)
	{
		m_FileTreeView.SetCheck(hNextItem, !m_FileTreeView.GetCheck(hParent));

		if (m_FileTreeView.ItemHasChildren(hNextItem))
		{
			setAllChildren(hParent, hNextItem);
		}

		hNextItem = m_FileTreeView.GetNextItem(hNextItem, TVGN_NEXT);
	}
}

void CRAFExtractorDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	UINT uFlags = 0;
	CPoint pt(0, 0);
	GetCursorPos(&pt);
	m_FileTreeView.ScreenToClient(&pt);
	HTREEITEM hParent = m_FileTreeView.HitTest(pt, &uFlags);

	if (hParent && (TVHT_ONITEMSTATEICON & uFlags))
	{
		setAllChildren(hParent, hParent);
	}
	*pResult = 0;
}

void CRAFExtractorDlg::addRootNodes()
{
	unordered_set<string> uniqueDirs;
	vector <pair<string, treePointer>> uniqueFiles;

	for (auto& curArchive : archives)
	{
		for (auto& curFile : curArchive.second.fileEntries)
		{
			string relativePath = curFile.pathString;
			
			size_t endPos = relativePath.find_first_of("/", 0);

			if (endPos != string::npos)
			{
				string rootDirectory;
				rootDirectory = relativePath.substr(0, endPos);
				uniqueDirs.insert(rootDirectory);
			}

			else
			{
				treePointer tp;
				tp.rArchive = &curArchive.first;
				tp.rFile = &curFile;
				pair<string, treePointer> stp;

				stp.first = relativePath;
				stp.second = tp;

				uniqueFiles.push_back(stp);
			}
		}
	}

	for (auto& curDir : uniqueDirs)
	{
		CString longString = (CString)curDir.c_str();

		addTreeItem(NULL, longString);
	}

	sort(uniqueFiles.begin(), uniqueFiles.end(), sortByName);

	for (auto& curFile : uniqueFiles)
	{
		CString longString = (CString)curFile.first.c_str();
		treePointers.push_back(curFile.second);

		addTreeItem(NULL, longString, treePointers.back());
	}
}

void CRAFExtractorDlg::OnBnClickedOpenfile()
{
	CString fileName;
	wchar_t* buffer = fileName.GetBuffer(MAX_PATH);

	CFileDialog openFileDlg(TRUE);
	OPENFILENAME& ofn = openFileDlg.GetOFN();
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _T("Riot Archive File (*.raf)\0*.raf\0All Files (*.*)\0*.*\0");
	ofn.lpstrDefExt = _T("raf");

	if (openFileDlg.DoModal() == IDOK)
	{
		initShortProcess();

		try
		{
			archives.clear();

			RAFReader inRAFReader;
			inRAFReader.readFile(buffer);
			
			pair<wstring, RAFReader> archive;
			archive.first = buffer;
			archive.second = inRAFReader;

			archives.push_back(archive);

			treePointers.clear();
			m_FileTreeView.DeleteAllItems();

			addRootNodes();
		}

		catch (RAFError &e)
		{
			MessageBox((CString)e.what(), _T("Error"), MB_ICONERROR);
		}

		endShortProcess();
	}

	fileName.ReleaseBuffer();
}

bool CRAFExtractorDlg::findFiles(wstring path, vector<pair<wstring, RAFReader>>& archives)
{
	wstring mask = _T("*");
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA findData;
	wstring findFirstPath;
	stack<wstring> directories;

	directories.push(path);
	archives.clear();

	while (!directories.empty())
	{
		path = directories.top();
		findFirstPath = path + _T("\\") + mask;
		directories.pop();

		hFind = FindFirstFile(findFirstPath.c_str(), &findData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		do
		{
			if (wcscmp(findData.cFileName, _T(".")) != 0 && wcscmp(findData.cFileName, _T("..")) != 0)
			{
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					directories.push(path + _T("\\") + findData.cFileName);
				}
				else
				{
					wstring extension = findData.cFileName;
					extension = extension.substr(extension.length() - 4, 4);
					if (wcscmp(extension.c_str(), _T(".raf")) == 0)
					{
						pair<wstring, RAFReader> archive;
						archive.first = path + _T("\\") + findData.cFileName;
						archives.push_back(archive);
					}
				}
			}
		} while (FindNextFile(hFind, &findData) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);
			return false;
		}

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return true;
}

UINT CRAFExtractorDlg::openDirWorkerThread(LPVOID lParam)
{
	CRAFExtractorDlg* dlg = (CRAFExtractorDlg*)lParam;

	dlg->openDir();

	return 0;
}

bool CRAFExtractorDlg::openDirDlg(wstring& dir)
{
	BROWSEINFO bi = { 0 };
	wchar_t folder[MAX_PATH];
	bi.lpszTitle = _T("Please choose a directory.");
	bi.pszDisplayName = folder;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		wchar_t fullPath[MAX_PATH];
		SHGetPathFromIDList(pidl, fullPath);

		IMalloc* imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
			dir = fullPath;
			return true;
		}
	}

	return false;
}

void CRAFExtractorDlg::openDir()
{
	wstring dir;
	if (!openDirDlg(dir))
	{
		return;
	}

	initLongProcess();

	archives.clear();

	setProgressInfo(_T("Searching for Riot Archive Files..."));

	if (!findFiles(dir, archives))
	{
		MessageBox(_T("Unable to find any Riot Archive Files after searching all directories."), _T("Information"), MB_ICONINFORMATION | MB_OK);
		return;
	}

	clearProgressInfo();

	m_FileTreeView.DeleteAllItems();
	treePointers.clear();

	try
	{
		setProgressInfo(_T("Loading Riot Archive Files..."));

		for (auto& curArchive : archives)
		{
			RAFReader inRAFReader;
			inRAFReader.readFile(curArchive.first.c_str());
			curArchive.second = inRAFReader;
		}

		clearProgressInfo();
		setProgressInfo(_T("Creating items..."));

		addRootNodes();

		clearProgressInfo();
	}

	catch (RAFError &e)
	{
		MessageBox((CString)e.what(), _T("Error"), MB_ICONERROR);
	}

	endLongProcess();
}

void CRAFExtractorDlg::OnBnClickedOpendir()
{
	AfxBeginThread(openDirWorkerThread, this);
}

void CRAFExtractorDlg::OnTvnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	initShortProcess();

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	UINT uFlags = 0;
	CPoint pt(0, 0);
	GetCursorPos(&pt);
	m_FileTreeView.ScreenToClient(&pt);
	HTREEITEM hSelectedFolder = m_FileTreeView.HitTest(pt, &uFlags);

	if (hSelectedFolder && (TVHT_ONITEMBUTTON & uFlags))
	{
		HTREEITEM hChild = m_FileTreeView.GetChildItem(hSelectedFolder);
		if (!hChild)
		{
			dynamicallyLoadChildren(hSelectedFolder);
		}
	}
	*pResult = 0;

	endShortProcess();
}

bool sortByName(const pair<string, treePointer> &lhs, const pair<string, treePointer> &rhs)
{ 
	string lhslc, rhslc;

	lhslc = lhs.first;
	rhslc = rhs.first;

	std::transform(rhslc.begin(), rhslc.end(), rhslc.begin(), ::tolower);
	std::transform(lhslc.begin(), lhslc.end(), lhslc.begin(), ::tolower);

	return lhslc < rhslc;
}

void CRAFExtractorDlg::dynamicallyLoadChildren(HTREEITEM hParent)
{
	BOOL isParentChecked = m_FileTreeView.GetCheck(hParent);
	HTREEITEM hCurrent = hParent;

	wstring dirs;
	while (hCurrent)
	{
		dirs.insert(0, _T("/"));
		dirs.insert(0, m_FileTreeView.GetItemText(hCurrent));
		hCurrent = m_FileTreeView.GetParentItem(hCurrent);
	}

	string parentPath(dirs.begin(), dirs.end());
	size_t pathLength = parentPath.length();

	unordered_set<string> uniqueDirs;
	vector <pair<string, treePointer>> uniqueFiles;

	for (auto& curArchive : archives)
	{
		for (auto& curFile : curArchive.second.fileEntries)
		{
			string relativePath = curFile.pathString;

			if (relativePath.compare(0, pathLength, parentPath) != 0)
			{
				continue;
			}
				
			size_t endPos = relativePath.find_first_of("/", pathLength);
			string add = relativePath.substr(pathLength, endPos - pathLength);

			if (relativePath.find_first_of("/", endPos) == string::npos)
			{
				treePointer tp;
				tp.rArchive = &curArchive.first;
				tp.rFile = &curFile;
				pair<string, treePointer> stp;

				stp.first = add;
				stp.second = tp;

				uniqueFiles.push_back(stp);
			}

			else
			{
				uniqueDirs.insert(add);
			}
		}
	}

	for (auto& curDir : uniqueDirs)
	{
		CString longString = (CString)curDir.c_str();

		HTREEITEM hNew = addTreeItem(hParent, longString);

		if (isParentChecked)
		{
			m_FileTreeView.SetCheck(hNew);
		}
	}

	sort(uniqueFiles.begin(), uniqueFiles.end(), sortByName);

	for (auto& curFile : uniqueFiles)
	{
		CString longString = (CString)curFile.first.c_str();

		treePointers.push_back(curFile.second);

		HTREEITEM hNew = addTreeItem(hParent, longString, treePointers.back());

		if (isParentChecked)
		{
			m_FileTreeView.SetCheck(hNew);
		}
	}
}

UINT CRAFExtractorDlg::searchWorkerThread(LPVOID lParam)
{
	CRAFExtractorDlg* dlg = (CRAFExtractorDlg*)lParam;
	dlg->initLongProcess();
	dlg->setProgressInfo(_T("Searching..."));

	dlg->search();

	dlg->endLongProcess();
	dlg->clearProgressInfo();
	return 0;
}

void CRAFExtractorDlg::search()
{
	CString searchText;
	m_SearchEdit.GetWindowTextW(searchText);

	if (searchText.IsEmpty())
	{
		return;
	}

	treePointers.clear();
	m_FileTreeView.DeleteAllItems();

	if (m_FilesRadio.GetCheck())
	{
		vector <pair<string, treePointer>> uniqueFiles;

		for (auto& curArchive : archives)
		{
			for (auto& curFile : curArchive.second.fileEntries)
			{
				string relativePath = curFile.pathString;

				size_t fileIndex = relativePath.rfind("/");
				string fileName;

				if (fileIndex != string::npos)
				{
					fileName = relativePath.substr(fileIndex);
				}

				else
				{
					fileName = relativePath;
				}

				CString wFileName = (CString)fileName.c_str();

				if (wFileName.Find(searchText) > -1)
				{
					treePointer tp;
					tp.rArchive = &curArchive.first;
					tp.rFile = &curFile;
					pair<string, treePointer> stp;

					stp.first = relativePath;
					stp.second = tp;

					uniqueFiles.push_back(stp);
				}
			}
		}

		if (uniqueFiles.empty())
		{
			MessageBox(_T("No results found, please refine your search."), _T("Information"), MB_ICONINFORMATION);
			return;
		}

		sort(uniqueFiles.begin(), uniqueFiles.end(), sortByName);

		for (auto& curFile : uniqueFiles)
		{
			CString longString = (CString)curFile.first.c_str();
			treePointers.push_back(curFile.second);

			addTreeItem(NULL, longString, treePointers.back());
		}
	}

	else
	{
		unordered_set<string> uniqueDirs;

		for (auto& curArchive : archives)
		{
			for (auto& curFile : curArchive.second.fileEntries)
			{
				string relativePath = curFile.pathString;

				size_t fileIndex = relativePath.rfind("/");

				if (fileIndex == string::npos)
				{
					continue;
				}

				string dirName = relativePath.substr(0, fileIndex);
				CString wDirName = (CString)dirName.c_str();

				int index = wDirName.Find(searchText);

				if (index > -1)
				{
					wDirName.Tokenize(_T("/"), index);
					uniqueDirs.insert(dirName.substr(0, index - 1));
				}
			}
		}

		if (uniqueDirs.empty())
		{
			MessageBox(_T("No results found, please refine your search."), _T("Information"), MB_ICONINFORMATION);
			return;
		}

		for (auto& curDir : uniqueDirs)
		{
			CString longString = (CString)curDir.c_str();

			addTreeItem(NULL, longString);
		}
	}
}

void CRAFExtractorDlg::OnBnClickedButtonsearch()
{
	AfxBeginThread(searchWorkerThread, this);
}


void CRAFExtractorDlg::OnBnClickedButtonclear()
{
	initShortProcess();

	m_SearchEdit.SetWindowTextW(_T(""));
	treePointers.clear();
	m_FileTreeView.DeleteAllItems();
	addRootNodes();

	endShortProcess();
}

UINT CRAFExtractorDlg::extractWorkerThread(LPVOID lParam)
{
	CRAFExtractorDlg* dlg = (CRAFExtractorDlg*)lParam;

	dlg->initLongProcess();

	dlg->iterateTree(NULL);

	dlg->endLongProcess();
	return 0;
}

void CRAFExtractorDlg::iterateTree(HTREEITEM hItem)
{
	if (hItem)
	{
		extractItem(hItem);
		hItem = m_FileTreeView.GetNextItem(hItem, TVGN_CHILD);
		while (hItem)
		{
			iterateTree(hItem);
			hItem = m_FileTreeView.GetNextItem(hItem, TVGN_NEXT);
		}
	}
	else
	{
		HTREEITEM hItem = m_FileTreeView.GetNextItem(NULL, TVGN_ROOT);
		while (hItem)
		{
			iterateTree(hItem);
			hItem = m_FileTreeView.GetNextItem(hItem, TVGN_NEXT);
		}
	}
}

void CRAFExtractorDlg::extractItem(HTREEITEM hItem)
{
	if (!m_FileTreeView.GetCheck(hItem))
	{
		return;
	}

	if (m_FileTreeView.ItemHasChildren(hItem))
	{
		HTREEITEM hChild = m_FileTreeView.GetChildItem(hItem);
		if (hChild)
		{
			return;
		}
		
		HTREEITEM hCurrent = hItem;

		wstring dirs;
		while (hCurrent)
		{
			dirs.insert(0, _T("/"));
			dirs.insert(0, m_FileTreeView.GetItemText(hCurrent));
			hCurrent = m_FileTreeView.GetParentItem(hCurrent);
		}

		string treeItemDirs(dirs.begin(), dirs.end());
		size_t fileIndex = treeItemDirs.rfind("/");
		treeItemDirs = treeItemDirs.substr(0, fileIndex + 1);

		for (auto& curArchive : archives)
		{
			for (auto& curFile : curArchive.second.fileEntries)
			{
				string searchString = curFile.pathString;

				if (searchString.find(treeItemDirs) == string::npos)
				{
					continue;
				}

				try
				{
					RAFDataReader inRAFDataReader(curFile, curArchive.first);

					CString progressString;
					progressString += _T("Extracting: ");
					progressString += curFile.pathString.c_str();
					setProgressInfo(progressString);

					inRAFDataReader.extractFile(outDir);

					clearProgressInfo();
				}

				catch (RAFError &e)
				{
					MessageBox((CString)e.what(), _T("Error"), MB_ICONERROR);
				}
			}
		}
	}

	else
	{
		treePointer* tp = (treePointer*)m_FileTreeView.GetItemData(hItem);
		RAFFile currentFile = *tp->rFile;
		wstring fileArchive = *tp->rArchive;

		try
		{
			RAFDataReader inRAFDataReader(currentFile, fileArchive);

			CString progressString;
			progressString += _T("Extracting: ");
			progressString += currentFile.pathString.c_str();
			setProgressInfo(progressString);

			inRAFDataReader.extractFile(outDir);

			clearProgressInfo();
		}

		catch (RAFError &e)
		{
			MessageBox((CString)e.what(), _T("Error"), MB_ICONERROR);
		}
	}
}

void CRAFExtractorDlg::OnBnClickedExtract()
{
	if (!openDirDlg(outDir))
	{
		return;
	}

	AfxBeginThread(extractWorkerThread, this);
}

void CRAFExtractorDlg::initLongProcess()
{
	EnableWindow(FALSE);
	m_ProgressBar.SetState(PBST_NORMAL);
}

void CRAFExtractorDlg::endLongProcess()
{
	EnableWindow(TRUE);
	m_ProgressBar.SetState(PBST_PAUSED);
}

void CRAFExtractorDlg::initShortProcess()
{
	EnableWindow(FALSE);
	AfxGetApp()->DoWaitCursor(1);
}

void CRAFExtractorDlg::endShortProcess()
{
	EnableWindow(TRUE);
	AfxGetApp()->DoWaitCursor(-1);
}

void CRAFExtractorDlg::setProgressInfo(CString text)
{
	m_ProgressInfo.SetWindowTextW(text);
}

void CRAFExtractorDlg::clearProgressInfo()
{
	m_ProgressInfo.SetWindowTextW(_T(" "));
}

HTREEITEM CRAFExtractorDlg::addTreeItem(const HTREEITEM& hParent, const CString& name)
{
	TVINSERTSTRUCT tvItem;
	tvItem.hParent = hParent;
	tvItem.hInsertAfter = TVI_SORT;
	tvItem.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
	tvItem.item.pszText = (LPTSTR)(LPCTSTR)name;
	tvItem.item.iImage = folderId;
	tvItem.item.iSelectedImage = folderId;
	tvItem.item.cChildren = 1;

	HTREEITEM hNew = m_FileTreeView.InsertItem(&tvItem);
	
	return hNew;
}

HTREEITEM CRAFExtractorDlg::addTreeItem(const HTREEITEM& hParent, const CString& name, const treePointer& tp)
{
	TVINSERTSTRUCT tvItem;
	tvItem.hParent = hParent;
	tvItem.hInsertAfter = TVI_LAST;
	tvItem.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
	tvItem.item.pszText = (LPTSTR)(LPCTSTR)name;
	tvItem.item.iImage = fileId;
	tvItem.item.iSelectedImage = fileId;
	tvItem.item.cChildren = 0;
	tvItem.item.lParam = reinterpret_cast<LPARAM>(&tp);

	HTREEITEM hNew = m_FileTreeView.InsertItem(&tvItem);

	return hNew;
}