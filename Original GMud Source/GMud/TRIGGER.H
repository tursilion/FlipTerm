#ifndef TRIGGER_DEFINED
#define TRIGGER_DEFINED

#include "selectable.h"

class CMudView;

class CTrigger :public CSelectable
{
	public:
		CTrigger();
		~CTrigger();

		DECLARE_SERIAL(CTrigger);
		void Serialize(CArchive &ar);
		virtual CSelectable *CreateCopy();
		
		BOOL CheckActivation(LPCSTR str, CMudView *pView);
		CString m_sMudOutput;
		CString m_sFileName;
};

#endif // TRIGGER_DEFINED
