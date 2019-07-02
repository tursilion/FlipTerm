// Alias.cpp: implementation of the CAlias class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmud32.h"
#include "Alias.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CAlias,CObject,1|VERSIONABLE_SCHEMA);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlias::CAlias()
{

}

CAlias::~CAlias()
{

}

CSelectable *CAlias::CreateCopy()
{
	CAlias *pAlias = new CAlias;
	pAlias->SetName(GetName());
	pAlias->m_sAliasOutput=m_sAliasOutput;
	return pAlias;
}

void CAlias::Serialize(CArchive &ar)
{
	CSelectable::Serialize(ar);
  if(ar.IsStoring())
  {
    ar << m_sAliasOutput;
  }
  else
  {
    ar >> m_sAliasOutput;
  }
}