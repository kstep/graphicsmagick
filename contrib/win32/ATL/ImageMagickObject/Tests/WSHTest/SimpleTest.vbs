Option Explicit

'On Error Resume Next
Const ERROR_SUCCESS = 0

Dim img
Dim info
Dim msgs
Dim elem
Dim sMsgs

Set img = CreateObject("ImageMagickObject.MagickImage.1")

msgs = img.Convert("logo:","logo.jpg")
If Err.Number <> ERROR_SUCCESS Then ShowError: WScript.Quit
Set img=Nothing
WScript.Quit(0)

Sub ShowError
  sMsgs = ""
  If BasicError(Err.Number) > 5000 Then
    msgs = img.Messages
    If isArray(msgs) Then
      For Each elem In msgs
        sMsgs = sMsgs & elem & vbCrLf
      Next
    End If
  End If
  WScript.Echo Err.Number & ": " & Err.Description & vbCrLf & sMsgs
End Sub

Function BasicError(e)
  BasicError = e And &HFFFF&
End Function
