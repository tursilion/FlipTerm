// generic object base class for item SelArray from a list.

#include "stdafx.h"
#include "resource.h"
#include "SelArray.h"

CSelArray::CSelArray()
{

}

CSelArray::~CSelArray()
{
	for(int loop=0;loop<m_aChildren.GetSize();loop++)
		delete m_aChildren[loop];
}

CSelectable *CSelArray::Add(CSelectable *pSelectable)
{
	m_aChildren.Add(pSelectable);
	return pSelectable;
}

BOOL CSelArray::Delete(LPCSTR pName)
{
	for(int loop=0;loop<m_aChildren.GetSize();loop++)
	{
		if(Get(loop)->GetName().CompareNoCase(pName)==0)
		{
			delete Get(loop);
			m_aChildren.RemoveAt(loop);
			return TRUE;
		}
	}
	return FALSE;
}

CSelectable *CSelArray::Get(LPCSTR pName)
{
	CString sName;
	
	for(int loop=0;loop<m_aChildren.GetSize();loop++)
	{
 		sName = Get(loop)->GetName();
		if(sName.CompareNoCase(pName)==0)
			return Get(loop);
	}

	if(strcmpi(pName,"Default")!=0) {
		return NULL;
	}

	return Get("Default");
}

CSelectable *CSelArray::Get(int nPos)
{
	ASSERT(nPos<m_aChildren.GetSize());
	return (CSelectable *)m_aChildren.GetAt(nPos);
}

bool CSelArray::Exists(LPCSTR pName)
{
	CString sName;
	
	for(int loop=0;loop<m_aChildren.GetSize();loop++)
	{
 		sName = Get(loop)->GetName();
		if(sName.CompareNoCase(pName)==0)
			return true;
	}

	return false;
}

int CSelArray::GetSize()
{
	return m_aChildren.GetSize();
}
