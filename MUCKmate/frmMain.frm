VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmMain 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "MUCKmate"
   ClientHeight    =   2445
   ClientLeft      =   8145
   ClientTop       =   1365
   ClientWidth     =   6975
   BeginProperty Font 
      Name            =   "System"
      Size            =   9.75
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "frmMain"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   Picture         =   "frmMain.frx":030A
   ScaleHeight     =   2445
   ScaleWidth      =   6975
   Begin MSComDlg.CommonDialog cdlSaveFile 
      Left            =   7080
      Top             =   1320
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.TextBox txtData 
      BackColor       =   &H00000000&
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   345
      Left            =   0
      Locked          =   -1  'True
      TabIndex        =   2
      Top             =   2520
      Width           =   6975
   End
   Begin MSWinsockLib.Winsock wscHttp 
      Left            =   7080
      Top             =   240
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
   End
   Begin MSComctlLib.ProgressBar prgFile 
      Height          =   375
      Left            =   0
      TabIndex        =   1
      Top             =   2040
      Width           =   6975
      _ExtentX        =   12303
      _ExtentY        =   661
      _Version        =   393216
      Appearance      =   1
   End
   Begin MSWinsockLib.Winsock socket 
      Left            =   7080
      Top             =   840
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
   End
   Begin VB.Frame frameConsole 
      Caption         =   "Console"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   1935
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   6975
      Begin VB.TextBox txtConsole 
         BackColor       =   &H00000000&
         BorderStyle     =   0  'None
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   9.75
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00FFFFFF&
         Height          =   975
         Left            =   120
         Locked          =   -1  'True
         MultiLine       =   -1  'True
         ScrollBars      =   2  'Vertical
         TabIndex        =   5
         Top             =   240
         Width           =   6735
      End
      Begin VB.Frame Frame1 
         Caption         =   "Server State"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   495
         Left            =   120
         TabIndex        =   3
         Top             =   1320
         Width           =   6495
         Begin VB.Label lblServerstate 
            BeginProperty Font 
               Name            =   "MS Sans Serif"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Height          =   255
            Left            =   240
            TabIndex        =   4
            Top             =   210
            UseMnemonic     =   0   'False
            Width           =   5775
         End
      End
   End
   Begin VB.Menu mnuFile 
      Caption         =   "File"
      Begin VB.Menu mnuFileOptions 
         Caption         =   "Options"
      End
      Begin VB.Menu mnuFileSaveText 
         Caption         =   "Save Console Text"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "Exit"
      End
   End
   Begin VB.Menu mnuServer 
      Caption         =   "Server"
      Begin VB.Menu mnuServerStart 
         Caption         =   "Start"
      End
      Begin VB.Menu mnuServerShutdown 
         Caption         =   "Shutdown"
      End
   End
   Begin VB.Menu mnuWindow 
      Caption         =   "Window"
      Begin VB.Menu mnuSystray 
         Caption         =   "Minimize to System Tray"
      End
      Begin VB.Menu mnuWindowAlwaysOnTop 
         Caption         =   "Always on Top"
      End
      Begin VB.Menu mnuServerShowData 
         Caption         =   "Show Dataline"
      End
   End
   Begin VB.Menu mnuPopUp 
      Caption         =   "PopUp"
      Visible         =   0   'False
      Begin VB.Menu mnuPopUpOptions 
         Caption         =   "Options"
      End
      Begin VB.Menu mnuPopUpStart 
         Caption         =   "Start Server"
      End
      Begin VB.Menu mnuPopUpShutdown 
         Caption         =   "Shutdown Server"
      End
      Begin VB.Menu mnuPopUpShow 
         Caption         =   "Show Window"
      End
      Begin VB.Menu mnuPopUpHide 
         Caption         =   "Hide Window"
      End
      Begin VB.Menu mnuPopUpExit 
         Caption         =   "Exit"
      End
   End
   Begin VB.Menu mnuAbout 
      Caption         =   "About"
      Begin VB.Menu mnuAboutVersion 
         Caption         =   "Version and Build Information"
      End
      Begin VB.Menu mnuAboutWebsite 
         Caption         =   "Visit MUCKmate website"
      End
      Begin VB.Menu mnuAboutAuthor 
         Caption         =   "About the Author"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' All code copyright Charles LeFave 2004 unless otherwise
