#include "stdafx.h"
#include "gmud32.h"
#include "Trigger.h"
#include "TriggerArray.h"

CSelectable *CTriggerArray::CreateCopy()
{
	CTriggerArray *pArray = new CTriggerArray;
	for(int loop=0;loop<GetSize();loop++)
		pArray->Add(Get(loop)->CreateCopy());
	pArray->SetName(GetName());
	return pArray;
}

void CTriggerArray::Save(int num)
{
	char path[1024];
	CTrigger *pTr;

	sprintf(path, "Triggers\\%d", num);
	GetApp()->WriteProfileString(path, "Name", GetName());
	GetApp()->WriteProfileInt(path, "Count", GetSize());

	for (int loop=0; loop<GetSize(); loop++) {
		sprintf(path, "Triggers\\%d\\%d", num, loop);
		pTr=(CTrigger*)Get(loop);
		GetApp()->WriteProfileString(path, "Name", pTr->GetName());
		GetApp()->WriteProfileString(path, "Output", pTr->m_sMudOutput);
		GetApp()->WriteProfileString(path, "File", pTr->m_sFileName);
		GetApp()->WriteProfileInt(path, "fgColor", pTr->m_fgColor);
	}
}

void CTriggerArray::Load(int num)
{
	char path[1024];
	CTrigger *pTr;
	int cnt;
	
	sprintf(path, "Triggers\\%d", num);
	SetName(GetApp()->GetProfileString(path, "Name", "Default"));
	cnt=GetApp()->GetProfileInt(path, "Count", 0);

	for (int loop=0; loop<cnt; loop++) {
		sprintf(path, "Triggers\\%d\\%d", num, loop);
		pTr=new CTrigger;
		pTr->SetName(GetApp()->GetProfileString(path, "Name", ""));
		pTr->m_sMudOutput=GetApp()->GetProfileString(path, "Output", "");
		pTr->m_sFileName=GetApp()->GetProfileString(path, "File", "");
		pTr->m_fgColor=GetApp()->GetProfileInt(path, "fgColor", 0);
		Add(pTr);
	}
}
