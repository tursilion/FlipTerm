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
		void Serialize(CArchive &ar);
		DECLARE_SERIAL(CMacro);
		CSelectable *CreateCopy();
    OnActivate(CMudView &rView, CWorld &rWorld);
	  // member variables
		CString m_sText;
};

#endif // MACRO_DEFINED
