#if !defined(AFX_COLORSET_H__2DD9BE62_3D56_4594_A7E0_EF1A82CA2CAC__INCLUDED_)
#define AFX_COLORSET_H__2DD9BE62_3D56_4594_A7E0_EF1A82CA2CAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorSet dialog

class CColorSet : public CDialog
{
// Construction
public:
	CColorSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorSet)
	enum { IDD = IDD_COLORS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	unsigned long m_colormap[16];
	CBitmap bmp[16];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorSet)
	afx_msg void OnDefault();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSET_H__2DD9BE62_3D56_4594_A7E0_EF1A82CA2CAC__INCLUDED_)
