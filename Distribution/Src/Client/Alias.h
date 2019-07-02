// Alias.h: interface for the CAlias class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALIAS_H__5FA8CD42_A153_11D0_97DB_0000C037E924__INCLUDED_)
#define AFX_ALIAS_H__5FA8CD42_A153_11D0_97DB_0000C037E924__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "selectable.h"

class CAlias : public CSelectable  
{
	public:
		CAlias();
		virtual ~CAlias();
		CSelectable *CreateCopy();

		CString m_sAliasOutput;

};

#endif // !defined(AFX_ALIAS_H__5FA8CD42_A153_11D0_97DB_0000C037E924__INCLUDED_)
