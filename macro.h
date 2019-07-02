#ifndef MACRO_DEFINED
#define MACRO_DEFINED

#include "selectable.h"

class CMudView;
class CWorld;

class CMacro : public CSelectable
{
	public:
		CMacro::CMacro();
		CMacro::~CMacro();

	  // functions
		CSelectable *CreateCopy();
		int OnActivate(CMudView &rView, CWorld &rWorld);

	  // member variables
		CString m_sText;
};

#endif // MACRO_DEFINED
