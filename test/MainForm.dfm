object frmMainForm: TfrmMainForm
  Left = 738
  Top = 429
  AutoScroll = False
  Caption = 'KidCalibt est'
  ClientHeight = 317
  ClientWidth = 249
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object log: TMemo
    Left = 4
    Top = 132
    Width = 243
    Height = 176
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Consolas'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object btnStartCalib3: TButton
    Left = 8
    Top = 64
    Width = 237
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Profiled game'
    TabOrder = 1
    OnClick = btnStartCalib3Click
  end
  object btnStartCalib1: TButton
    Left = 8
    Top = 8
    Width = 237
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Standard'
    TabOrder = 2
    OnClick = btnStartCalib1Click
  end
  object btnStartCalib2: TButton
    Left = 8
    Top = 36
    Width = 237
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Firefly'
    TabOrder = 3
    OnClick = btnStartCalib2Click
  end
  object btnRunUserDay: TButton
    Left = 152
    Top = 100
    Width = 91
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Run session'
    Enabled = False
    TabOrder = 4
    OnClick = btnRunUserDayClick
  end
  object cmbUsers: TComboBox
    Left = 8
    Top = 104
    Width = 73
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 5
    OnChange = cmbUsersOrDaysChange
  end
  object cmbDays: TComboBox
    Left = 84
    Top = 104
    Width = 61
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
    OnChange = cmbUsersOrDaysChange
  end
end
