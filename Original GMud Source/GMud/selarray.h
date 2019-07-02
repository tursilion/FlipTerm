// generic object base class for item SelArray from a list.

#ifndef SelArray_DEFINED
#define SelArray_DEFINED

#include "selectable.h"


class CSelArray : public CSelectable
{
	public:
	//initialization
		CSelArray();
		~CSelArray();

	// functions
		void Serialize(CArchive &ar);
		DECLARE_SERIAL(CSelArray);
	// get / set
		CSelectable *CSelArray::Add(CSelectable *pSelectable);
		BOOL CSelArray::Delete(LPCSTR pName);
		CSelectable *CSelArray::Get(LPCSTR pName);
		CSelectable *CSelArray::Get(int nPos);
		int GetSize();
	private:
	// member variables
		CObArray m_aChildren;
		int m_iType;

};

class CArrayArray : public CSelArray
{
		virtual CSelectable *CreateCopy(){ASSERT(0);return NULL;}
};

#endif // SelArray_DEFINED
