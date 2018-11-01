object FormMain: TFormMain
  Left = 0
  Top = 0
  ActiveControl = Memo1
  Caption = 'Akai S900/S950 (rs232)'
  ClientHeight = 157
  ClientWidth = 583
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
  KeyPreview = True
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 137
    Height = 157
    Align = alLeft
    TabOrder = 0
    DesignSize = (
      137
      157)
    object ComboBoxRs232: TComboBox
      Left = 1
      Top = 2
      Width = 132
      Height = 21
      Hint = 'Baud rate (if using RS232)'
      Anchors = [akLeft, akTop, akRight]
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnChange = ComboBoxRs232Change
      OnSelect = ComboBoxRs232Select
      Items.Strings = (
        '300'
        '600'
        '1200'
        '2400'
        '4800'
        '9600'
        '19200'
        '38400'
        '57600'
        '115200')
    end
    object ListBox1: TListBox
      Tag = 1
      Left = 1
      Top = 29
      Width = 135
      Height = 127
      Anchors = [akLeft, akTop, akBottom]
      ItemHeight = 13
      TabOrder = 1
      OnClick = ListBox1Click
    end
  end
  object Panel1: TPanel
    Left = 137
    Top = 0
    Width = 446
    Height = 157
    Align = alClient
    TabOrder = 1
    object Memo1: TMemo
      Tag = 2
      Left = 1
      Top = 1
      Width = 444
      Height = 155
      Align = alClient
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 0
    end
  end
  object OpenDialog1: TOpenDialog
    InitialDir = '%Desktop%'
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofEnableSizing]
    Left = 416
    Top = 16
  end
  object SaveDialog1: TSaveDialog
    Left = 352
    Top = 16
  end
  object MainMenu1: TMainMenu
    Left = 160
    Top = 16
    object S9001: TMenuItem
      Caption = 'Menu'
      ShortCut = 16496
      object MenuGetCatalog: TMenuItem
        Caption = '&Get list of samples and programs'
        ShortCut = 16497
        OnClick = MenuGetCatalogClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object MenuPutSample: TMenuItem
        Caption = '&Send to machine (.wav .aki .prg files)'
        ShortCut = 16498
        OnClick = MenuPutSampleClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object MenuGetSample: TMenuItem
        Caption = '&Receive sample from machine (.wav .aki file)'
        ShortCut = 16499
        OnClick = MenuGetSampleClick
      end
      object MenuGetPrograms: TMenuItem
        Caption = 'Receive &programs from machine (.prg file)'
        ShortCut = 16500
        OnClick = MenuGetProgramsClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object MenuUseRightChanForStereoSamples: TMenuItem
        Caption = 'Use right channel for stereo WAVs'
        Checked = True
        ShortCut = 16502
        OnClick = MenuUseRightChanForStereoSamplesClick
      end
      object MenuAutomaticallyRenameSample: TMenuItem
        Caption = 'Automatically rename sample on machine'
        Checked = True
        ShortCut = 16503
        OnClick = MenuAutomaticallyRenameSampleClick
      end
      object MenuUseHWFlowControlBelow50000Baud: TMenuItem
        Caption = 'Use hardware flow-control below 50,000 baud'
        ShortCut = 16504
        OnClick = MenuUseRightChanForStereoSamplesClick
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object MenuMakeOrEditProgram: TMenuItem
        Caption = 'Make/Edit Program'
        ShortCut = 16505
        OnClick = MenuMakeOrEditProgramClick
      end
      object MenuEditSampleParameters: TMenuItem
        Caption = 'Edit Sample Parameters'
        ShortCut = 16506
        OnClick = MenuEditSampleParametersClick
      end
      object MenuEditOverallSettings: TMenuItem
        Caption = 'Edit Overall Settings'
        ShortCut = 16507
        OnClick = MenuEditOverallSettingsClick
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object MenuAbout: TMenuItem
        Caption = '&About'
        ShortCut = 112
        OnClick = MenuAboutClick
      end
    end
    object MainMenuHelp: TMenuItem
      Caption = 'Help'
      ShortCut = 112
      OnClick = MainMenuHelpClick
    end
  end
  object Timer1: TTimer
    Enabled = False
    Left = 224
    Top = 16
  end
  object ApdComPort1: TApdComPort
    Baud = 38400
    TraceName = 'APRO.TRC'
    LogName = 'APRO.LOG'
    Left = 281
    Top = 16
  end
end
