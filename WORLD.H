
#ifndef WORLD_DEFINED
#define WORLD_DEFINED

class CWorld : public CObject
{
	public:
		CMapStringToString  * GetUserVariables();
		CWorld() ;
		~CWorld();
		
		void Load(int num);
		void Save(int num);
        void setFingerprint(const char *);

		CString m_sName;
		CString m_sHostName;
		WORD m_wPort;
        bool m_useSSL;
		CString m_sConnectString;

		CString m_sDescription;
		CString m_sTriggerList;
		CString m_sMacroList;
		CString m_sAliasList;
		CString m_sAutologFile;
		CString m_sLastAutologFilename;
        CString m_sslFingerprint;
		CString m_csIRCChannel;		// not persistent
		CString m_csNick;			// not persistent
		int	m_nAutologMode;
		int m_nLastAutoFileNumber;
		int m_nLastDayLogOpened;
		int m_bReconnect;
		int m_bAutoConnect;
		int m_bIs7Bit;
		int m_bIsIRC;

		LOGFONT m_LogFont;
		int m_colorBackGround;
		int m_colorForeGround;
		int m_bAnsi;		// Note: -2 indicates default - not *really* a bool. Just for code simplicity.

	protected:
		CMapStringToString  m_UserVariables;
};

#endif // WORLD_DEFINED
