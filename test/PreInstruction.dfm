object frmPreInstruction: TfrmPreInstruction
  Left = 357
  Top = 179
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 493
  ClientWidth = 990
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClick = FormClick
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object trmInvalidator: TTimer
    Interval = 30
    OnTimer = trmInvalidatorTimer
    Left = 12
    Top = 16
  end
end
