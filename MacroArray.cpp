#include "stdafx.h"
#include "gmud32.h"
#include "Macro.h"
#include "MacroArray.h"

CSelectable *CMacroArray::CreateCopy()
{
	CMacroArray *pArray = new CMacroArray;
	for(int loop=0;loop<GetSize();loop++) {
		pArray->Add(Get(loop)->CreateCopy());
	}
	pArray->SetName(GetName());
	return pArray;
}

void CMacroArray::Save(int num)
{
	char path[1024];
	CMacro *pMa;

	sprintf(path, "Macros\\%d", num);
	GetApp()->WriteProfileString(path, "Name", GetName());
	GetApp()->WriteProfileInt(path, "Count", GetSize());

	for (int loop=0; loop<GetSize(); loop++) {
		sprintf(path, "Macros\\%d\\%d", num, loop);
		pMa=(CMacro*)Get(loop);
		GetApp()->WriteProfileString(path, "Name", pMa->GetName());
		GetApp()->WriteProfileString(path, "Output", pMa->m_sText);
	}
}

void CMacroArray::Load(int num)
{
	char path[1024];
	CMacro *pMa;
	int cnt;
	
	sprintf(path, "Macros\\%d", num);
	SetName(GetApp()->GetProfileString(path, "Name", "Default"));
	cnt=GetApp()->GetProfileInt(path, "Count", 0);

	for (int loop=0; loop<cnt; loop++) {
		sprintf(path, "Macros\\%d\\%d", num, loop);
		pMa=new CMacro;
		pMa->SetName(GetApp()->GetProfileString(path, "Name", "xx"));
		pMa->m_sText=GetApp()->GetProfileString(path, "Output", "xx");
		Add(pMa);
	}
}

