object frmMainForm: TfrmMainForm
  Left = 1138
  Top = 5
  AutoScroll = False
  Caption = 'KidCalibt est'
  ClientHeight = 383
  ClientWidth = 191
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label4: TLabel
    Left = 16
    Top = 12
    Width = 31
    Height = 13
    Caption = 'Target'
  end
  object log: TMemo
    Left = 4
    Top = 120
    Width = 185
    Height = 261
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
  object btnStartCalibAndGame: TButton
    Left = 16
    Top = 36
    Width = 169
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Calibration + Game'
    TabOrder = 1
    OnClick = btnStartClick
  end
  object btnStartCalibOnly: TButton
    Left = 16
    Top = 64
    Width = 169
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Calibration only'
    TabOrder = 2
    OnClick = btnStartClick
  end
  object btnStartGameOnly: TButton
    Left = 16
    Top = 92
    Width = 169
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Game only'
    TabOrder = 3
    OnClick = btnStartClick
  end
  object cmbTarget: TComboBox
    Left = 80
    Top = 8
    Width = 105
    Height = 21
    Style = csDropDownList
    Anchors = [akTop, akRight]
    ItemHeight = 13
    TabOrder = 4
    Items.Strings = (
      'firefly'
      'ball')
  end
end
