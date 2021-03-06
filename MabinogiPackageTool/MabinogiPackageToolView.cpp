
// MabinogiPackageToolView.cpp : CMabinogiPackageToolView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MabinogiPackageTool.h"
#endif

#include "MabinogiPackageToolDoc.h"
#include "MabinogiPackageToolView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMabinogiPackageToolView

IMPLEMENT_DYNCREATE(CMabinogiPackageToolView, CListView)

BEGIN_MESSAGE_MAP(CMabinogiPackageToolView, CListView)
	ON_WM_STYLECHANGED()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
//	ON_WM_CANCELMODE()
ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CMabinogiPackageToolView::OnLvnItemchanged)
ON_COMMAND(ID_EDIT_VIEW, &CMabinogiPackageToolView::OnEditView)
ON_COMMAND(ID_EDIT_VIEW_AS, &CMabinogiPackageToolView::OnEditViewAs)
ON_COMMAND(ID_EDIT_EXTRACT_TO, &CMabinogiPackageToolView::OnEditExtractTo)
ON_NOTIFY_REFLECT(NM_DBLCLK, &CMabinogiPackageToolView::OnNMDblclk)
END_MESSAGE_MAP()

// CMabinogiPackageToolView 构造/析构

CMabinogiPackageToolView::CMabinogiPackageToolView()
{
	// TODO: 在此处添加构造代码

}

CMabinogiPackageToolView::~CMabinogiPackageToolView()
{
}

BOOL CMabinogiPackageToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= LVS_REPORT|LVS_SHAREIMAGELISTS;
	cs.dwExStyle |= LVS_EX_TRACKSELECT;
	return CListView::PreCreateWindow(cs);
}

HIMAGELIST CMabinogiPackageToolView::GetShellImageList(BOOL bLarge)
{
	TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory(szWinDir, MAX_PATH) == 0)
	{
		return NULL;
	}

	SHFILEINFO sfi;
	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo(szWinDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX |(bLarge ? 0 : SHGFI_SMALLICON));
	return hImageList;
}

void CMabinogiPackageToolView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	::SetWindowTheme(GetListCtrl().GetSafeHwnd(), L"Explorer", 0);

	TCHAR szCurDir [MAX_PATH + 1];
	if (GetCurrentDirectory(MAX_PATH, szCurDir) > 0)
	{
		SHFILEINFO sfi;
		GetListCtrl().SetImageList(CImageList::FromHandle((HIMAGELIST) SHGetFileInfo(
			szCurDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)), 0);
	}

	GetListCtrl().SetImageList(CImageList::FromHandle(GetShellImageList(TRUE)), LVSIL_NORMAL);
	GetListCtrl().SetImageList(CImageList::FromHandle(GetShellImageList(FALSE)), LVSIL_SMALL);

	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	int index = 0;
	GetListCtrl().InsertColumn(index++, TEXT("文件名"), 0 , 150);
	GetListCtrl().InsertColumn(index++, TEXT("版本"), 0 , 100);
	GetListCtrl().InsertColumn(index++, TEXT("类型"), 0 , 100);
	GetListCtrl().InsertColumn(index++, TEXT("大小"), 0 , 100);
	GetListCtrl().InsertColumn(index++, TEXT("压缩后大小"), 0 , 100);
	GetListCtrl().InsertColumn(index++, TEXT("创建日期"), 0 , 100);
	GetListCtrl().InsertColumn(index++, TEXT("最后写入日期"), 0 , 100);
	GetListCtrl().InsertColumn(index++, TEXT("最后访问日期"), 0 , 100);
}

void CMabinogiPackageToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMabinogiPackageToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMabinogiPackageToolView 诊断

#ifdef _DEBUG
void CMabinogiPackageToolView::AssertValid() const
{
	CListView::AssertValid();
}

void CMabinogiPackageToolView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMabinogiPackageToolDoc* CMabinogiPackageToolView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMabinogiPackageToolDoc)));
	return (CMabinogiPackageToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMabinogiPackageToolView 消息处理程序
void CMabinogiPackageToolView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: 添加代码以响应用户对窗口视图样式的更改	
	CListView::OnStyleChanged(nStyleType,lpStyleStruct);	
}

int CALLBACK lambda_OnUpdate(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pListCtrl = &((CMabinogiPackageToolView*) lParamSort)->GetListCtrl();
	CString    strItem1 = pListCtrl->GetItemText(static_cast<int>(lParam1), 0);
	CString    strItem2 = pListCtrl->GetItemText(static_cast<int>(lParam2), 0);

	return strItem1.CompareNoCase(strItem2);
}

