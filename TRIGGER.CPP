#include "stdafx.h"
#include <Windows.h>
#include "trigger.h"
#include "resource.h"
#include "mainfrm.h"
#include "mmsystem.h"
#include "gmud32.h"
#include "io.h"
#include "outwnd.h"
#include "GMud32Vw.h"

bool match(LPCSTR *szOrigTest, LPCSTR szOrigPattern,CMapStringToString &params);

CTrigger::CTrigger()
{
	m_sMudOutput="";
	m_sFileName="";
	m_fgColor=0;
}

CTrigger::~CTrigger()
{
	;
}

CSelectable *CTrigger::CreateCopy()
{
	CTrigger *pTrig = new CTrigger;
	pTrig->m_sMudOutput = m_sMudOutput;
	pTrig->m_sFileName = m_sFileName;
	pTrig->SetName(GetName());
	return pTrig;
}

// return 0 for none, 1 for processed, 2 to change string color
int CTrigger::CheckActivation(LPCSTR str, CMudView *pView)
{
	int ret;
	
	if (!GetName().GetLength())	// if the trigger string is set...
	{
		return 0;
	}
	
	ret=0;
	
	while(str && *str && match(&str,GetName(),*pView->GetUserVariables()))
	{
		str+=GetName().GetLength();
		CMudApp *pApp = GetApp();
		
		if(pApp->m_bTriggerSound && m_sFileName.GetLength() && _access(m_sFileName,0)==0) {
			sndPlaySound(m_sFileName,SND_ASYNC);
		}
		
		if(!m_sMudOutput.IsEmpty()) {
            pView->OnUserInput(0,(long)(LPCSTR)m_sMudOutput);
		}
		
		if (m_fgColor) {
			ret=2;
		}
	}
	
	return ret;
}


bool match(LPCSTR *szOrigTest, LPCSTR szOrigPattern,CMapStringToString &params)
{
	
	if(!*szOrigPattern)
		return false;
	LPCSTR szTest=*szOrigTest;
	LPCSTR szPattern=szOrigPattern;
	LPCSTR szNonMatch =szTest;
	bool bMatching=false;
	CMapStringToString aTempParams;
	
	while(*szTest && *szPattern)
	{
		CString sVarName;

		if(*szPattern == '%')
		{
			szPattern++;
			while(*szPattern && !isspace(*szPattern))
			{
				sVarName+=*szPattern;
				szPattern++;
			}
		}

		if (sVarName.GetLength()) {
			CString sVarValue;
			// Search for the first matching char or EOL
			while(*szTest && (*szTest != *szPattern))
			{
				sVarValue+=*szTest;
				szTest++;
			}
			aTempParams.SetAt(sVarName,sVarValue);
			bMatching=true;
		}
		else if(toupper(*szTest) == toupper(*szPattern))
		{
			bMatching=true;
			szTest++;
			szPattern++;
		}
		else 
			bMatching=false;
		if(!bMatching)
		{
			szPattern=szOrigPattern;
			szNonMatch++;
			szTest=szNonMatch;
		}
	}
	if(*szPattern)
		bMatching=false;
	if(bMatching)
	{
		*szOrigTest=szTest;
		for(POSITION pos = aTempParams.GetStartPosition(); pos != NULL; )
		{
			CString sKey,sVal;
			aTempParams.GetNextAssoc( pos, sKey, sVal);
			params.SetAt(sKey,sVal);
		}
	}
	return bMatching;
}
