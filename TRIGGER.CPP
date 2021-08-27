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

bool match(LPCSTR &szOrigTest, LPCSTR szOrigPattern,CMapStringToString &params,bool caseSensitive);

CTrigger::CTrigger()
{
	m_sMudOutput="";
	m_sFileName="";
	m_fgColor=0;
	m_bCaseSensitive=false;
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
	pTrig->m_bCaseSensitive = m_bCaseSensitive;
	return pTrig;
}

// return 0 for none, 1 for processed, 2 to change string color
// str - pointer to input string from MUCK
// pView - pointer to the view object for variables and output
int CTrigger::CheckActivation(LPCSTR str, CMudView *pView)
{
	int ret;

	if ((NULL == str) || (NULL == pView)) {
		return 0;
	}
	
	if (0 == GetName().GetLength()) {	// if the trigger string is set...
		return 0;
	}
	
	ret=0;
	
	// TODO: frankly, I think I might delete the user variables - it would simplify
	// a lot of the code and I can't help but think it was meant for the incomplete
	// scripting, which I did delete. But for the moment, they can stay...
	while(str && *str && match(str, GetName(), *pView->GetUserVariables(), m_bCaseSensitive))
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

// szOrigTest - refernce to source string pointer (will be updated to the match point)
// szOrigPattern - pointer to match string
// params - reference to view's user variables
// caseSensitive - true to match the trigger name in a case-sensitive manner
bool match(LPCSTR &szOrigTest, LPCSTR szOrigPattern, CMapStringToString &params, bool caseSensitive)
{
	// verify input pointers have strings
	if((NULL == szOrigTest) || (0 == *szOrigTest)) {
		return false;
	}
	if ((NULL == szOrigPattern) || ( 0 == *szOrigPattern)) {
		return false;
	}

	// make some locals to work with
	LPCSTR szTest=szOrigTest;			// source string search
	LPCSTR szPattern=szOrigPattern;		// pattern string search
	LPCSTR szNonMatch=szTest;			// beginning of search stored when pattern is found
	bool bMatching=false;				// whether we are currently matching
	CMapStringToString aTempParams;		// I have no idea
	
	while(*szTest && *szPattern)
	{
		CString sVarName;

		if(*szPattern == '%')			// this pulls a %VARNAME from the string - I don't think I actually USE variables anymore...
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
			while(*szTest && (*szTest != *szPattern))	// var searches are always case-sensitive?
			{
				sVarValue+=*szTest;
				szTest++;
			}
			aTempParams.SetAt(sVarName,sVarValue);
			bMatching=true;
		} else if ((!caseSensitive)&&(toupper(*szTest) == toupper(*szPattern))) {
			bMatching=true;
			szTest++;
			szPattern++;
		} else if ((caseSensitive)&&(*szTest == *szPattern)) {
			bMatching=true;
			szTest++;
			szPattern++;
		} else {
			bMatching=false;
		}
		if(!bMatching)
		{
			szPattern=szOrigPattern;
			szNonMatch++;
			szTest=szNonMatch;
		}
	}
	if(*szPattern) {
		bMatching=false;
	}
	if(bMatching)
	{
		szOrigTest=szTest;
		for(POSITION pos = aTempParams.GetStartPosition(); pos != NULL; )
		{
			CString sKey,sVal;
			aTempParams.GetNextAssoc( pos, sKey, sVal);
			params.SetAt(sKey,sVal);
		}
	}
	return bMatching;
}
