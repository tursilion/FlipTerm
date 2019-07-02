Attribute VB_Name = "DockHandler"
Option Explicit
 
Public Type RECT
        Left As Long
        Top As Long
        Right As Long
        Bottom As Long
End Type
 
Public Const HWND_TOPMOST = -1
Public Const HWND_NOTOPMOST = -2
Public Const SPI_GETWORKAREA = 48
 
Public Declare Function SetWindowPos Lib "user32" (ByVal hwnd As Long, _
       ByVal hWndInsertAfter As Long, ByVal X As Long, ByVal Y As Long, _
       ByVal cx As Long, ByVal cy As Long, ByVal wFlags As Long) As Long
Public Declare Function SystemParametersInfo Lib "user32" Alias "SystemParametersInfoA" _
       (ByVal uAction As Long, ByVal uParam As Long, _
        ByRef lpvParam As Any, ByVal fuWinIni As Long) As Long
Public Declare Function FlashWindow Lib "user32" (ByVal hwnd As Long, _
        ByVal bInvert As Long) As Long

