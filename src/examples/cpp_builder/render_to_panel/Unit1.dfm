object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Render to Panel'
  ClientHeight = 330
  ClientWidth = 489
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 304
    Height = 289
    Align = alClient
    Caption = 'Panel1'
    TabOrder = 0
  end
  object Panel2: TPanel
    Left = 304
    Top = 0
    Width = 185
    Height = 289
    Align = alRight
    Caption = 'Panel2'
    ShowCaption = False
    TabOrder = 1
    ExplicitLeft = 310
    ExplicitTop = -6
    object Label1: TLabel
      Left = 56
      Top = 5
      Width = 59
      Height = 29
      Caption = 'Zoom'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -24
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object ScrollBar1: TScrollBar
      Left = 80
      Top = 48
      Width = 17
      Height = 217
      Kind = sbVertical
      Max = 50
      PageSize = 0
      Position = 25
      TabOrder = 0
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 289
    Width = 489
    Height = 41
    Align = alBottom
    Caption = 'Engine is rendering to TPanel VCL component.'
    TabOrder = 2
  end
end
