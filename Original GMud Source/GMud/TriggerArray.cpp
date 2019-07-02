#include "stdafx.h"
#include "TriggerArray.h"

IMPLEMENT_SERIAL(CTriggerArray,CSelArray,1|VERSIONABLE_SCHEMA);


CSelectable *CTriggerArray::CreateCopy()
{
	CTriggerArray *pArray = new CTriggerArray;
	for(int loop=0;loop<GetSize();loop++)
		pArray->Add(Get(loop)->CreateCopy());
	pArray->SetName(GetName());
	return pArray;
}

void CTriggerArray::Serialize(CArchive &ar)
{
	CSelArray::Serialize(ar);
}
