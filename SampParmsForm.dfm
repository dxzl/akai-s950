object FormEditSampParms: TFormEditSampParms
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Edit Sample Parameters'
  ClientHeight = 304
  ClientWidth = 802
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
    Top = 97
    Width = 802
    Height = 207
    Align = alClient
    TabOrder = 0
    object LabelEnd: TLabel
      Left = 752
      Top = 60
      Width = 18
      Height = 13
      Caption = 'End'
    end
    object LabelStart: TLabel
      Left = 752
      Top = 6
      Width = 24
      Height = 13
      Caption = 'Start'
    end
    object LabelLoop: TLabel
      Left = 752
      Top = 109
      Width = 23
      Height = 13
      Caption = 'Loop'
    end
    object LabelInfo: TLabel
      Left = 297
      Top = 162
      Width = 324
      Height = 13
      Caption = 
        'Loop Length (measured from the end point back to the loop-point)' +
        ': '
    end
    object LabelStartTimeL: TLabel
      Left = 257
      Top = 181
      Width = 77
      Height = 13
      Caption = 'Start Time (ms):'
    end
    object LabelEndTimeL: TLabel
      Left = 432
      Top = 181
      Width = 71
      Height = 13
      Caption = 'End Time (ms):'
    end
    object LabelLoopTimeL: TLabel
      Left = 603
      Top = 181
      Width = 76
      Height = 13
      Caption = 'Loop Time (ms):'
    end
    object LabelStartTime: TLabel
      Left = 336
      Top = 181
      Width = 64
      Height = 13
      Caption = '100000.0000'
    end
    object LabelEndTime: TLabel
      Left = 505
      Top = 181
      Width = 64
      Height = 13
      Caption = '100000.0000'
    end
    object LabelLoopTime: TLabel
      Left = 681
      Top = 181
      Width = 64
      Height = 13
      Caption = '100000.0000'
    end
    object LabelLoopLength: TLabel
      Left = 621
      Top = 162
      Width = 120
      Height = 13
      Caption = '10000000000000000000'
    end
    object EditEnd: TEdit
      Left = 7
      Top = 57
      Width = 57
      Height = 21
      TabStop = False
      ReadOnly = True
      TabOrder = 6
      Text = '0'
    end
    object EditStart: TEdit
      Left = 7
      Top = 6
      Width = 57
      Height = 21
      TabStop = False
      ReadOnly = True
      TabOrder = 4
      Text = '0'
    end
    object EditLoop: TEdit
      Left = 7
      Top = 109
      Width = 57
      Height = 21
      TabStop = False
      ReadOnly = True
      TabOrder = 7
      Text = '0'
    end
    object ButtonClose: TButton
      Left = 7
      Top = 159
      Width = 98
      Height = 25
      Caption = 'Close'
      TabOrder = 3
      OnClick = ButtonCloseClick
    end
    object TBStart: TTrackBar
      Left = 87
      Top = 6
      Width = 659
      Height = 45
      Max = 0
      ShowSelRange = False
      TabOrder = 0
      TickStyle = tsNone
      OnChange = TBStartChange
    end
    object TBEnd: TTrackBar
      Left = 87
      Top = 57
      Width = 659
      Height = 45
      Max = 0
      ShowSelRange = False
      TabOrder = 1
      TickStyle = tsNone
      OnChange = TBEndChange
    end
    object TBLoop: TTrackBar
      Left = 87
      Top = 108
      Width = 659
      Height = 45
      Max = 0
      ShowSelRange = False
      TabOrder = 2
      TickStyle = tsNone
      OnChange = TBLoopChange
    end
    object UpDownStart: TUpDown
      Left = 64
      Top = 6
      Width = 16
      Height = 21
      Associate = EditStart
      Max = 0
      TabOrder = 5
      OnChangingEx = UpDownStartChangingEx
    end
    object UpDownEnd: TUpDown
      Left = 64
      Top = 57
      Width = 16
      Height = 21
      Associate = EditEnd
      Max = 0
      TabOrder = 8
      OnChangingEx = UpDownEndChangingEx
    end
    object UpDownLoop: TUpDown
      Left = 64
      Top = 109
      Width = 16
      Height = 21
      Associate = EditLoop
      Max = 0
      TabOrder = 9
      OnChangingEx = UpDownLoopChangingEx
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 802
    Height = 97
    Align = alTop
    TabOrder = 1
    object LabelSampleName: TLabel
      Left = 126
      Top = 8
      Width = 149
      Height = 13
      Caption = 'Sample Name (10 ASCII Chars)'
    end
    object LabelLoudnessOffset: TLabel
      Left = 534
      Top = 62
      Width = 145
      Height = 13
      Caption = 'Loudness Offset [- 50 to +50]'
    end
    object LabelNominalPitch: TLabel
      Left = 534
      Top = 8
      Width = 257
      Height = 13
      Caption = 'Nominal Pitch [-2C (0) to +8G (127) b=flat, #=sharp]'
    end
    object Label1: TLabel
      Left = 534
      Top = 34
      Width = 220
      Height = 13
      Caption = 'Fine Pitch [-15 to +15 in 1/16 semitone steps]'
    end
    object LabelSampleRate: TLabel
      Left = 7
      Top = 78
      Width = 103
      Height = 13
      Caption = 'Frequency: ?????? Hz'
    end
    object LabelSampleLength: TLabel
      Left = 140
      Top = 78
      Width = 121
      Height = 13
      Caption = 'Length (in points): ??????'
    end
    object LabelSampleTime: TLabel
      Left = 295
      Top = 78
      Width = 83
      Height = 13
      Caption = 'Time (ms): ??????'
    end
    object ComboBoxSampNames: TComboBox
      Left = 7
      Top = 6
      Width = 113
      Height = 21
      AutoComplete = False
      TabOrder = 0
      Text = '<choose sample>'
      OnSelect = ComboBoxSampNamesSelect
    end
    object ButtonRefreshSamplesList: TButton
      Left = 7
      Top = 32
      Width = 130
      Height = 25
      Caption = 'Refresh Samples List'
      TabOrder = 6
      OnClick = ButtonRefreshSamplesListClick
    end
    object RadioGroupWaveForm: TRadioGroup
      Left = 297
      Top = 5
      Width = 81
      Height = 54
      Caption = 'Wave Form'
      Items.Strings = (
        'Normal'
        'Reversed')
      TabOrder = 1
      TabStop = True
    end
    object RadioGroupReplayMode: TRadioGroup
      Left = 384
      Top = 5
      Width = 89
      Height = 69
      Caption = 'Replay Mode'
      Items.Strings = (
        'One Shot'
        'Looping'
        'Alternating')
      TabOrder = 2
      TabStop = True
    end
    object EditLoudness: TEdit
      Left = 483
      Top = 59
      Width = 41
      Height = 21
      TabOrder = 5
    end
    object EditNominalPitch: TEdit
      Left = 483
      Top = 5
      Width = 41
      Height = 21
      TabOrder = 3
    end
    object EditFinePitch: TEdit
      Left = 483
      Top = 32
      Width = 41
      Height = 21
      TabOrder = 4
    end
  end
  object ButtonSendOrSave: TButton
    Left = 119
    Top = 256
    Width = 114
    Height = 25
    Caption = '(send or save)'
    Default = True
    Enabled = False
    TabOrder = 2
    OnClick = MenuSaveClick
  end
  object MainMenu1: TMainMenu
    Left = 408
    Top = 120
    object MenuOptions: TMenuItem
      Caption = 'Options'
      object MenuLoadFromAkiFile: TMenuItem
        Caption = 'Load from .aki file'
        ShortCut = 16497
        OnClick = MenuLoadFromAkiFileClick
      end
      object MenuSave: TMenuItem
        Caption = 'Save'
        Enabled = False
        ShortCut = 16498
        OnClick = MenuSaveClick
      end
    end
    object Help: TMenuItem
      Caption = 'Help'
      ShortCut = 112
      OnClick = HelpClick
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 336
    Top = 114
  end
end
