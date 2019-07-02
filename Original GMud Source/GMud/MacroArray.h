
#ifndef MacroARRAYDEFINED
#define MacroARRAYDEFINED

#include "selarray.h"


class CMacroArray : public  CSelArray
{
		virtual CSelectable *CreateCopy();
		DECLARE_SERIAL(CMacroArray);
		void Serialize(CArchive &ar);
};

#endif // MacroARRAYDEFINED
