// generic object base class for item selectable from a list.

#include "stdafx.h"
#include "Macro.h"
#include "Script.h"

IMPLEMENT_SERIAL(CMacro,CSelectable,1|VERSIONABLE_SCHEMA);

#define new DEBUG_NEW

CMacro::CMacro()
{

}

CMacro::~CMacro()
{

}

void CMacro::Serialize(CArchive &ar)
{
	CSelectable::Serialize(ar);
	if(ar.IsStoring())
	{
		ar << m_sText;
	}
	else
	{
		ar >> m_sText;
	}
}

CSelectable *CMacro::CreateCopy()
{
	CMacro *pMac = new CMacro;
	pMac->SetName(GetName());
	pMac->m_sText = m_sText;
	return pMac;
}


CMacro::OnActivate(CMudView &rView, CWorld &rWorld)
{
  m_sText.TrimLeft();
  m_sText.TrimRight();
  RunScript(m_sText,rView,rWorld);
  return true;
}