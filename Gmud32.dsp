# Microsoft Developer Studio Project File - Name="Gmud32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Gmud32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Gmud32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gmud32.mak" CFG="Gmud32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Gmud32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Gmud32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Gmud32", CAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Gmud32 - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /G4 /MTd /W4 /Gm /GX /ZI /Od /Oy /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib shfolder.lib /nologo /debug /machine:I386 /out:"WinDebug/FlipTerm.exe" /SUBSYSTEM:windows,4.0.
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Gmud32 - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W4 /Gm /GX /Zi /Ox /Ot /Oa /Ow /Og /Oi /Ob2 /Gf /Gy /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fr /FD /c
# SUBTRACT CPP /Os /YX /Yc /Yu
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib shfolder.lib /nologo /incremental:yes /debug /machine:I386 /out:"WinRel/FlipTerm.exe" /SUBSYSTEM:windows,4.0.
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Gmud32 - Win32 Debug"
# Name "Gmud32 - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Alias.cpp
# End Source File
# Begin Source File

SOURCE=.\AliasArray.cpp
# End Source File
# Begin Source File

SOURCE=.\AliasDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AliasEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\BUTTONBA.CPP
# End Source File
# Begin Source File

SOURCE=.\buttoned.cpp
# End Source File
# Begin Source File

SOURCE=.\CHILDFRM.CPP
# End Source File
# Begin Source File

SOURCE=.\COLORCOM.CPP
# End Source File
# Begin Source File

SOURCE=.\ColorSet.cpp
# End Source File
# Begin Source File

SOURCE=.\conbar.cpp
# End Source File
# Begin Source File

SOURCE=.\connectd.cpp
# End Source File
# Begin Source File

SOURCE=.\CSmartSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Disconnect.cpp
# End Source File
# Begin Source File

SOURCE=.\gmud32.cpp
# End Source File
# Begin Source File

SOURCE=.\gmud32doc.cpp
# End Source File
# Begin Source File

SOURCE=.\gmud32vw.cpp
# End Source File
# Begin Source File

SOURCE=.\INWND.CPP
# End Source File
# Begin Source File

SOURCE=.\LOWLEV.CPP
# End Source File
# Begin Source File

SOURCE=.\Macro.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroArray.cpp
# End Source File
# Begin Source File

SOURCE=.\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\namedial.cpp
# End Source File
# Begin Source File

SOURCE=.\outwnd.cpp

!IF  "$(CFG)" == "Gmud32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "Gmud32 - Win32 Release"

# ADD CPP /Ot
# SUBTRACT CPP /Os /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\renamedi.cpp
# End Source File
# Begin Source File

SOURCE=.\replaced.cpp
# End Source File
# Begin Source File

SOURCE=.\Script.cpp
# End Source File
# Begin Source File

SOURCE=.\Script.h
# End Source File
# Begin Source File

SOURCE=.\selarray.cpp
# End Source File
# Begin Source File

SOURCE=.\selarraydlg.cpp
# End Source File
# Begin Source File

SOURCE=.\selectable.cpp
# End Source File
# Begin Source File

SOURCE=.\soundsdi.cpp
# End Source File
# Begin Source File

SOURCE=.\STATUSWN.CPP
# End Source File
# Begin Source File

SOURCE=.\STDAFX.CPP
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Switches.cpp
# End Source File
# Begin Source File

SOURCE=.\textclr.cpp
# End Source File
# Begin Source File

SOURCE=.\TEXTFRMT.CPP
# End Source File
# Begin Source File

SOURCE=.\TRIGGER.CPP
# End Source File
# Begin Source File

SOURCE=.\TriggerArray.cpp
# End Source File
# Begin Source File

SOURCE=.\TRIGGERD.CPP
# End Source File
# Begin Source File

SOURCE=.\WORLD.CPP
# End Source File
# Begin Source File

