#include "stdafx.h"
#include "trigger.h"
#include "resource.h"
#include "mainfrm.h"
#include "mmsystem.h"
#include "gmud32.h"
#include "io.h"
#include "GMud32Vw.h"
#include "script.h"

IMPLEMENT_SERIAL(CTrigger,CSelectable,2|VERSIONABLE_SCHEMA);

bool match(LPCSTR *szOrigTest, LPCSTR szOrigPattern,CMapStringToString &params);

CTrigger::CTrigger()
{
;
}

CTrigger::~CTrigger()
{
;
}

void CTrigger::Serialize(CArchive &ar)
{
	WORD wSchema=2;
	if(!ar.IsStoring())
		wSchema = ar.GetObjectSchema();
	if(wSchema>1)
		CSelectable::Serialize(ar);
	if(ar.IsStoring())
	{
		ar << m_sMudOutput;
		ar << m_sFileName;
	}
	else
	{
		if(wSchema<2)
		{
			CString sTemp;
			ar >> sTemp;
			SetName(sTemp);
		}
		ar >> m_sMudOutput;
		ar >> m_sFileName;
	}
}

CSelectable *CTrigger::CreateCopy()
{
	CTrigger *pTrig = new CTrigger;
	pTrig->m_sMudOutput = m_sMudOutput;
	pTrig->m_sFileName = m_sFileName;
	pTrig->SetName(GetName());
	return pTrig;
}

BOOL CTrigger::CheckActivation(LPCSTR str, CMudView *pView)
{
	if(!GetName().GetLength())	// if the trigger string is set...
		return FALSE;
  
	while(str && *str && match(&str,GetName(),*pView->GetUserVariables()))
	{
    str+=GetName().GetLength();
		CMudApp *pApp = (CMudApp*)AfxGetApp();

    if(pApp->m_bTriggerSound && m_sFileName.GetLength() && _access(m_sFileName,0)==0)
			sndPlaySound(m_sFileName,SND_ASYNC);

    if(!m_sMudOutput.IsEmpty())
      RunScript((LPCSTR)(m_sMudOutput),*pView,*pView->m_pWorld);
	}
	return FALSE; // no trigger string set
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
    if(*szPattern == '%')
    {
      szPattern++;
      CString sVarName;
      while(*szPattern && !isspace(*szPattern))
      {
        sVarName+=*szPattern;
        szPattern++;
      }
      CString sVarValue;
      while(*szTest && !isspace(*szTest))
      {
        sVarValue+=*szTest;
        szTest++;
      }
      aTempParams.SetAt(sVarName,sVarValue);
      bMatching=true;
    }
    else if(*szTest == *szPattern)
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