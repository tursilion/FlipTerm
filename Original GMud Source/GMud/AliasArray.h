// AliasArray.h: interface for the CAliasArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALIASARRAY_H__5FA8CD41_A153_11D0_97DB_0000C037E924__INCLUDED_)
#define AFX_ALIASARRAY_H__5FA8CD41_A153_11D0_97DB_0000C037E924__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "selarray.h"

class CAliasArray : public CSelArray  
{
public:
	CString CheckAliases(CString sAliases);
	CAliasArray();
	virtual ~CAliasArray();

	CSelectable *CreateCopy();

	DECLARE_SERIAL(CAliasArray);
	virtual void Serialize(CArchive &ar);

};

#endif // !defined(AFX_ALIASARRAY_H__5FA8CD41_A153_11D0_97DB_0000C037E924__INCLUDED_)
