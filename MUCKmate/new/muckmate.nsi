; Installer script for MUCKMate
; 2/27/2004 by Karna
Name "MUCKMate"
OutFile "InstallMUCKMate-Full.exe"
InstallDir "$PROGRAMFILES\MUCKMate"
InstallDirRegKey HKEY_CURRENT_USER "SOFTWARE\KarnaSoft\MUCKMate" ""
DirText "Choose a location to install MUCKMate, then click 'Install' to continue. If you aren't sure, you can select the default."
InstType "Full"
ComponentText "This program will install MUCKMate to your machine."
Section "MUCKMate" ; 
SetOutPath "$INSTDIR" 
File "C:\Documents and Settings\Owner\Desktop\Projects\MUCKmate\Release\MUCKmate.exe"
File "C:\Documents and Settings\Owner\Desktop\Projects\MUCKmate\Release\ReadMe.txt"
File /r "C:\Documents and Settings\Owner\Desktop\Projects\MUCKmate\Release\muf"
MessageBox MB_YESNO "Would you like to install the Visual Basic dependency files? If you select yes, you can find these files in your MUCKMate /
dependencies folder. For more information, see the ReadMe file." IDNO skipdepend
File /r "C:\Documents and Settings\Owner\Desktop\Projects\MUCKmate\Release\dependencies"
skipdepend:
CreateDirectory "$INSTDIR\logs"
CreateDirectory "$INSTDIR\data\flash"
CreateDirectory "$INSTDIR\data\images"
CreateDirectory "$INSTDIR\data\music"
CreateDirectory "$STARTMENU\Programs\MUCKMate"
CreateShortcut "$STARTMENU\Programs\MUCKMate\MUCKMate.lnk" "$INSTDIR\MUCKMate.exe"
CreateShortcut "$STARTMENU\Programs\MUCKMate\ReadMe.lnk" "$INSTDIR\ReadMe.txt"
SectionEnd
Section "-post"
MessageBox MB_YESNO "Would you like a shortcut placed on the desktop?" IDNO skipdesktop
CreateShortcut "$DESKTOP\MUCKMate.lnk" "$INSTDIR\MUCKMate.exe"
skipdesktop:
WriteRegStr HKEY_CURRENT_USER "Software\MUCKMate\params" "exe" "$INSTDIR\MUCKMate.exe"
WriteRegStr HKEY_CURRENT_USER "Software\MUCKMate\params" "port" "10698"
WriteRegStr HKEY_CURRENT_USER "Software\MUCKMate\applications" "browser" "$PROGRAMFILES\Internet Explorer\IExplore.exe"
WriteRegStr HKEY_CURRENT_USER "Software\MUCKMate\applications" "flash" "$PROGRAMFILES\Internet Explorer\IExplore.exe"
SectionEnd
