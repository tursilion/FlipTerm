
#ifndef MacroARRAYDEFINED
#define MacroARRAYDEFINED

#include "selarray.h"


class CMacroArray : public  CSelArray
{
	virtual CSelectable *CreateCopy();
public:
	void Load(int num);
	void Save(int num);
};

#endif // MacroARRAYDEFINED
