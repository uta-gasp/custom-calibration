object frmMainForm: TfrmMainForm
  Left = 618
  Top = 513
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
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object log: TMemo
    Left = 4
    Top = 32
    Width = 243
    Height = 276
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
    Left = 12
    Top = 4
    Width = 227
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Calibration + Game'
    TabOrder = 1
    OnClick = btnStartClick
  end
  object tmrMouse: TTimer
    Enabled = False
    Interval = 30
    OnTimer = tmrMouseTimer
    Left = 12
    Top = 44
  end
end
