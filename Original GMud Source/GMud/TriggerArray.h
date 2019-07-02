
#ifndef TRIGGERARRAYDEFINED
#define TRIGGERARRAYDEFINED

#include "selarray.h"


class CTriggerArray : public  CSelArray
{
		virtual CSelectable *CreateCopy();
		DECLARE_SERIAL(CTriggerArray);
		void Serialize(CArchive &ar);
};

#endif // TRIGGERARRAYDEFINED
