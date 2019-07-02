// Not much of a 'scripting' system anymore. We'll consider it in the future

#include "StdAFX.h"
#include "Script.h"
#include "outwnd.h"
#include "GMud32VW.h"
#include "World.h"

void RunScript(CString sScript,CMudView &rView,CWorld &/*rWorld*/)
{
  sScript.TrimLeft();
  rView.OnUserInput(0,(long)(LPCSTR)sScript);
  return;
}