' stated in headers.


Private m_strHttpResponse   As String
Private m_strHttpRequest    As String
Private m_strHttpHeader     As String
Private m_strFileName       As String
Private m_strArgs()         As String

Private m_bResponseReceived As Boolean  'flag variable
Private m_bHeaderReceived   As Boolean  'flag variable

Private m_lContentLength    As Long     'value of "Content-Length" HTTP header
Private m_lDownloadedBytes  As Long

Dim Dock As New clsDockingHandler


' m_strArgs(0) - Command
' m_strArgs(1) - Server
' m_strArgs(2) - File Location
' m_strArgs(3) - Port
' m_strArgs(4) - File Name


Public Function getFile()

Dim intPort As Integer

If IsNumeric(m_strArgs(3)) Then
    intPort = CInt(m_strArgs(3))
End If

With wscHttp
    .Close
    .Connect m_strArgs(1), intPort
End With

txtConsole.Text = txtConsole.Text & "[!] Connecting to HTTP server " _
    & m_strArgs(1) & " port " & m_strArgs(3) & "." & vbCrLf


End Function

Private Sub Form_Load()

Dim strDataLine As String
Dim strOnTop As String

With prgFile
    .Value = 0.01
    .Scrolling = ccScrollingSmooth
End With

strDataLine = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\data", "String")
strOnTop = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\ontop", "String")

If strDataLine = "on" Then
    frmMain.Height = 3525
    mnuServerShowData.Checked = True
End If

If strOnTop = "on" Then
    Dock.AlwaysOnTop = True
    mnuWindowAlwaysOnTop.Checked = True
End If

mnuServerShutdown.Enabled = False
mnuPopUpShutdown.Enabled = False

mnuPopUpShow.Enabled = False

If ShellTrayAdd = 1 Then
    SubClass frmMain.hWnd
End If

Set Dock.ParentForm = Me
Dock.SnapDistance = 250

End Sub


Private Sub Form_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)

If Button = vbLeftButton Then
        Dock.StartDockDrag X, Y
End If

End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)

If Button = vbLeftButton Then
    Dock.UpdateDockDrag X, Y
End If

End Sub

Private Sub Form_Unload(Cancel As Integer)

socket.Close

frmOptions.CloseWin
frmImageview.CloseWin
frmAbout.CloseWin

Set frmOptions = Nothing
Set frmImageview = Nothing
Set frmAbout = Nothing

ShellTrayRemove

UnSubClass

Cancel = False

End Sub

Private Sub mnuAboutVersion_Click()
frmAbout.Show
End Sub

Private Sub mnuFileExit_Click()
Call PostMessage(frmMain.hWnd, WM_CLOSE, 0&, ByVal 0&)
End Sub

Private Sub mnuFileOptions_Click()
frmOptions.Show
End Sub

Private Sub mnuFileSaveText_Click()
With cdlSaveFile
    .DialogTitle = "Save.."
    .DefaultExt = ".txt"
    .Filter = "Plain Text File|*.txt"
    .InitDir = "logs"
    .ShowSave
End With

Dim intFileNum As Integer

intFileNum = FreeFile

If cdlSaveFile.FileName <> "" Then
    Open cdlSaveFile.FileName For Output As #intFileNum
        Print #intFileNum, txtConsole.Text
    Close #intFileNum
End If
End Sub

Private Sub mnuPopUpExit_Click()

Call PostMessage(frmMain.hWnd, WM_CLOSE, 0&, ByVal 0&)

End Sub

Private Sub mnuPopUpHide_Click()
frmMain.Hide
mnuPopUpHide.Enabled = False
mnuPopUpShow.Enabled = True
End Sub

Private Sub mnuPopUpOptions_Click()
frmOptions.Show
End Sub

