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
      Hint = 
        'Sets baud-rate at both the machine and here when you click Send.' +
        ' If you change this and communication stope, you must reset the ' +
        'baud rate manually at both the machine and main window of this p' +
        'rogram!'
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
