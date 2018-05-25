object FormListBox: TFormListBox
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsSizeToolWin
  Caption = 'Select Program'
  ClientHeight = 161
  ClientWidth = 121
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ListBox1: TListBox
    Left = 0
    Top = 0
    Width = 121
    Height = 161
    BevelWidth = 2
    ItemHeight = 13
    TabOrder = 0
    OnClick = ListBox1Click
  end
end
