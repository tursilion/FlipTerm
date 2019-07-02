// colorcom.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorCombo window

class CColorCombo : public CComboBox
{
// Construction
public:
	CColorCombo();

// Attributes
public:
	void AddColorItem(COLORREF color);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorCombo)
	public:
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorCombo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
