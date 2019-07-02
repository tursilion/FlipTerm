// AliasArray.cpp: implementation of the CAliasArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmud32.h"
#include "AliasArray.h"
#include "Alias.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CAliasArray,CSelArray,1|VERSIONABLE_SCHEMA);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAliasArray::CAliasArray()
{

}

CAliasArray::~CAliasArray()
{

}

CSelectable *CAliasArray::CreateCopy()
{
	CAliasArray *pArray = new CAliasArray;
	for(int loop=0;loop<GetSize();loop++)
		pArray->Add(Get(loop)->CreateCopy());
	pArray->SetName(GetName());
	return pArray;
}

void CAliasArray::Serialize(CArchive &ar)
{
	CSelArray::Serialize(ar);
}

CString CAliasArray::CheckAliases(CString sInput)
{
	for(int loop=0;loop<GetSize();loop++)
	{
		CString sAlias = Get(loop)->GetName();
		if(strnicmp(sAlias,sInput,sAlias.GetLength())==0 && (sAlias.GetLength()==sInput.GetLength() || isspace(sInput[sAlias.GetLength()])))
		{
			sInput = ((CAlias *)Get(loop))->m_sAliasOutput+sInput.Mid(sAlias.GetLength());
			break;
		}
	}
	return sInput;
}
