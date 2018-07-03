object FormOverallSettings: TFormOverallSettings
  Left = 0
  Top = 0
  AutoSize = True
  Caption = 'Overall Settings'
  ClientHeight = 257
  ClientWidth = 481
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
    Width = 481
    Height = 257
    TabOrder = 0
    object LabelMidiExTx: TLabel
      Left = 160
      Top = 61
      Width = 164
      Height = 13
      Caption = 'Midi Tx Chan Akai Exclusive (1-16)'
    end
    object LabelProgName: TLabel
      Left = 16
      Top = 5
      Width = 110
      Height = 13
      Caption = 'Current Program Name'
    end
    object LabelRxSimChan: TLabel
      Left = 16
      Top = 110
      Width = 107
      Height = 13
      Caption = 'Rx Sim Channel (1-16)'
    end
    object LabelRxSimKey: TLabel
      Left = 160
      Top = 110
      Width = 92
      Height = 13
      Caption = 'Rx Sim Key (0-127)'
    end
    object LabelRxSimVel: TLabel
      Left = 16
      Top = 156
      Width = 111
      Height = 13
      Caption = 'Rx Sim Velocity (1-127)'
    end
    object LabelBaseMidiChan: TLabel
      Left = 16
      Top = 61
      Width = 119
      Height = 13
      Caption = 'Base Midi Channel (1-16)'
    end
    object LabelBaudRate: TLabel
      Left = 160
      Top = 5
      Width = 99
      Height = 13
      Caption = 'Machine'#39's Baud Rate'
    end
    object LabelPitchWheelRange: TLabel
      Left = 160
      Top = 156
      Width = 123
      Height = 13
      Caption = 'Pitch Wheel Range (0-12)'
    end
    object EditRxSimKey: TEdit
      Left = 160
      Top = 129
      Width = 53
      Height = 21
      TabOrder = 5
    end
    object EditRxSimVel: TEdit
      Left = 16
      Top = 175
      Width = 57
      Height = 21
      TabOrder = 6
    end
    object EditPitchWheelRange: TEdit
      Left = 160
      Top = 175
      Width = 53
      Height = 21
      Hint = 'Range is up/down by the number of semitones entered (0-12)'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 7
    end
    object CheckBoxOmni: TCheckBox
      Left = 304
      Top = 109
      Width = 145
      Height = 17
      Caption = 'Midi Omni (All Channels)'
      TabOrder = 8
    end
    object CheckBoxLoudnessOnMidi7: TCheckBox
      Left = 304
      Top = 132
      Width = 145
      Height = 17
      Hint = 'Enable volume control via Midi controller 7 messages'
      Caption = 'Loudness (Midi Ctrl 7)'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 9
    end
    object CheckBoxMidiProgChange: TCheckBox
      Left = 304
      Top = 155
      Width = 145
      Height = 17
      Hint = 'Allow midi program (patch-change)'
      Caption = 'Allow Midi Prog Change'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 10
    end
    object ComboBoxProgNames: TComboBox
      Left = 16
      Top = 24
      Width = 110
      Height = 21
      Hint = 'Selects the current program'
      AutoComplete = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object ComboBoxMidiExTx: TComboBox
      Left = 160
      Top = 80
      Width = 74
      Height = 21
      AutoComplete = False
      TabOrder = 3
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16')
    end
    object ComboBoxRxSimChan: TComboBox
      Left = 16
      Top = 129
      Width = 74
      Height = 21
      Hint = 'Midi simulator receive channel'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16')
    end
    object ComboBoxBaseMidiChan: TComboBox
      Left = 16
      Top = 80
      Width = 74
      Height = 21
      Hint = 'This is the base channel a keygroup midi offset is added to'
      AutoComplete = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16')
    end
    object ComboBoxBaudRate: TComboBox
      Left = 160
      Top = 24
      Width = 89
      Height = 21
      Hint = 'blah'
      AutoComplete = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
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
    object RadioGroupMidiRs232: TRadioGroup
      Left = 328
      Top = 5
      Width = 129
      Height = 57
      Hint = 'This is fixed at RS232 for this app!'
      Caption = 'Communication Mode'
      Enabled = False
      ItemIndex = 1
      Items.Strings = (
        'Midi'
        'RS232')
      ParentShowHint = False
      ShowHint = True
      TabOrder = 14
    end
    object ButtonClose: TButton
      Left = 30
      Top = 216
      Width = 121
      Height = 25
      Caption = 'Close'
      TabOrder = 11
      OnClick = ButtonCloseClick
    end
    object ButtonSend: TButton
      Left = 171
      Top = 216
      Width = 129
      Height = 25
      Caption = 'Send'
      Default = True
      Enabled = False
      TabOrder = 12
      OnClick = ButtonSendClick
    end
    object ButtonRefresh: TButton
      Left = 318
      Top = 216
      Width = 129
      Height = 25
      Caption = 'Refresh'
      TabOrder = 13
      OnClick = ButtonRefreshClick
    end
  end
end
