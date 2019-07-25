// generic object base class for item selectable from a list.

#include "stdafx.h"
#include "Macro.h"

CMacro::CMacro()
{

}

CMacro::~CMacro()
{

}

CSelectable *CMacro::CreateCopy()
{
	CMacro *pMac = new CMacro;
	pMac->SetName(GetName());
	pMac->m_sText = m_sText;
	return pMac;
}


int CMacro::OnActivate(CMudView &rView, CWorld &rWorld)
{
  m_sText.TrimLeft();
  m_sText.TrimRight();
  rView.OnUserInput(0,(long)(LPCSTR)m_sText);
  return true;
}