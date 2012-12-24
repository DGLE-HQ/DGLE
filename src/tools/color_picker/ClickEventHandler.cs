// \author		Kuzmin Maxim aka eshkin_kot
// \date		18.12.2012 (c)Andrey Korotkov
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;

namespace ColorPicker
{
	public class ClickEventHandler
	{
		private static ClickEventHandler inst;
		private ColorSpectrum colorSpectrum;
		private ColorBrightness colorBrightness;
		private ColorSaturation colorSaturation;
		private ColorSquare colorSquare;
		private ColorView colorViewHandler;
		private ColorCode colorCodeHandler;
		private AColorCode alphaColorCodeHandler;
		private ColorScales colorScalesHandler;
		private AlphaView alphaView;
		
		public ClickEventHandler ()
		{
			colorSpectrum = ColorSpectrum.Inst;
			colorBrightness = ColorBrightness.Inst;
			colorSaturation = ColorSaturation.Inst;
			colorSquare = ColorSquare.Inst;
			colorViewHandler = ColorView.Inst;
			colorCodeHandler = ColorCode.Inst;
			alphaColorCodeHandler = AColorCode.Inst;
			colorScalesHandler = ColorScales.Inst;
			alphaView = AlphaView.Inst;
			inst = this;
		}
		
		public void SpectrClicked (int x, int y)
		{
			colorBrightness.ClickProcessing (x, y);
			colorSaturation.ClickProcessing (colorSpectrum.NativeRed, 
			                                 colorSpectrum.NativeGreen, 
			                                 colorSpectrum.NativeBlue);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y, 
			                            colorBrightness.SpectrX);
			colorViewHandler.SetupColor (colorSpectrum.Red, 
			                             colorSpectrum.Green,
			                             colorSpectrum.Blue);
			colorScalesHandler.SetupScales (colorSpectrum.Red, 
			                                colorSpectrum.Green,
			                                colorSpectrum.Blue);
			colorCodeHandler.SetupCode (colorViewHandler.Color);
			alphaColorCodeHandler.SetupCode(colorViewHandler.Color,
			                                (ushort)colorScalesHandler.Alpha);
			alphaView.ClickProcessing(colorSpectrum.Red, 
			                          colorSpectrum.Green,
			                          colorSpectrum.Blue);
		}
		
		public void BrightnessClicked (int y)
		{
			colorSaturation.ClickProcessing (0);
			colorSpectrum.ClickProcessing (y);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y);
			colorViewHandler.SetupColor (colorSpectrum.Red, 
			                             colorSpectrum.Green,
			                             colorSpectrum.Blue);
			colorScalesHandler.SetupScales (colorSpectrum.Red, 
			                                colorSpectrum.Green,
			                                colorSpectrum.Blue);
			colorCodeHandler.SetupCode (colorViewHandler.Color);
			alphaColorCodeHandler.SetupCode(colorViewHandler.Color,
			                                (ushort)colorScalesHandler.Alpha);
			alphaView.ClickProcessing(colorSpectrum.Red, 
			                          colorSpectrum.Green,
			                          colorSpectrum.Blue);
		}
		
		public void SaturationClicked()
		{
			colorBrightness.ClickProcessing (ColorBrightness.HEIGHT / 2);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y);
			colorViewHandler.SetupColor (colorSaturation.Red, 
			                             colorSaturation.Green,
			                             colorSaturation.Blue);
			colorScalesHandler.SetupScales (colorSaturation.Red, 
			                                colorSaturation.Green,
			                                colorSaturation.Blue);
			colorCodeHandler.SetupCode (colorViewHandler.Color);
			alphaColorCodeHandler.SetupCode(colorViewHandler.Color,
			                                (ushort)colorScalesHandler.Alpha);
			alphaView.ClickProcessing(colorSaturation.Red, 
			                          colorSaturation.Green,
			                          colorSaturation.Blue);
		}
		
		public void SquareClicked()
		{
			colorSpectrum.ClickProcessing (colorSquare.Red,
			                               colorSquare.Green,
			                               colorSquare.Blue);
			colorSaturation.ClickProcessing (colorSquare.ArcX);
			colorBrightness.ClickProcessing (colorSquare.ArcY);
			colorViewHandler.SetupColor (colorSquare.Red, 
			                             colorSquare.Green,
			                             colorSquare.Blue);
			colorScalesHandler.SetupScales (colorSquare.Red, 
			                                colorSquare.Green,
			                                colorSquare.Blue);
			colorCodeHandler.SetupCode (colorViewHandler.Color);
			alphaColorCodeHandler.SetupCode(colorViewHandler.Color,
			                                (ushort)colorScalesHandler.Alpha);
			alphaView.ClickProcessing(colorSquare.Red, 
			                          colorSquare.Green,
			                          colorSquare.Blue);
		}
		
		public static ClickEventHandler Inst 
		{
			get 
			{
				return inst;
			}
		}
	}
}

