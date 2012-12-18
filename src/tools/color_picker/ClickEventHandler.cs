// \author		Макс
// \date		18.12.2012 (c)Макс
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
		private ColorScales colorScalesHandler;
		
		public ClickEventHandler (ColorSpectrum colorSpectrum, ColorBrightness colorBrightness,
		                          ColorSaturation colorSaturation, ColorSquare colorSquare, ColorView colorViewHandler,
		                          ColorCode colorCodeHandler, ColorScales colorScalesHandler)
		{
			this.colorSpectrum = colorSpectrum;
			this.colorBrightness = colorBrightness;
			this.colorSaturation = colorSaturation;
			this.colorSquare = colorSquare;
			this.colorViewHandler = colorViewHandler;
			this.colorCodeHandler = colorCodeHandler;
			this.colorScalesHandler = colorScalesHandler;
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
		}
		
		public void BrightnessClicked (int y)
		{
			colorSpectrum.ClickProcessing (y);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y);
			colorViewHandler.SetupColor (colorSpectrum.Red, 
			                             colorSpectrum.Green,
			                             colorSpectrum.Blue);
			colorScalesHandler.SetupScales (colorSpectrum.Red, 
			                                colorSpectrum.Green,
			                                colorSpectrum.Blue);
			colorCodeHandler.SetupCode (colorViewHandler.Color);
		}
		
		public void SaturationClicked()
		{
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y);
			colorViewHandler.SetupColor (colorSaturation.Red, 
			                             colorSaturation.Green,
			                             colorSaturation.Blue);
			colorScalesHandler.SetupScales (colorSaturation.Red, 
			                                colorSaturation.Green,
			                                colorSaturation.Blue);
			colorCodeHandler.SetupCode (colorViewHandler.Color);
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
		}
		
		public static ClickEventHandler Inst {
			get {
				return inst;
			}
		}
	}
}