Private Sub mnuPopUpShow_Click()
frmMain.Show
mnuPopUpHide.Enabled = True
mnuPopUpShow.Enabled = False
txtConsole.SelStart = Len(txtConsole.Text)
End Sub

Private Sub mnuPopUpShutdown_Click()
mnuServerShutdown_Click
End Sub

Private Sub mnuPopUpStart_Click()
mnuServerStart_Click
End Sub


Private Sub mnuServerShowData_Click()

If mnuServerShowData.Checked = False Then
    frmMain.Height = 3525
    mnuServerShowData.Checked = True
    Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\data", "String", "on")
Else
    frmMain.Height = 3100
    mnuServerShowData.Checked = False
    Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\data", "String", "off")
End If

End Sub

Private Sub mnuServerShutdown_Click()
txtConsole.Text = txtConsole.Text & "[>] Server shutting down." & vbCrLf
socket.Close
socket.LocalPort = 0
txtConsole.Text = txtConsole.Text & "[>] Server successfully shut down." & vbCrLf
mnuServerShutdown.Enabled = False
mnuPopUpShutdown.Enabled = False
mnuServerStart.Enabled = True
mnuPopUpStart.Enabled = True
End Sub

Private Sub mnuServerStart_Click()

Dim strExe As String

strExe = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\exe", _
    "String")

If strExe <> "" Then
    socket.LocalPort = 10698
    txtConsole.Text = txtConsole.Text & "[>] Starting up server at " _
        & socket.LocalHostName & ":" & socket.LocalPort & vbCrLf
    socket.Listen
    txtConsole.Text = txtConsole.Text _
        & "[>] Server initialized successfully." & vbCrLf
    mnuServerStart.Enabled = False
    mnuPopUpStart.Enabled = False
    mnuServerShutdown.Enabled = True
    mnuPopUpShutdown.Enabled = True
Else
    txtConsole.Text = txtConsole.Text & "[>] Unable to initialize server. " _
        & "Please make sure you have visited the menu in File -> Options " _
        & "and have set the field that identifies the location of the " _
        & "MUCKmate.exe Executable File." & vbCrLf
    mnuServerStart.Enabled = True
    mnuPopUpStart.Enabled = True
    mnuServerShutdown.Enabled = False
    mnuPopUpShutdown.Enabled = False
End If


End Sub

Private Sub mnuSystray_Click()
frmMain.Hide
mnuPopUpShow.Enabled = True
mnuPopUpHide.Enabled = False
End Sub

Private Sub mnuWindowAlwaysOnTop_Click()

If mnuWindowAlwaysOnTop.Checked = False Then
    mnuWindowAlwaysOnTop.Checked = True
    Dock.AlwaysOnTop = True
    Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\ontop", "String", "on")
Else
    mnuWindowAlwaysOnTop.Checked = False
    Dock.AlwaysOnTop = False
    Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\ontop", "String", "off")
End If

End Sub

Private Sub socket_Close()

txtConsole.Text = txtConsole.Text & "[>] Connection closed from: " _
    & socket.RemoteHostIP & vbCrLf
    
socket.Close
socket.LocalPort = 10698
socket.Listen

End Sub

Private Sub socket_ConnectionRequest(ByVal requestID As Long)

On Error Resume Next

If socket.State <> sckClosed Then socket.Close

socket.Accept requestID

txtConsole.Text = txtConsole.Text & "[>] Connection accepted from: " _
    & socket.RemoteHostIP & vbCrLf

End Sub

Private Sub socket_DataArrival(ByVal bytesTotal As Long)

Dim strdata As String
Dim strTmp As String

Dim strExe As String
Dim strFile As String
Dim strApp As String

Dim lngShell As Long



socket.GetData strdata, vbString

strTmp = Replace(strdata, Chr(10), "")
strdata = strTmp

txtData.Text = strdata

m_strArgs = Split(strdata)

