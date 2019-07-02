VERSION 5.00
Begin VB.Form frmAbout 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "About MUCKmate"
   ClientHeight    =   2610
   ClientLeft      =   5190
   ClientTop       =   4425
   ClientWidth     =   4680
   Icon            =   "frmAbout.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2610
   ScaleWidth      =   4680
   Begin VB.CommandButton cmdOk 
      Caption         =   "Ok"
      Height          =   375
      Left            =   1560
      TabIndex        =   3
      Top             =   2160
      Width           =   1335
   End
   Begin VB.PictureBox Picture1 
      Height          =   540
      Left            =   120
      Picture         =   "frmAbout.frx":030A
      ScaleHeight     =   32
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   32
      TabIndex        =   0
      Top             =   120
      Width           =   540
   End
   Begin VB.Line Line2 
      X1              =   720
      X2              =   4560
      Y1              =   600
      Y2              =   600
   End
   Begin VB.Label Label2 
      Caption         =   $"frmAbout.frx":0614
      Height          =   1215
      Left            =   720
      TabIndex        =   2
      Top             =   720
      Width           =   3855
   End
   Begin VB.Label Label1 
      Caption         =   "MUCKmate, version 1.0 BETA by Karna."
      Height          =   255
      Left            =   720
      TabIndex        =   1
      Top             =   120
      Width           =   3855
   End
   Begin VB.Line Line1 
      X1              =   120
      X2              =   4560
      Y1              =   2040
      Y2              =   2040
   End
End
Attribute VB_Name = "frmAbout"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub cmdOk_Click()
Unload Me
End Sub

Public Sub CloseWin()

Unload Me

End Sub

Private Sub Form_Unload(Cancel As Integer)
Set frmAbout = Nothing
Unload Me
End Sub
