object iFireflyAndPoints: TiFireflyAndPoints
  Left = 693
  Top = 308
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 550
  ClientWidth = 909
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyUp = FormKeyUp
  OnMouseMove = FormMouseMove
  OnMouseUp = FormMouseUp
  PixelsPerInch = 96
  TextHeight = 13
  object tmrKostyl: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrKostylTimer
    Left = 16
    Top = 12
  end
end
