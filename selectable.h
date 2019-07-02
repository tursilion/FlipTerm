// generic object base class for item selectable from a list.

#ifndef SELECTABLE_DEFINED
#define SELECTABLE_DEFINED

class CSelectable : public CObject
{
	public:
	//initialization
		CSelectable();
		~CSelectable();

	// pure virtuals
		virtual CSelectable *CreateCopy(){ASSERT(0);return NULL;}

	// get / set
		CString GetName(){return m_sName;}
		bool SetName(CString sName){m_sName=sName;return TRUE;}

	private:
	// member variables
		CString m_sName;

};

#endif // SELECTABLE_DEFINED