void CMabinogiPackageToolView::OnUpdate(CView* pSender, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	if (pSender != this)
	{
		GetListCtrl().LockWindowUpdate();

		GetListCtrl().DeleteAllItems();

		CPackFolder * pFolder = GetDocument()->m_pSelectedFolder;
		if ( pFolder != 0)
		{
			for (size_t i = 0;i < pFolder->m_entries.size();i++)
			{
				shared_ptr<CPackEntry> spFile = pFolder->m_entries.at(i);
				SHFILEINFO shFilefo;
				SHGetFileInfo( spFile->m_strName ,FILE_ATTRIBUTE_NORMAL , &shFilefo, sizeof(shFilefo),
					SHGFI_TYPENAME|SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX );
				
				int nItem = GetListCtrl().InsertItem(0, spFile->m_strName, shFilefo.iIcon);
				int index = 1;

				

				TCHAR szBuffer[20];
#ifdef _UNICODE
				_itow_s(spFile->GetEntry()->seed, szBuffer, 20, 10);
#else
				_itoa_s(spFile->GetEntry()->seed, szBuffer, 20, 10);
#endif
				GetListCtrl().SetItemText(nItem, index++, szBuffer);

				GetListCtrl().SetItemText(nItem, index++, shFilefo.szTypeName);

				// 原始大小
				GetListCtrl().SetItemText(nItem, index++, GetFileSizeText(spFile->GetEntry()->decompress_size));
				
				// 压缩后大小
				GetListCtrl().SetItemText(nItem, index++, GetFileSizeText(spFile->GetEntry()->compress_size));

				GetListCtrl().SetItemText(nItem, index++, GetFileTimeText(& spFile->GetEntry()->ft[2]));
				GetListCtrl().SetItemText(nItem, index++, GetFileTimeText(& spFile->GetEntry()->ft[0]));
				GetListCtrl().SetItemText(nItem, index++, GetFileTimeText(& spFile->GetEntry()->ft[4]));

				GetListCtrl().SetItemData(nItem, (DWORD_PTR)spFile.get());
			}
		}

		//GetListCtrl().SortItemsEx([](LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) -> int CALLBACK 
		//	{
		//		CListCtrl* pListCtrl = &((CMabinogiPackageToolView*) lParamSort)->GetListCtrl();
		//		CString    strItem1 = pListCtrl->GetItemText(static_cast<int>(lParam1), 0);
		//		CString    strItem2 = pListCtrl->GetItemText(static_cast<int>(lParam2), 0);

		//		return strItem1.CompareNoCase(strItem2);
		//	}, (DWORD_PTR)this);

		GetListCtrl().SortItemsEx(lambda_OnUpdate, (DWORD_PTR)this);

		GetListCtrl().UnlockWindowUpdate();
	}
}


//void CMabinogiPackageToolView::OnCancelMode()
//{
//	CListView::OnCancelMode();
//
//	// TODO: 在此处添加消息处理程序代码
//}


