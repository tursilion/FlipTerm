; Installer script for FlipTerm
; 13 Aug 02 by Tursi
; Updated 31 Aug 06 for new NSIS

Name "FlipTerm"
OutFile "InstallFlipTerm.exe"

InstallDir "$PROGRAMFILES\FlipTerm"
InstallDirRegKey HKEY_CURRENT_USER "SOFTWARE\TursiSoft\FlipTerm" ""
DirText "Choose a location to install FlipTerm, then click 'Install' to continue. If you aren't sure, you can accept the default."

InstType "Typical"
InstType "Full"

ComponentText "This program will install the FlipTerm MUD client to your machine."

Section "FlipTerm Client" ; (default, required section)
SectionIn 1 2
SetOutPath "$INSTDIR"
File "C:\work\gmud\Distribution\FlipTerm.exe"
File "C:\work\gmud\Distribution\FlipTerm.txt"
File "C:\work\gmud\Distribution\SHFolder.dll"
CreateDirectory "$STARTMENU\Programs\FlipTerm"
CreateShortcut "$STARTMENU\Programs\FlipTerm\FlipTerm.lnk" "$INSTDIR\FlipTerm.exe"
CreateShortcut "$STARTMENU\Programs\FlipTerm\Notes.lnk" "$INSTDIR\FlipTerm.txt"
SectionEnd ; end of default section

Section "Plugins (recommended)"
SectionIn 1 2
SetOutPath "$INSTDIR"
File /r "C:\work\gmud\Distribution\Plugins"
; Initialize the 3D plugin here
WriteRegStr HKEY_CURRENT_USER "SOFTWARE\TursiSoft\FlipTerm\PlugIns\3DView" "Cache" "$INSTDIR\Plugins\3DGraphics"
WriteRegDWORD HKEY_CURRENT_USER "SOFTWARE\TursiSoft\FlipTerm\PlugIns\3DView" "Size" 1
CreateShortcut "$STARTMENU\Programs\FlipTerm\Plugins Folder.lnk" "$INSTDIR\Plugins"
SectionEnd ; end of section 'Plugins'

Section "Default Worlds (optional)"
SectionIn 1 2
; Just sets up default mucks in the registry - don't overwrite any others though
ClearErrors
;
ReadRegStr $0 HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "Name"
IfErrors 0 +9
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "Name" "FlipSide"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "AliasList" "Default"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "Description" "http://flipsidemuck.org"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "Host" "muck.flipsidemuck.org"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "MacroList" "Default"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "TriggerList" "Default"
WriteRegDWORD HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\0\" "Port" 9999
WriteRegDWORD HKCU "SOFTWARE\TursiSoft\FlipTerm" "WorldCount" 1
;
ReadRegStr $0 HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "Name"
IfErrors 0 +9
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "Name" "TLK Muck"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "AliasList" "Default"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "Description" "Unofficial Lion King Muck"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "Host" "muck.tlkmuck.org"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "MacroList" "Default"
WriteRegStr HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "TriggerList" "Default"
WriteRegDWORD HKCU "SOFTWARE\TursiSoft\FlipTerm\Worlds\1\" "Port" 7675
WriteRegDWORD HKCU "SOFTWARE\TursiSoft\FlipTerm" "WorldCount" 2
;
SectionEnd

Section "Documentation (optional)"
SectionIn 1 2
SetOutPath "$INSTDIR"
File /r "C:\work\gmud\distribution\old gmud docs"
File /r "C:\work\gmud\distribution\docs"
CreateShortcut "$STARTMENU\Programs\FlipTerm\Documentation.lnk" "$INSTDIR\Docs"
CreateShortcut "$STARTMENU\Programs\FlipTerm\Old GMud Docs.lnk" "$INSTDIR\old gmud docs"
SectionEnd

Section "Source Code (optional)"
SectionIn 2
SetOutPath "$INSTDIR"
File /r "C:\work\gmud\distribution\src"
CreateShortcut "$STARTMENU\Programs\FlipTerm\Source Code.lnk" "$INSTDIR\Src"
SectionEnd ; end of section 'Source Code'

Section "-post" ; (post install section, happens last after any optional sections)
; add any commands that need to happen after any optional sections here
WriteRegStr HKEY_CURRENT_USER "SOFTWARE\TursiSoft\FlipTerm" "" "$INSTDIR"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\FlipTerm" "DisplayName" "FlipTerm"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\FlipTerm" "UninstallString" '"$INSTDIR\uninst.exe"'
; write out uninstaller
WriteUninstaller "$INSTDIR\uninst.exe"
CreateShortcut "$STARTMENU\Programs\FlipTerm\Uninstall.lnk" "$INSTDIR\uninst.exe"
MessageBox MB_YESNO "Would you like a shortcut placed on the desktop?" IDNO skipdesktop
CreateShortcut "$DESKTOP\FlipTerm.lnk" "$INSTDIR\FlipTerm.exe"
skipdesktop:
SectionEnd ; end of -post section

; begin uninstall settings/section
UninstallText "This will uninstall FlipTerm (and all registry settings) from your system. Be warned that it will delete all files in the install folder and all World configuration."

Section Uninstall
; add delete commands to delete whatever files/registry keys/etc you installed here.
Delete "$INSTDIR\uninst.exe"
DeleteRegKey HKEY_CURRENT_USER "SOFTWARE\TursiSoft\FlipTerm"
DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\FlipTerm"
RMDir /r "$INSTDIR"
RMDir /r "$STARTMENU\Programs\FlipTerm"
Delete "$DESKTOP\FlipTerm.lnk"
SectionEnd ; end of uninstall section

; eof
