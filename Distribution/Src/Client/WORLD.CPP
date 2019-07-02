
#include "stdafx.h"
#include "world.h"
#include "gmud32.h"
#include "outwnd.h"
#include "gmud32vw.h"

#define WVERCUR	4

CWorld::CWorld()
{
	m_sTriggerList= "Default";
	m_sMacroList= "Default";
	m_sAliasList = "Default";
	m_sDescription = "";
	m_sName="";
	m_sHostName="";
	m_wPort=23;
	m_sConnectString="";
	m_sAutologFile="";
	m_sLastAutologFilename="";
	m_nLastAutoFileNumber=0;
	m_nLastDayLogOpened=0;
	m_bReconnect=0;
	m_bAutoConnect=0;
	m_bIs7Bit=1;
	m_bIsIRC=0;

	strcpy(m_LogFont.lfFaceName, "Default");	// use program defaults
	m_colorBackGround=-1;				
	m_colorForeGround=-1;
	m_bAnsi=-2;
}

CWorld::~CWorld()
{

}

void CWorld::Save(int num) {
	char szPath[1024];
	POSITION pos;
	CString Key, Val;
	int cnt;
	CString cTmp;

	sprintf(szPath, "Worlds\\%d", num);
	GetApp()->WriteProfileString(szPath, "Name", m_sName);

	GetApp()->WriteProfileString(szPath, "Host", m_sHostName);
	GetApp()->WriteProfileInt(szPath, "Port", m_wPort);
	cTmp=m_sConnectString;
	if (cTmp.GetLength() > 0) {
		for (int idx=0; idx<cTmp.GetLength(); idx++) {
			cTmp.SetAt(idx, (char)(127-cTmp.GetAt(idx)));
		}
		cTmp="}~{" + cTmp;
	}
	GetApp()->WriteProfileString(szPath, "ConnectString", cTmp);

	GetApp()->WriteProfileString(szPath, "AliasList", m_sAliasList);
	GetApp()->WriteProfileString(szPath, "Description", m_sDescription);
	GetApp()->WriteProfileString(szPath, "TriggerList", m_sTriggerList);
	GetApp()->WriteProfileString(szPath, "MacroList", m_sMacroList);
	GetApp()->WriteProfileString(szPath, "AutologFile", m_sAutologFile);
	GetApp()->WriteProfileString(szPath, "LastAutologFilename", m_sLastAutologFilename);
	GetApp()->WriteProfileInt(szPath, "AutologMode", m_nAutologMode);
	GetApp()->WriteProfileInt(szPath, "LastAutoFileNumber", m_nLastAutoFileNumber);
	GetApp()->WriteProfileInt(szPath, "LastDayLogOpened", m_nLastDayLogOpened);
	GetApp()->WriteProfileInt(szPath, "AutoReconnect", m_bReconnect);
	GetApp()->WriteProfileInt(szPath, "AutoConnect", m_bAutoConnect);
	GetApp()->WriteProfileInt(szPath, "Is7Bit", m_bIs7Bit);
	GetApp()->WriteProfileInt(szPath, "IsIRC", m_bIsIRC);

	GetApp()->WriteProfileString(szPath, "FontName", m_LogFont.lfFaceName);
	GetApp()->WriteProfileInt(szPath, "FontSize", m_LogFont.lfHeight);
	GetApp()->WriteProfileInt(szPath, "FontWeight", m_LogFont.lfWeight);
	GetApp()->WriteProfileInt(szPath, "FontItalic", m_LogFont.lfItalic);

	GetApp()->WriteProfileInt(szPath, "BackColor", m_colorBackGround);
	GetApp()->WriteProfileInt(szPath, "ForeColor", m_colorForeGround);
	
	// Save the user variables
	pos=m_UserVariables.GetStartPosition();
	if (NULL != pos) {
		sprintf(szPath, "Worlds\\%d\\Variables", m_sName);
		GetApp()->WriteProfileInt(szPath, "Count", m_UserVariables.GetCount());
	
		if (m_UserVariables.GetCount() > 0) {
			cnt=0;
			while (NULL != pos) {
				m_UserVariables.GetNextAssoc(pos, Key, Val);
				if (NULL != pos) {
					sprintf(szPath, "Worlds\\%d\\Variables\\%d", m_sName, cnt);
					cnt++;
					GetApp()->WriteProfileString(szPath, "Name", Key);
					GetApp()->WriteProfileString(szPath, "Value", Val);
				}
			}
		}
	}
}

