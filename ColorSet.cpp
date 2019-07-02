// ColorSet.cpp : implementation file
//

#include "stdafx.h"
#include "gmud32.h"
#include "ColorSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern unsigned long colormap[];
extern unsigned long defcolormap[];

/////////////////////////////////////////////////////////////////////////////
// CColorSet dialog


CColorSet::CColorSet(CWnd* pParent /*=NULL*/)
	: CDialog(CColorSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorSet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CColorSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorSet)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorSet, CDialog)
	//{{AFX_MSG_MAP(CColorSet)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSet message handlers

void CColorSet::OnDefault() 
{
	if (IDYES != AfxMessageBox("Are you sure you'd like to reset the colors to default?", MB_ICONQUESTION | MB_YESNO)) {
		return;
	}

	memcpy(m_colormap, defcolormap, sizeof(unsigned long)*16);

	Invalidate();
}

BOOL CColorSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	memcpy(m_colormap, colormap, sizeof(unsigned long)*16);

	return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorSet::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// Draw the custom buttons
	int n;
	CDC myDC;
	COLORREF col1, col2;

	n=nIDCtl-IDC_BUTTON1;

	// May not be elegant, but it works.

	if ((n>=0)&&(n<16)) {
		// If drawing selected, add the pushed style to DrawFrameControl.
		if (lpDrawItemStruct->itemState & ODS_SELECTED) {
			col1=GetSysColor(COLOR_3DSHADOW);
			col2=GetSysColor(COLOR_3DLIGHT);
		} else {
			col1=GetSysColor(COLOR_3DLIGHT);
			col2=GetSysColor(COLOR_3DSHADOW);
		}

		// Fill it in
		HBRUSH hBr;
		hBr=::CreateSolidBrush(m_colormap[n]);
		::FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hBr);
		::DeleteObject(hBr);

		// Draw the button frame.
		myDC.Attach(lpDrawItemStruct->hDC);
		myDC.Draw3dRect(&lpDrawItemStruct->rcItem, col1, col2);
		lpDrawItemStruct->rcItem.bottom--;
		lpDrawItemStruct->rcItem.top++;
		lpDrawItemStruct->rcItem.left++;
		lpDrawItemStruct->rcItem.right--;
		myDC.Draw3dRect(&lpDrawItemStruct->rcItem, col1, col2);
		myDC.Detach();
	
		return;
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL CColorSet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int n;
	COLORREF custcol[16];

	n=wParam-IDC_BUTTON1;
	if ((n>=0)&&(n<16)) {
		// It's a color button
		CHOOSECOLOR col;

		memcpy(custcol, m_colormap, sizeof(unsigned long)*16);

		col.lStructSize=sizeof(col);
		col.hwndOwner=GetSafeHwnd();
		col.rgbResult=m_colormap[n];
		col.lpCustColors=custcol;
		col.Flags=CC_ANYCOLOR | CC_RGBINIT | CC_SOLIDCOLOR | CC_FULLOPEN;
		
		if (ChooseColor(&col)) {
			m_colormap[n]=col.rgbResult;
			Invalidate();
		}
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

void CColorSet::OnOK() 
{
	memcpy(colormap, m_colormap, sizeof(unsigned long)*16);
	
	CDialog::OnOK();
}