If UBound(m_strArgs) > -1 Then
    Select Case m_strArgs(0)
        Case Is = "IMAGE"
    
        txtConsole.Text = txtConsole.Text & "[!] Received IMAGE command." & vbCrLf
        getFile
    
        Case Is = "MUSIC"
    
        txtConsole.Text = txtConsole.Text & "[!] Received MUSIC command." & vbCrLf
        
        strExe = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\exe", _
            "String")
            
        strExe = Replace(strExe, "MUCKmate.exe", "")
        strExe = Replace(strExe, "MUCKmate.EXE", "")
            
        If Dir(strExe & "data\music\" & m_strArgs(4) & ".mp3") = "" Then
            getFile
        Else
            txtConsole.Text = txtConsole.Text & "[!] File " & m_strArgs(4) _
                & ".mp3 found! Now playing." & vbCrLf
                
 Debug.Print "About to execute " & strFile
            strApp = getstring(HKEY_CURRENT_USER, _
                "Software\MUCKmate\musicapp", "String")
            
            strFile = strApp & " " & Chr(34) & strExe & "data\music\" _
                & m_strArgs(4) & ".mp3" & Chr(34)
 
            lngShell = Shell(strFile, vbMaximizedFocus)
            
            prgFile.Value = 0
            lblServerstate.Caption = ""
            
        End If

    End Select
End If

End Sub



Private Sub txtConsole_Change()
txtConsole.SelStart = Len(txtConsole.Text)

End Sub

Private Sub wscHttp_Close()

txtConsole.Text = txtConsole.Text & "[!] Connection closed from " _
    & wscHttp.RemoteHost & " port " & wscHttp.RemotePort & "." & vbCrLf

wscHttp.Close
wscHttp.RemoteHost = ""
wscHttp.RemotePort = 0

m_strHttpHeader = Left(m_strHttpResponse, _
    InStr(1, m_strHttpResponse, _
    vbCrLf & vbCrLf) + 1)
    
If m_strArgs(0) = "IMAGE" Then

    If Left$(GetHttpHeaderFieldValue(m_strHttpHeader, _
        "Content-Type"), Len("image/")) <> "image/" Then

        txtConsole.Text = txtConsole.Text _
            & "[!] Error: File corrupt or not found on server. " _
            & "The server may be down, or the file may no longer exit " _
            & "on this server. Please contact the player or room/object " _
            & "owner for more information." & vbCrLf
    
        m_strHttpResponse = ""
        m_strHttpHeader = ""
        m_lDownloadedBytes = 0
        m_lContentLength = 0
        m_bResponseReceived = False
        m_bHeaderReceived = False
    
        Exit Sub
    End If

End If

If m_lDownloadedBytes >= (m_lContentLength - 2000) Then

    Dim intFileNum As Integer
    
    Dim strFileExt As String
    Dim strSubDir  As String
    
    m_strHttpResponse = Mid(m_strHttpResponse, _
        InStr(1, m_strHttpResponse, _
        vbCrLf & vbCrLf) + 4)
        
    txtConsole.Text = txtConsole.Text & "[!] File " _
    & m_strArgs(4)
    
    If m_strArgs(0) = "IMAGE" Then
        txtConsole.Text = txtConsole.Text & ".jpg"
        strFileExt = ".jpg"
        strSubDir = "data\images\"
    ElseIf m_strArgs(0) = "MUSIC" Then
        txtConsole.Text = txtConsole.Text & ".mp3"
        strFileExt = ".mp3"
        strSubDir = "data\music\"
    End If
    
    txtConsole.Text = txtConsole.Text & " successfully downloaded" _
    & " at " & m_lDownloadedBytes & " bytes." & vbCrLf
    
    intFileNum = FreeFile
    
    Open strSubDir & m_strArgs(4) & strFileExt For Binary As #intFileNum
        Put #intFileNum, , m_strHttpResponse
    Close #intFileNum

    Dim strApp As String
    Dim strExe As String
    Dim strFile As String
    Dim lngShell As Long
    
    If m_strArgs(0) = "IMAGE" Then

        strApp = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\imageapp", _
            "String")

        If strApp = "built-in" Or strApp = "" Then
    
            frmImageview.pctImage.Picture = LoadPicture("data\images\" _
                & m_strArgs(4) & strFileExt)
            frmImageview.Show
    
        Else
        
            strExe = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\exe", _
                "String")
                
            strExe = Replace(strExe, "MUCKmate.exe", "")
            strExe = Replace(strExe, "MUCKmate.EXE", "")

            lngShell = Shell(strApp & " " & strExe & strSubDir & m_strArgs(4) _
                & strFileExt, vbMaximizedFocus)
    
        End If
    End If
    
    If m_strArgs(0) = "MUSIC" Then

        strApp = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\musicapp", _
            "String")
            
        If strApp = "" Then
        
            lblServerstate.Caption = "Unable to play file. No media player " _
                & "specified."
        
        Else

        
            strExe = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\exe", _
                "String")
                
            strExe = Replace(strExe, "MUCKmate.exe", "")
            strExe = Replace(strExe, "MUCKmate.EXE", "")
            
            strFile = strApp & " " & Chr(34) & strExe & strSubDir _
                & m_strArgs(4) & strFileExt & Chr(34)


            lngShell = Shell(strFile, vbMaximizedFocus)
            
        End If
  
    End If