void CWorld::Load(int num) {
	char szPath[1024];
	CString Key, Val;
	int cnt, loop;

	sprintf(szPath, "Worlds\\%d", num);
	m_sName=GetApp()->GetProfileString(szPath, "Name", "Default");

	m_sHostName=GetApp()->GetProfileString(szPath, "Host", "Unknown");
	m_wPort=(unsigned short)GetApp()->GetProfileInt(szPath, "Port", 23);
	m_sConnectString=GetApp()->GetProfileString(szPath, "ConnectString", "");
	
	if (m_sConnectString.Left(3) == "}~{") {
		if (m_sConnectString.GetLength()>3) {
			m_sConnectString = m_sConnectString.Mid(3);
			for (int idx=0; idx<m_sConnectString.GetLength(); idx++) {
				m_sConnectString.SetAt(idx, (char)(127-m_sConnectString.GetAt(idx)));
			}
		} else {
			m_sConnectString="";
		}
	}

	m_sAliasList=GetApp()->GetProfileString(szPath, "AliasList", "Default");
	m_sDescription=GetApp()->GetProfileString(szPath, "Description", "");
	m_sTriggerList=GetApp()->GetProfileString(szPath, "TriggerList", "Default");
	m_sMacroList=GetApp()->GetProfileString(szPath, "MacroList", "Default");
	m_sAutologFile=GetApp()->GetProfileString(szPath, "AutologFile", "");
	m_nAutologMode=GetApp()->GetProfileInt(szPath, "AutologMode", 0);
	m_nLastAutoFileNumber=GetApp()->GetProfileInt(szPath, "LastAutoFileNumber", 0);
	m_sLastAutologFilename=GetApp()->GetProfileString(szPath, "LastAutologFilename", "");
	m_nLastDayLogOpened=GetApp()->GetProfileInt(szPath, "LastDayLogOpened", 0);
	m_bReconnect=GetApp()->GetProfileInt(szPath, "AutoReconnect", 0);
	m_bAutoConnect=GetApp()->GetProfileInt(szPath, "AutoConnect", 0);
	m_bIs7Bit=GetApp()->GetProfileInt(szPath, "Is7Bit", 1);
	m_bIsIRC=GetApp()->GetProfileInt(szPath, "IsIRC", 0);

	memset(&m_LogFont,0,sizeof(m_LogFont));
	strcpy(m_LogFont.lfFaceName, GetApp()->GetProfileString(szPath, "FontName", "Default"));
	m_LogFont.lfHeight=GetApp()->GetProfileInt(szPath, "FontSize", -12);
	m_LogFont.lfWeight=GetApp()->GetProfileInt(szPath, "FontWeight", 0);
	m_LogFont.lfItalic=GetApp()->GetProfileInt(szPath, "FontItalic", 0) ? true:false;
	
	m_colorBackGround=GetApp()->GetProfileInt(szPath, "BackColor", -1);
	m_colorForeGround=GetApp()->GetProfileInt(szPath, "ForeColor", -1);

	// Load the user variables
	sprintf(szPath, "Worlds\\%d\\Variables", num);
	cnt=GetApp()->GetProfileInt(szPath, "Count", 0);
	
	for (loop=0; loop<cnt; loop++) {
		sprintf(szPath, "Worlds\\%d\\Variables\\%d", num, loop);
		Key=GetApp()->GetProfileString(szPath, "Name", "");
		Val=GetApp()->GetProfileString(szPath, "Value", "");
		if ((Key!="")&&(Val!="")) {
			m_UserVariables.SetAt(Key, Val);
		}
	}
}

CMapStringToString  * CWorld::GetUserVariables()
{
  return &m_UserVariables;
}