void CMabinogiPackageToolView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (GetListCtrl().GetSelectedCount() == 1)
	{
		// 只有在选中一个的时候更新预览内容
		if ( (pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
		{
			CMainFrame *pFrame = reinterpret_cast<CMainFrame*>(theApp.GetMainWnd());
			CPreviewPane &previewPane = pFrame->GetPreviewPane();
			if (previewPane.IsVisible())
			{
				CPackEntry *pEntry = (CPackEntry*)GetListCtrl().GetItemData(pNMLV->iItem);

				if (pEntry->IsTextContent())
				{
					shared_ptr<vector<byte> > spData = pEntry->GetData();
					spData->push_back(0);
					spData->push_back(0);

					if (IsTextUnicode(&*spData->begin(), spData->size() - 2, NULL))
					{
						USES_CONVERSION;
						CString temp =  CW2CT((LPCWSTR)&*spData->begin()) ;
						previewPane.SetTextContent(temp);
					}
					else
					{
						USES_CONVERSION;
						CString temp =  CA2CT((LPCSTR)&*spData->begin()) ;
						previewPane.SetTextContent(temp);
					}
				}
			}
		}
	}

	*pResult = 0;
}


CString CMabinogiPackageToolView::GetFileSizeText(unsigned long size)
{
	CString result;
	if (size > 1024 * 1024 * 1024)
	{
		result.Format(TEXT("%.2f GB"), (float)size / (float)(1024 * 1024 * 1024));
	}
	else if (size > 1024 * 1024)
	{
		result.Format(TEXT("%.2f MB"), (float)size / (float)(1024 * 1024));
	}
	else if (size > 1024)
	{
		result.Format(TEXT("%.2f KB"), (float)size / (float)1024);
	}
	else
	{
		result.Format(TEXT("%d 字节"), size);
	}
	return result;
}


CString CMabinogiPackageToolView::GetFileTimeText(FILETIME* pFileTime)
{
	CString result;
	CTime theTime(*pFileTime);
	return theTime.Format(TEXT("%Y/%m/%d %H:%M:%S"));
}


void CMabinogiPackageToolView::OnEditView()
{
	auto pos = GetListCtrl().GetFirstSelectedItemPosition();
	if (pos == nullptr)
	{
		// 没有选中则不处理
	}
	else
	{
		USES_CONVERSION;

		int nItem = GetListCtrl().GetNextSelectedItem(pos);
		CPackEntry* pEntry = (CPackEntry*) GetListCtrl().GetItemData(nItem);

		CString fullName = CA2T(pEntry->GetEntry()->name);
		fullName.Replace(TEXT("\\"), TEXT("_"));

		CString tempFileName = theApp.GetMyTempPath(); // 最后的 '\' 应该是已经有了的
		tempFileName += theApp.GetMyTempFilePrefix();
		tempFileName += fullName;

		pEntry->WriteToFile(tempFileName);

		if ((DWORD)::ShellExecute(0, TEXT("open"), tempFileName, 0, 0, SW_SHOW) <= 32)
		{
			CString strCmd;
			strCmd.Format(TEXT("shell32, OpenAs_RunDLL \"%s\""), tempFileName);
			::ShellExecute(0, 0, TEXT("rundll32"), strCmd, 0, SW_SHOW);
		};
	}
}


void CMabinogiPackageToolView::OnEditViewAs()
{
	auto pos = GetListCtrl().GetFirstSelectedItemPosition();
	if (pos == nullptr)
	{
		// 没有选中则不处理
	}
	else
	{
		USES_CONVERSION;

		int nItem = GetListCtrl().GetNextSelectedItem(pos);
		CPackEntry* pEntry = (CPackEntry*) GetListCtrl().GetItemData(nItem);

		CString fullName = CA2T(pEntry->GetEntry()->name);
		fullName.Replace(TEXT("\\"), TEXT("_"));

		CString tempFileName = theApp.GetMyTempPath(); // 最后的 '\' 应该是已经有了的
		tempFileName += theApp.GetMyTempFilePrefix();
		tempFileName += fullName;

		pEntry->WriteToFile(tempFileName);

		CString strCmd;
		strCmd.Format(TEXT("shell32, OpenAs_RunDLL \"%s\""), tempFileName);
		::ShellExecute(0, 0, TEXT("rundll32"), strCmd, 0, SW_SHOW);
	}
}


void CMabinogiPackageToolView::OnEditExtractTo()
{
	auto pos = GetListCtrl().GetFirstSelectedItemPosition();
	if (pos == nullptr)
	{
		// 没有选中则不处理
	}
	else
	{
		vector<CPackEntry*> entries;

		while (pos)
		{
			int nItem = GetListCtrl().GetNextSelectedItem(pos);
			CPackEntry* pEntry = (CPackEntry*) GetListCtrl().GetItemData(nItem);
			entries.push_back(pEntry);
		}

		// 这里需要区分单个选中和多个选中
		if (entries.size() == 1)
		{
			CFileDialog dlg(FALSE, NULL, (*entries.begin())->m_strName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);
			if( dlg.DoModal() == IDOK )
			{
				(*entries.begin())->WriteToFile(dlg.GetPathName());
			}
		}
		else
		{
			CString strOutput;
			if (theApp.GetShellManager()->BrowseForFolder(strOutput, this, 0, TEXT("选择一个解压文件夹"), 
				BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_NEWDIALOGSTYLE|BIF_USENEWUI) != FALSE)
			{
				for (auto iter = entries.begin();iter != entries.end();++iter)
				{
					(*iter)->WriteToFile(strOutput + TEXT("\\") + (*iter)->m_strName);
				}
			}
		}
	}
	
}


void CMabinogiPackageToolView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	OnEditView();
	*pResult = 0;
}
