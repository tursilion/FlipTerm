VERSION 5.00
Begin VB.Form frmImageview 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Image Viewer"
   ClientHeight    =   6015
   ClientLeft      =   240
   ClientTop       =   915
   ClientWidth     =   4590
   Icon            =   "frmImageview.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6015
   ScaleWidth      =   4590
   Begin VB.PictureBox pctImage 
      Height          =   6015
      Left            =   0
      ScaleHeight     =   397
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   301
      TabIndex        =   0
      Top             =   0
      Width           =   4575
   End
End
Attribute VB_Name = "frmImageview"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Public Sub CloseWin()

Form_Unload (1)

End Sub


Private Sub Form_Unload(Cancel As Integer)

Set frmImageview = Nothing
Unload Me

End Sub

