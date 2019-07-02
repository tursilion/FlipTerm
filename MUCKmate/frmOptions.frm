VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmOptions 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Options"
   ClientHeight    =   4635
   ClientLeft      =   10290
   ClientTop       =   4425
   ClientWidth     =   4815
   Icon            =   "frmOptions.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4635
   ScaleWidth      =   4815
   Begin VB.Frame Frame3 
      Caption         =   "Executable Location"
      Height          =   1215
      Left            =   0
      TabIndex        =   11
      Top             =   0
      Width           =   4815
      Begin VB.CommandButton cmdExeBrowse 
         Caption         =   "Browse"
         Height          =   285
         Left            =   3720
         TabIndex        =   13
         Top             =   720
         Width           =   975
      End
      Begin VB.TextBox txtExe 
         Height          =   285
         Left            =   120
         TabIndex        =   12
         Top             =   720
         Width           =   3495
      End
      Begin VB.Label Label2 
         Caption         =   "Before this program will work, you must click 'Browse' and select MUCKmate.exe from the menu, then press 'Apply'."
         Height          =   495
         Left            =   120
         TabIndex        =   14
         Top             =   240
         Width           =   4575
      End
   End
   Begin MSComDlg.CommonDialog cdlBrowse 
      Left            =   3960
      Top             =   4080
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton cmdApply 
      Caption         =   "Apply"
      Height          =   375
      Left            =   2520
      TabIndex        =   10
      Top             =   4200
      Width           =   1095
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   1200
      TabIndex        =   9
      Top             =   4200
      Width           =   1215
   End
   Begin VB.Frame Frame2 
      Caption         =   "Music"
      Height          =   1215
      Left            =   0
      TabIndex        =   5
      Top             =   2880
      Width           =   4815
      Begin VB.CommandButton cmdMusicBrowse 
         Caption         =   "Browse"
         Height          =   285
         Left            =   3720
         TabIndex        =   7
         Top             =   720
         Width           =   975
      End
      Begin VB.TextBox txtMusic 
         Height          =   285
         Left            =   120
         TabIndex        =   6
         Top             =   720
         Width           =   3495
      End
      Begin VB.Label Label3 
         Caption         =   "Select the program you want to launch MP3 files with, ie: winamp.exe"
         Height          =   375
         Left            =   120
         TabIndex        =   8
         Top             =   240
         Width           =   4575
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Images"
      Height          =   1455
      Left            =   0
      TabIndex        =   0
      Top             =   1320
      Width           =   4815
      Begin VB.CheckBox chkmmImage 
         Caption         =   "Use MUCKmate's built-in image viewer."
         Height          =   255
         Left            =   120
         TabIndex        =   4
         Top             =   1080
         Width           =   4575
      End
      Begin VB.CommandButton cmdImageBrowse 
         Caption         =   "Browse"
         Height          =   285
         Left            =   3720
         TabIndex        =   2
         Top             =   720
         Width           =   975
      End
      Begin VB.TextBox txtImages 
         Height          =   285
         Left            =   120
         TabIndex        =   1
         Top             =   720
         Width           =   3495
      End
      Begin VB.Label Label1 
         Caption         =   "Select the program you want to view images with, ie: A web browser such as iexplore.exe"
         Height          =   495
         Left            =   120
         TabIndex        =   3
         Top             =   240
         Width           =   4575
      End
   End
End
Attribute VB_Name = "frmOptions"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Public Sub CloseWin()

Form_Unload (1)

End Sub

Private Sub chkmmImage_Click()

If chkmmImage.Value = 1 Then
    txtImages.Text = "built-in"
    txtImages.Enabled = False
    Label1.Visible = False
Else
    txtImages.Text = ""
    txtImages.Enabled = True
    Label1.Visible = True
End If

End Sub

Private Sub cmdApply_Click()
If chkmmImage.Value = 1 Then
    txtImages.Text = "built-in"
    txtImages.Enabled = False
    Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\imageapp", "String", "built-in")
Else
    Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\imageapp", "String", txtImages.Text)
End If

Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\musicapp", "String", txtMusic.Text)
Call savestring(HKEY_CURRENT_USER, "Software\MUCKmate\exe", "String", txtExe.Text)
End Sub

Private Sub cmdCancel_Click()
Set frmOptions = Nothing
Unload Me
End Sub

Private Sub cmdExeBrowse_Click()
cdlBrowse.Filter = "Windows Executable File|*.exe"
cdlBrowse.ShowOpen
txtExe.Text = cdlBrowse.FileName
End Sub

Private Sub cmdImageBrowse_Click()
cdlBrowse.Filter = "Windows Executable File|*.exe"
cdlBrowse.ShowOpen
txtImages.Text = cdlBrowse.FileName
End Sub

Private Sub cmdMusicBrowse_Click()
cdlBrowse.Filter = "Windows Executable File|*.exe"
cdlBrowse.ShowOpen
txtMusic.Text = cdlBrowse.FileName
End Sub

Private Sub Form_Load()
txtImages.Text = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\imageapp", "String")
txtMusic.Text = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\musicapp", "String")
txtExe.Text = getstring(HKEY_CURRENT_USER, "Software\MUCKmate\exe", "String")

If txtImages.Text = "built-in" Or txtImages.Text = "" Then

    txtImages.Text = "built-in"
    chkmmImage.Value = 1
    txtImages.Enabled = False
End If
End Sub

Private Sub Form_Unload(Cancel As Integer)
Set frmOptions = Nothing
Unload Me
End Sub







