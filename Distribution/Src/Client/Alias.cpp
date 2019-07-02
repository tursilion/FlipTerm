// Alias.cpp: implementation of the CAlias class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmud32.h"
#include "Alias.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

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
