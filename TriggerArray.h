
#ifndef TRIGGERARRAYDEFINED
#define TRIGGERARRAYDEFINED

#include "selarray.h"


class CTriggerArray : public  CSelArray
{
	virtual CSelectable *CreateCopy();
public:
	void Load(int num);
	void Save(int num);
};

#endif // TRIGGERARRAYDEFINED
