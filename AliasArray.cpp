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
#endif

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
	for(int loop=0;loop<GetSize();loop++) {
		pArray->Add(Get(loop)->CreateCopy());
	}
	pArray->SetName(GetName());
	return pArray;
}

void CAliasArray::Save(int num)
{
	char path[1024];
	CAlias *pAl;

	sprintf(path, "Aliases\\%d", num);
	GetApp()->WriteProfileString(path, "Name", GetName());
	GetApp()->WriteProfileInt(path, "Count", GetSize());

	for (int loop=0; loop<GetSize(); loop++) {
		sprintf(path, "Aliases\\%d\\%d", num, loop);
		pAl=(CAlias*)Get(loop);
		GetApp()->WriteProfileString(path, "Name", pAl->GetName());
		GetApp()->WriteProfileString(path, "Output", pAl->m_sAliasOutput);
	}
}

void CAliasArray::Load(int num)
{
	char path[1024];
	CAlias *pAl;
	int cnt;
	
	sprintf(path, "Aliases\\%d", num);
	SetName(GetApp()->GetProfileString(path, "Name", "Default"));
	cnt=GetApp()->GetProfileInt(path, "Count", 0);

	for (int loop=0; loop<cnt; loop++) {
		sprintf(path, "Aliases\\%d\\%d", num, loop);
		pAl=new CAlias;
		pAl->SetName(GetApp()->GetProfileString(path, "Name", "xx"));
		pAl->m_sAliasOutput=GetApp()->GetProfileString(path, "Output", "xx");
		Add(pAl);
	}
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
