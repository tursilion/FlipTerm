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
File "D:\work\gmud\Distribution\FlipTerm.exe"
File "D:\work\gmud\Distribution\FlipTerm.txt"
File "D:\work\gmud\Distribution\source_code.txt"
File "D:\work\gmud\Distribution\OpenSSL_Readme.txt"
CreateDirectory "$STARTMENU\Programs\FlipTerm"
CreateShortcut "$STARTMENU\Programs\FlipTerm\FlipTerm.lnk" "$INSTDIR\FlipTerm.exe"
CreateShortcut "$STARTMENU\Programs\FlipTerm\Notes.lnk" "$INSTDIR\FlipTerm.txt"
SectionEnd ; end of default section

Section "Old GMUD Documentation (optional)"
SectionIn 2
SetOutPath "$INSTDIR"
File /r "D:\work\gmud\distribution\old gmud docs"
CreateShortcut "$STARTMENU\Programs\FlipTerm\Old GMud Docs.lnk" "$INSTDIR\old gmud docs"
SectionEnd

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
