object FormProgram: TFormProgram
  Left = 0
  Top = 0
  Caption = 'Create S900/S950 Program'
  ClientHeight = 467
  ClientWidth = 916
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
    0000800080008080000080808000C0C0C0000000FF0000FF000000FFFF00FF00
    0000FF00FF00FFFF0000FFFFFF00F000000000000000F888888888888880F777
    777777777780F7BE00000000DB80F7BBE000000DBB80F7B0BE7888DB0B80F7B0
    0B7008B00B80F7B0097008900B80F7B0097F08900B80F7B00B7778B00B80F7B0
    BD0000EB0B80F7BBD000000EBB80F7BD00000000EB80F788888888888880F777
    777777777780FFFFFFFFFFFFFFFF0000000000000000000000000FF0000007E0
    0000000000000180000001800000008000000000000003C0000007E000000FF0
    0000000000000000000000000000280000002000000040000000010004000000
    0000800200000000000000000000000000000000000000000000000080000080
    00000080800080000000800080008080000080808000C0C0C0000000FF0000FF
    000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0000000000000000000000
    000000000000F7888888888888888888888888888880F7877777777777777777
    777777777780F78E000000000000000000000000D780F78BE000000000000000
    0000000DB780F78BBE00000000000000000000DBB780F78B0BE0000000000000
    00000DB0B780F78B00BE0000000000000000DB00B780F78B000BE00000000000
    000DB000B780F78B0000BE000000000000DB0000B780F78B00000BE000000000
    0DB00000B780F78B000000B7888888888B000000B780F78B0000009787777777
    89000000B780F78B000000978F00000789000000B780F78B000000978F000007
    89000000B780F78B000000978F00000789000000B780F78B000000978F000007
    89000000B780F78B000000978F00000789000000B780F78B000000978FFFFF07
    89000000B780F78B000000978888888789000000B780F78B000000B777777777
    8B000000B780F78B00000BD0000000000EB00000B780F78B0000BD0000000000
    00EB0000B780F78B000BD00000000000000EB000B780F78B00BD000000000000
    0000EB00B780F78B0BD000000000000000000EB0B780F78BBD00000000000000
    000000EBB780F78BD0000000000000000000000EB780F78D0000000000000000
    00000000E780F7888888888888888888888888888780F7777777777777777777
    777777777780FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000
    00000FFFFFF007FFFFE003FFFFC001FFFF8000FFFF00007FFE00003FFC00001F
    F8000000000000000000000000000003C0000003C0000003C0000003C0000000
    00000000000000000000001FF800003FFC00007FFE0000FFFF0001FFFF8003FF
    FFC007FFFFE00FFFFFF0000000000000000000000000}
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
    Width = 916
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
    Top = 426
    Width = 916
    Height = 41
    Align = alBottom
    TabOrder = 1
    ExplicitTop = 220
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
    object ButtonSelectAll: TButton
      Left = 600
      Top = 5
      Width = 129
      Height = 25
      Caption = 'Select All'
      TabOrder = 3
      OnClick = ButtonSelectAllClick
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 73
    Width = 916
    Height = 353
    Align = alClient
    Caption = 'Panel3'
    TabOrder = 2
    ExplicitHeight = 147
    object SG: TStringGrid
      Left = 1
      Top = 1
      Width = 914
      Height = 351
      Align = alClient
      ColCount = 2
      DefaultColWidth = 100
      RowCount = 4
      FixedRows = 3
      Options = [goFixedVertLine, goVertLine, goHorzLine, goRangeSelect, goRowMoving, goEditing, goAlwaysShowEditor, goFixedHotTrack]
      TabOrder = 0
      OnMouseDown = SGMouseDown
      ExplicitHeight = 145
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
    object MenuTools: TMenuItem
      Caption = 'Tools'
      object MenuDrumify: TMenuItem
        Caption = 'Drumify'
        ShortCut = 32881
        OnClick = MenuDrumifyClick
      end
      object MenuDrumifyBlack: TMenuItem
        Caption = 'Drumify (semitones only)'
        Hint = 'Puts selected keygroups on ascending individual black keys'
        ShortCut = 32882
        OnClick = MenuDrumifyBlackClick
      end
      object MenuDrumifyWhite: TMenuItem
        Caption = 'Drumify  (whole notes only)'
        Hint = 'Puts selected keygroups on ascending individual white keys'
        ShortCut = 32883
        OnClick = MenuDrumifyWhiteClick
      end
      object MenuOctifyKG: TMenuItem
        Caption = 'Octify Keygroups'
        Hint = 'Assigns each keygroup to one octave and auto-transposes'
        ShortCut = 32884
        OnClick = MenuOctifyKGClick
      end
      object MenuMidifyKeygroups: TMenuItem
        Caption = 'Midify Keygroups'
        Hint = 'Assigns each selected keygroup to a different midi-offset'
        ShortCut = 32885
        OnClick = MenuMidifyKeygroupsClick
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object MenuDeleteSelectedKeygroups: TMenuItem
        Caption = 'Delete Selected Keygroups'
        ShortCut = 32886
        OnClick = MenuDeleteSelectedKeygroupsClick
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object MenuClearSelected: TMenuItem
        Caption = 'Clear Selected Keygroups'
        ShortCut = 32887
        OnClick = MenuClearSelectedClick
      end
      object MenuSelectAll: TMenuItem
        Caption = 'Select All Keygroups'
        ShortCut = 32888
        OnClick = MenuSelectAllClick
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object MenuCopyHilightedKeygroup: TMenuItem
        Caption = 'Copy Current Keygroup'#39's Data'
        ShortCut = 32889
        OnClick = MenuCopyHilightedKeygroupClick
      end
      object MenuPasteHilightedKeygroup: TMenuItem
        Caption = 'Paste Copied Keygroup'#39's Data'
        ShortCut = 32890
        OnClick = MenuPasteHilightedKeygroupClick
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
