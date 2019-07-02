// generic object base class for item selectable from a list.

#include "stdafx.h"
#include "Selectable.h"

IMPLEMENT_SERIAL(CSelectable,CObject,1|VERSIONABLE_SCHEMA);

CSelectable::CSelectable()
{

}

CSelectable::~CSelectable()
{

}

void CSelectable::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	if(ar.IsStoring())
	{
		ar << m_sName;
	}
	else
	{
		ar >> m_sName;
	}
}
