#include "stdafx.h"
#include "MacroArray.h"

IMPLEMENT_SERIAL(CMacroArray,CSelArray,1|VERSIONABLE_SCHEMA);

CSelectable *CMacroArray::CreateCopy()
{
	CMacroArray *pArray = new CMacroArray;
	for(int loop=0;loop<GetSize();loop++)
		pArray->Add(Get(loop)->CreateCopy());
	pArray->SetName(GetName());
	return pArray;
}

void CMacroArray::Serialize(CArchive &ar)
{
	CSelArray::Serialize(ar);
}
