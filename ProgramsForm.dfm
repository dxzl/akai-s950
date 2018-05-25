object FormProgram: TFormProgram
  Left = 0
  Top = 0
  Caption = 'Create S900/S950 Program'
  ClientHeight = 305
  ClientWidth = 889
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    000001000200101010000000000028010000260000002020100000000000E802
    00004E0100002800000010000000200000000100040000000000C00000000000
    0000000000000000000000000000000000000000800000800000008080008000
    00008000800080800000C0C0C000808080000000FF0000FF000000FFFF00FF00
    0000FF00FF00FFFF0000FFFFFF00999AAACCCBBB5555999AAACCCBBB5555999A
    AACCCBBB5555999AAACCCBBB5555999AAACCCBBB5555999AAACCCBBB5555999A
    AACCCBBB5555999AAACCCBBB5555999AAACCCBBB5555999AAACCCBBB5555999A
    AACCCBBB5555999AAACCCBBB5555999AAACCCBBB5555999AAACCCBBB5555999A
    AACCCBBB5555999AAACCCBBB5555000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000280000002000000040000000010004000000
    0000800200000000000000000000000000000000000000000000000080000080
    000000808000800000008000800080800000C0C0C000808080000000FF0000FF
    000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00CCCCDDDDDD0099990000
    000000444400CCCCCDDDDD0009999900000004000040CCCCCCCDDD000A999999
    000040000004CC55CCCCDD000AAA9999900040000004CC55CCCCDDD00AAAA999
    990040000004CC5555CCCCD00AAAAAA9999040000004CCC5555CCCC000AAAAAA
    999904000040CCC55555CCC000AAAAAAA999004444000CC5555555CC00AAAAAA
    A999000000000CC5555555CC00AAAAAAA999000000000CCC5555555CC00AAAAA
    A999000000000CCC5555555CC00AAAAA99900000000000CC5555555CC00AAAA9
    99900000000000CC55555555CC0AAAA999000000000000CCC5555555CC00AA99
    90000000000000CCC5555555CC00AA99000000000000000CC5555555CC00A999
    000000000000000CC5555555CC000999000000000000000CC5555555CC000999
    000000000000000CCC55555CCC000B99000000000000000CCC55555CC2000B99
    9000000000000000CC55555CC2000BB99900000000000000CC5555CCC2200BB9
    9990000000000000CCC555CCC22000BB9999000000000000CCC55CCC222000BB
    B9999000000000000CC5CCCC222200BBBBB99990000000000CCCCCC2222200BB
    BBBB9999900000000CCCCC222222000BBBBBBB999900000000CCC00000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000FFC300003F910000
    0F46000007420000035A00000142000000BD000000C3800000FF800000FF8000
    00FF800001FFC00001FFC00003FFC00007FFC0000FFFE0000FFFE0000FFFE000
    0FFFE0000FFFE00007FFF00003FFF00001FFF00000FFF000007FF800001FF800
    0007F8000003F8000003F8000003FFFFFFFFFFFFFFFF}
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 889
    Height = 73
    Align = alTop
    TabOrder = 0
    object LabelProgName: TLabel
      Left = 119
      Top = 11
      Width = 174
      Height = 13
      Caption = 'Program Name (10 characters max.)'
    end
    object LabelKeyTilt: TLabel
      Left = 391
      Top = 11
      Width = 155
      Height = 13
      Caption = 'Key vs. Loudness Slope (+/- 50)'
    end
    object Label1: TLabel
      Left = 391
      Top = 38
      Width = 200
      Height = 13
      Caption = 'Midi Program  number (0-127) [S950 only]'
    end
    object EditKeyTilt: TEdit
      Left = 352
      Top = 8
      Width = 33
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object ComboBoxProgNames: TComboBox
      Left = 8
      Top = 8
      Width = 105
      Height = 21
      AutoComplete = False
      TabOrder = 1
      OnSelect = ComboBoxProgNamesSelect
    end
    object EditMidiPrgNum: TEdit
      Left = 352
      Top = 35
      Width = 33
      Height = 21
      TabOrder = 2
      Text = '0'
    end
    object CheckBoxPosXfade: TCheckBox
      Left = 616
      Top = 10
      Width = 161
      Height = 17
      Caption = 'Enable Positional Crossfade'
      TabOrder = 3
    end
    object CheckBoxEnableProgNumber: TCheckBox
      Left = 616
      Top = 37
      Width = 161
      Height = 17
      Caption = 'Enable Midi Program Number'
      TabOrder = 4
    end
    object ButtonRefreshProgramsList: TButton
      Left = 8
      Top = 37
      Width = 137
      Height = 25
      Caption = 'Refresh Programs List'
      TabOrder = 5
      OnClick = ButtonRefreshProgramsListClick
    end
    object ButtonAddKg: TButton
      Left = 214
      Top = 37
      Width = 35
      Height = 25
      Caption = '+'
      TabOrder = 6
      OnClick = ButtonAddKgClick
    end
    object ButtonDelKg: TButton
      Left = 255
      Top = 37
      Width = 35
      Height = 25
      Caption = '-'
      Enabled = False
      TabOrder = 7
      OnClick = ButtonDelKgClick
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 264
    Width = 889
    Height = 41
    Align = alBottom
    TabOrder = 1
    object ButtonSend: TButton
      Left = 97
      Top = 6
      Width = 168
      Height = 25
      Caption = 'Send Program to Machine'
      Default = True
      Enabled = False
      TabOrder = 0
      OnClick = ButtonSendClick
    end
    object ButtonClose: TButton
      Left = 16
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Close'
      TabOrder = 1
      OnClick = ButtonCloseClick
    end
    object ButtonKgDefaults: TButton
      Left = 360
      Top = 6
      Width = 217
      Height = 25
      Caption = 'Selected Keygroup Set Defaults'
      TabOrder = 2
      OnClick = ButtonKgDefaultsClick
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 73
    Width = 889
    Height = 191
    Align = alClient
    Caption = 'Panel3'
    TabOrder = 2
    object SG: TStringGrid
      Left = 1
      Top = 1
      Width = 887
      Height = 189
      Align = alClient
      ColCount = 2
      DefaultColWidth = 100
      RowCount = 4
      FixedRows = 3
      Options = [goFixedVertLine, goVertLine, goHorzLine, goRangeSelect, goRowMoving, goEditing, goAlwaysShowEditor, goFixedHotTrack]
      TabOrder = 0
      ColWidths = (
        100
        100)
      RowHeights = (
        24
        24
        24
        24)
    end
  end
  object MainMenu1: TMainMenu
    Left = 488
    Top = 113
    object Menu1: TMenuItem
      Caption = 'Menu'
      object CopyKeygroup1: TMenuItem
        Caption = 'Copy and add keygroup'
        ShortCut = 16497
        OnClick = CopyKeygroup1Click
      end
      object DeleteKeygroup1: TMenuItem
        Caption = 'Remove keygroup'
        ShortCut = 16498
        OnClick = DeleteKeygroup1Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object MenuSaveIntoPrgFile: TMenuItem
        Caption = 'Save program into .prg file'
        Enabled = False
        ShortCut = 16499
        OnClick = MenuSaveIntoPrgFileClick
      end
      object MenuLoadFromPrgFile: TMenuItem
        Caption = 'Load program from .prg file'
        ShortCut = 16500
        OnClick = MenuLoadFromPrgFileClick
      end
      object MenuDeleteProg: TMenuItem
        Caption = 'Delete program from .prg file'
        ShortCut = 16501
        OnClick = MenuDeleteProgClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object MenuSendToMachine: TMenuItem
        Caption = 'Send program to machine'
        Enabled = False
        ShortCut = 16502
        OnClick = MenuSendToMachineClick
      end
      object MenuLoadFromMachine: TMenuItem
        Caption = 'Load program from machine'
        ShortCut = 16503
        OnClick = MenuLoadFromMachineClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object MenuShowUndefined: TMenuItem
        Caption = 'Display undefined/reserved fields'
        OnClick = MenuShowUndefinedClick
      end
    end
    object MenuHelp: TMenuItem
      Caption = 'Help'
      ShortCut = 112
      OnClick = MenuHelpClick
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 568
    Top = 113
  end
  object SaveDialog1: TSaveDialog
    Left = 640
    Top = 113
  end
end
