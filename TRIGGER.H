#ifndef TRIGGER_DEFINED
#define TRIGGER_DEFINED

#include "selectable.h"

class CMudView;

class CTrigger :public CSelectable
{
	public:
		CTrigger();
		~CTrigger();

		virtual CSelectable *CreateCopy();
		int CheckActivation(LPCSTR str, CMudView *pView);
		
		CString m_sMudOutput;
		CString m_sFileName;
		int m_fgColor;
		bool m_bCaseSensitive;
};

#define ANTIIDLETRIGGERTEXT "~~IDLE~~ "
#define ANTIIDLETRIGGERLENGTH 9

#endif // TRIGGER_DEFINED