Else

txtConsole.Text = txtConsole.Text & "[!] Download Failed. Please try again." _
    & vbCrLf

End If

m_strHttpResponse = ""
m_strHttpHeader = ""
m_lDownloadedBytes = 0
m_lContentLength = 0
m_bResponseReceived = False
m_bHeaderReceived = False


End Sub

Private Sub wscHttp_Connect()


strHttpRequest = "GET " & m_strArgs(2) & " HTTP/1.1" & vbCrLf _
                & "Host: " & wscHttp.RemoteHost & vbCrLf _
                & "Accept: */*" & vbCrLf _
                & "Connection: close" & vbCrLf _
                & vbCrLf
                
wscHttp.SendData strHttpRequest

prgFile.Value = 0
lblServerstate.Caption = "Sending request.."
txtConsole.Text = txtConsole.Text & "[!] Requesting file from HTTP server." _
    & vbCrLf
    
                
End Sub

Private Sub wscHttp_DataArrival(ByVal bytesTotal As Long)
    On Error Resume Next
    
    Dim strdata As String
    Dim intBreakePosition As Integer
    Dim vHeaders As Variant
    Dim vHeader As Variant
    
    lblServerstate.Caption = "Receiving response.."

    wscHttp.GetData strdata, vbString
 
    m_strHttpResponse = m_strHttpResponse & strdata
    
    m_lDownloadedBytes = m_lDownloadedBytes + bytesTotal
    
    If Not m_bHeaderReceived Then
        intBreakePosition = InStr(1, m_strHttpResponse, vbCrLf & vbCrLf)
        If intBreakePosition Then
            m_bHeaderReceived = True
            m_lDownloadedBytes = m_lDownloadedBytes - intBreakePosition - 3
            vHeaders = Split(Left(m_strHttpResponse, intBreakePosition - 1), vbCrLf)
            For Each vHeader In vHeaders
                If InStr(1, vHeader, "Content-Length") Then
                    m_lContentLength = CLng(Mid(vHeader, InStr(1, vHeader, " ") + 1))
                    Exit For
                End If
            Next
        End If
     Else
         If m_lContentLength > 0 Then
            prgFile.Value = m_lDownloadedBytes / (m_lContentLength / 100)
            lblServerstate.Caption = "Downloaded " & m_lDownloadedBytes & _
                " of " & m_lContentLength & _
                " (" & CInt(m_lDownloadedBytes / (m_lContentLength / 100)) & "%)" & _
                " bytes"
        End If
    End If
    
End Sub