SOURCE=.\wrapopti.cpp
# End Source File
# End Group
# Begin Group "document files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\ANSI.TXT
# End Source File
# Begin Source File

SOURCE=".\Distribution\Docs\FlipTerm MCP Handling.txt"
# End Source File
# Begin Source File

SOURCE=".\Distribution\Docs\FlipTerm MCP Plugin Structure.txt"
# End Source File
# Begin Source File

SOURCE=.\Distribution\FlipTerm.txt
# End Source File
# Begin Source File

SOURCE=.\MANUAL.WRI
# End Source File
# Begin Source File

SOURCE=".\Distribution\Docs\MCP 2_1 Spec.htm"
# End Source File
# Begin Source File

SOURCE=".\Distribution\Docs\MuckMate MCP Wrapper.txt"
# End Source File
# Begin Source File

SOURCE=.\README.TXT
# End Source File
# Begin Source File

SOURCE=.\VERSION.TXT
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Alias.h
# End Source File
# Begin Source File

SOURCE=.\AliasArray.h
# End Source File
# Begin Source File

SOURCE=.\AliasDialog.h
# End Source File
# Begin Source File

SOURCE=.\AliasEdit.h
# End Source File
# Begin Source File

SOURCE=.\BUTTONBA.H
# End Source File
# Begin Source File

SOURCE=.\buttoned.h
# End Source File
# Begin Source File

SOURCE=.\CHILDFRM.H
# End Source File
# Begin Source File

SOURCE=.\COLORCOM.H
# End Source File
# Begin Source File

SOURCE=.\ColorSet.h
# End Source File
# Begin Source File

SOURCE=.\conbar.h
# End Source File
# Begin Source File

SOURCE=.\connectd.h
# End Source File
# Begin Source File

SOURCE=.\CSmartSocket.h
# End Source File
# Begin Source File

SOURCE=.\Disconnect.h
# End Source File
# Begin Source File

SOURCE=.\gmud32.h
# End Source File
# Begin Source File

SOURCE=.\gmud32doc.h
# End Source File
# Begin Source File

SOURCE=.\gmud32vw.h
# End Source File
# Begin Source File

SOURCE=.\INWND.H
# End Source File
# Begin Source File

SOURCE=.\LOWLEV.H
# End Source File
# Begin Source File

SOURCE=.\macro.h
# End Source File
# Begin Source File

SOURCE=.\MacroArray.h
# End Source File
# Begin Source File

SOURCE=.\mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\namedial.h
# End Source File
# Begin Source File

SOURCE=.\outwnd.h
# End Source File
# Begin Source File

SOURCE=.\renamedi.h
# End Source File
# Begin Source File

SOURCE=.\replaced.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\selarray.h
# End Source File
# Begin Source File

SOURCE=.\selarraydlg.h
# End Source File
# Begin Source File

SOURCE=.\selectable.h
# End Source File
# Begin Source File

SOURCE=.\soundsdi.h
# End Source File
# Begin Source File

SOURCE=.\STATUSWN.H
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# Begin Source File

SOURCE=.\Switches.h
# End Source File
# Begin Source File

SOURCE=.\textclr.h
# End Source File
# Begin Source File

SOURCE=.\TEXTFRMT.H
# End Source File
# Begin Source File

SOURCE=.\TRIGGER.H
# End Source File
# Begin Source File

SOURCE=.\TriggerArray.h
# End Source File
# Begin Source File

SOURCE=.\TRIGGERD.H
# End Source File
# Begin Source File

SOURCE=.\WORLD.H
# End Source File
# Begin Source File

SOURCE=.\wrapopti.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\GMUD32.RC
# End Source File
# Begin Source File

SOURCE=.\RES\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\RES\MUD32.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\MUD32.RC2
# End Source File
# Begin Source File

SOURCE=.\RES\Mud32doc.ico
# End Source File
# Begin Source File

SOURCE=.\RES\NumberBar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
