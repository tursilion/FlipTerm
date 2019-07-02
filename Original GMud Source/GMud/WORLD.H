
#ifndef WORLD_DEFINED
#define WORLD_DEFINED

class CWorld : public CObject
{
	public:
		CMapStringToString  * GetUserVariables();
		CWorld() ;
		~CWorld();
		
		void Serialize(CArchive &ar);

		CString m_sName;
		CString m_sHostName;
		WORD m_wPort;
		CString m_sConnectString;

		CString m_sDescription;
		CString m_sTriggerList;
		CString m_sMacroList;
		CString m_sAliasList;

		DECLARE_SERIAL(CWorld);
protected:
	  CMapStringToString  m_UserVariables;
};

#endif // WORLD_DEFINED