Public Function ShellTrayAdd() As Long

 'prepare the NOTIFYICONDATA type with the
 'required parameters:
 
 '.cbSize: Size of this structure, in bytes.
 '
 '.hwnd:   Handle of the window that will receive
 '         notification messages associated with
 '         an icon in the taskbar status area.
 '
 'uID:     Application-defined identifier of
 '         the taskbar icon. In an application
 '         with a single tray icon, this can be
 '         an arbitrary number.  For apps with
 '         multiple icons, each icon ID must be
 '         different as this member identifies
 '         which of the icons was selected.
 '
 '.uFlags: flags that indicate which of the other
 '         members contain valid data. This member
 '         can be a combination of the following:
 '         NIF_ICON    hIcon member is valid.
 '         NIF_MESSAGE uCallbackMessage member is valid.
 '         NIF_TIP     szTip member is valid.
 '
 'uCallbackMessage: Application-defined message identifier.
 '         The system uses this identifier for
 '         notification messages that it sends
 '         to the window identified in hWnd.
 '         These notifications are sent when a
 '         mouse event occurs in the bounding
 '         rectangle of the icon. (Note: 'callback'
 '         is a bit misused here (in the context of
 '         other callback demonstrations); there is
 '         no systray-specific callback defined -
 '         instead the form itself must be subclassed
 '         to respond to this message.
 '
 'hIcon:   Handle to the icon to add, modify, or delete.
 '
 'szTip:   Tooltip text to display for the icon. Must
 '         be terminated with a Chr$(0).
 
 'Shell_NotifyIcon messages:
 'dwMessage: Message value to send. This parameter
 '           can be one of these values:
 '           NIM_ADD     Adds icon to status area
 '           NIM_DELETE  Deletes icon from status area
 '           NIM_MODIFY  Modifies icon in status area
 '
 'pnid:      Address of the prepared NOTIFYICONDATA.
 '           The content of the structure depends
 '           on the value of dwMessage.

   If NOTIFYICONDATA_SIZE = 0 Then SetShellVersion
    
   With NID
      .cbSize = NOTIFYICONDATA_SIZE
      .hWnd = frmMain.hWnd
      .uID = 125&
      .uFlags = NIF_ICON Or NIF_TIP Or NIF_MESSAGE
      .uCallbackMessage = WM_MYHOOK
      .hIcon = Me.Icon
      .szTip = "MUCKmate Image and Music server." & Chr$(0)
    End With
   
    ShellTrayAdd = Shell_NotifyIcon(NIM_ADD, NID)

End Function


Private Sub ShellTrayRemove()

  'Remove the icon from the taskbar
   Call Shell_NotifyIcon(NIM_DELETE, NID)
   
End Sub


Private Sub UnSubClass()

  'restore the default message handling
  'before exiting
   If defWindowProc Then
      SetWindowLong frmMain.hWnd, GWL_WNDPROC, defWindowProc
      defWindowProc = 0
   End If
   
End Sub


Private Sub SubClass(hWnd As Long)

  'assign our own window message
  'procedure (WindowProc)
  
   On Error Resume Next
   defWindowProc = SetWindowLong(hWnd, GWL_WNDPROC, AddressOf WindowProc)
   
End Sub

Private Sub wscHttp_Error(ByVal Number As Integer, Description As String, ByVal Scode As Long, ByVal Source As String, ByVal HelpFile As String, ByVal HelpContext As Long, CancelDisplay As Boolean)

txtConsole.Text = txtConsole.Text & "[!] Error occured while " _
& "connecting to HTTP server: " & Description & "." & vbCrLf
End Sub

Private Function GetHttpHeaderFieldValue(strHttpHeader As String, _
    strHttpHeaderField As String) As String
   
   Dim strBuffer As String
   Dim intStart As Integer
   Dim strSearchString As String
   
   strSearchString = vbCrLf & strHttpHeaderField & ": "
   
   intStart = InStr(1, strHttpHeader, strSearchString) + Len(strSearchString)
   strBuffer = Mid$(strHttpHeader, intStart, InStr(intStart, _
               strHttpHeader, vbCrLf) - intStart)
   
   If Len(strBuffer) > 0 Then
      GetHttpHeaderFieldValue = strBuffer
   End If
   
End Function


