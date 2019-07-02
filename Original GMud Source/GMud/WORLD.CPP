
#include "stdafx.h"
#include "world.h"

#define WVERCUR	4

IMPLEMENT_SERIAL(CWorld, CObject, WVERCUR|VERSIONABLE_SCHEMA);

CWorld::CWorld()
{
	m_sTriggerList= "Default";
	m_sMacroList= "Default";
	m_sAliasList = "Default";
	m_sDescription = "";
}

CWorld::~CWorld()
{

}

void CWorld::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

  if(ar.IsStoring())
	{
		ar << m_sName;
		ar << m_sHostName;
		ar << m_wPort;
		ar << m_sConnectString;

		ar << m_sAliasList;
		ar << m_sDescription;
		ar << m_sTriggerList;
		ar << m_sMacroList;

    m_UserVariables.Serialize(ar);
	}
	else
	{
		UINT wDocVer = ar.GetObjectSchema();

		ar >> m_sName;
		ar >> m_sHostName;
		ar >> m_wPort;
		ar >> m_sConnectString;

		if(wDocVer>2)
		{
			ar >> m_sAliasList;
		}
		if(wDocVer>1)
		{
			ar >> m_sDescription;
			ar >> m_sTriggerList;
			ar >> m_sMacroList;
		}
    if(wDocVer>3)
    {
      m_UserVariables.Serialize(ar);
    }
	}
}

CMapStringToString  * CWorld::GetUserVariables()
{
  return &m_UserVariables;
}
