object iProfiledGame: TiProfiledGame
  Left = 187
  Top = 180
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
  OnKeyDown = FormKeyDown
  OnKeyUp = FormKeyUp
  OnMouseUp = FormMouseUp
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object pnlNameContainer: TPanel
    Left = 248
    Top = 231
    Width = 390
    Height = 60
    Anchors = []
    BevelOuter = bvNone
    Color = 14473942
    TabOrder = 0
    object memName: TMemo
      Left = 12
      Top = 12
      Width = 366
      Height = 40
      Alignment = taCenter
      Anchors = [akLeft, akTop, akRight, akBottom]
      BorderStyle = bsNone
      Font.Charset = ANSI_CHARSET
      Font.Color = 4079166
      Font.Height = -32
      Font.Name = 'Calibri'
      Font.Style = []
      MaxLength = 12
      ParentColor = True
      ParentFont = False
      TabOrder = 0
      WantReturns = False
      WordWrap = False
      OnKeyDown = memNameKeyDown
      OnKeyPress = memNameKeyPress
    end
  end
  object tmrKostyl1: TTimer
    Enabled = False
    Interval = 1500
    OnTimer = tmrKostyl1Timer
    Left = 8
    Top = 12
  end
  object tmrKostyl2: TTimer
    Enabled = False
    OnTimer = tmrKostyl2Timer
    Left = 44
    Top = 12
  end
  object tmrKostyl3: TTimer
    Enabled = False
    Interval = 10
    OnTimer = tmrKostyl3Timer
    Left = 80
    Top = 12
  end
  object tmrKostyl4: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrKostyl4Timer
    Left = 116
    Top = 12
  end
end
