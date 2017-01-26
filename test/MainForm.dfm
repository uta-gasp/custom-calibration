object frmMainForm: TfrmMainForm
  Left = 1138
  Top = 88
  AutoScroll = False
  Caption = 'KidCalibt est'
  ClientHeight = 621
  ClientWidth = 191
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyUp = FormKeyUp
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 12
    Top = 80
    Width = 78
    Height = 13
    Caption = 'Animation speed'
  end
  object Label2: TLabel
    Left = 16
    Top = 140
    Width = 82
    Height = 13
    Caption = 'Movement speed'
  end
  object Label3: TLabel
    Left = 20
    Top = 196
    Width = 72
    Height = 13
    Caption = 'Rotation speed'
  end
  object Button1: TButton
    Left = 12
    Top = 8
    Width = 75
    Height = 25
    Caption = 'animate'
    TabOrder = 0
    OnClick = Button1Click
  end
  object CheckBox1: TCheckBox
    Left = 92
    Top = 12
    Width = 97
    Height = 17
    Caption = 'Loop'
    TabOrder = 1
    OnClick = CheckBox1Click
  end
  object TrackBar1: TTrackBar
    Left = 0
    Top = 96
    Width = 189
    Height = 45
    Max = 30
    Min = 10
    Orientation = trHorizontal
    Frequency = 1
    Position = 10
    SelEnd = 0
    SelStart = 0
    TabOrder = 2
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = TrackBar1Change
  end
  object TrackBar2: TTrackBar
    Left = 0
    Top = 152
    Width = 189
    Height = 45
    LineSize = 50
    Max = 2000
    Min = 200
    Orientation = trHorizontal
    PageSize = 200
    Frequency = 100
    Position = 200
    SelEnd = 0
    SelStart = 0
    TabOrder = 3
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = TrackBar2Change
  end
  object log: TMemo
    Left = 0
    Top = 264
    Width = 185
    Height = 221
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Consolas'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 4
  end
  object Button2: TButton
    Left = 12
    Top = 40
    Width = 75
    Height = 25
    Caption = 'fade'
    TabOrder = 5
    OnClick = Button2Click
  end
  object Panel1: TPanel
    Left = 0
    Top = 488
    Width = 193
    Height = 129
    TabOrder = 6
    OnMouseMove = Panel1MouseMove
    OnMouseUp = Panel1MouseUp
  end
  object Button3: TButton
    Left = 96
    Top = 40
    Width = 75
    Height = 25
    Caption = 'rotate'
    TabOrder = 7
    OnClick = Button3Click
  end
  object TrackBar3: TTrackBar
    Left = 2
    Top = 208
    Width = 189
    Height = 45
    LineSize = 50
    Max = 2000
    Min = 200
    Orientation = trHorizontal
    PageSize = 200
    Frequency = 100
    Position = 200
    SelEnd = 0
    SelStart = 0
    TabOrder = 8
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = TrackBar3Change
  end
end
