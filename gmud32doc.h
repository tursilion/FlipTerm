/////////////////////////////////////////////////////////////////////////////
//
// Mud32doc.h : interface of the CMudDoc class
//

class CWorld;

#include "selarray.h"

class CMudDoc : public CDocument
{
	public:
		CMudDoc();
		virtual ~CMudDoc();
		DECLARE_DYNCREATE(CMudDoc)

	// Attributes
	public:
		int EditWorldList();
		int EditTriggerList(CWorld *pWorld);
		int AddWorld(CWorld *pWorld);
		void Save();
		void Load();

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMudDoc)
	public:
		virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL
public:
		int EditAliasList(CWorld *pWorld);
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif
	
		CObArray m_aWorlds;
		CArrayArray m_aTriggers;
		CArrayArray m_aMacros;
		CArrayArray m_aAliases;

	// Generated message map functions
	protected:
		//{{AFX_MSG(CMudDoc)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	void OnCloseDocument();
};

/////////////////////////////////////////////////////////////////////////////
