// \author		Макс
// \date		18.12.2012 (c)Макс
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gtk;
using ColorPicker;

public partial class MainWindow: Gtk.Window
{
	private static readonly string DEFAULT_COLOR_CODE = "#FFFFFF";
	private ColorView colorViewHandler;
	private ColorCode colorCodeHandler;
	private ColorScales colorScalesHandler;
	private ClickEventHandler clickEventHandler;
	
	public MainWindow (): base (Gtk.WindowType.Toplevel)
	{
		Build ();
		colorViewHandler = new ColorView(colorView, DEFAULT_COLOR_CODE);
		colorCodeHandler = new ColorCode(colorCode);
		colorScalesHandler = new ColorScales(redScale, greenScale, 
		                                     blueScale,
		                                     alphaScale);
		clickEventHandler = new ClickEventHandler(colorSpectrum, colorBrightness, 
		                                          colorSaturation, colorSquare, 
		                                          colorViewHandler, colorCodeHandler, 
		                                          colorScalesHandler);
	}
	
	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}
	
	protected void OnExposeEvent (object sender, ExposeEventArgs a)
	{
		/*Gdk.Window win = this.GdkWindow;
		Gdk.GC gc = new Gdk.GC(win);
		gc.RgbFgColor = new Gdk.Color(255,50,50);
		gc.RgbBgColor = new Gdk.Color(0,0,0);
		win.DrawLine(gc, 0, 0, 30, 30);*/
	}
	
	protected void OnButtonPressEvent (object sender, Gtk.ButtonPressEventArgs a)
	{
	}
	
	protected void OnColorCodeChanged (object sender, EventArgs e)
	{
		if (colorCodeHandler.IsItEventOrigin) {
			colorViewHandler.SetupColor (colorCode.Text);
			colorScalesHandler.SetupScales (colorViewHandler.Red, 
			                                colorViewHandler.Green,
			                                colorViewHandler.Blue);
			colorSpectrum.ClickProcessing (colorViewHandler.Red, 
			                               colorViewHandler.Green,
			                               colorViewHandler.Blue);
			colorBrightness.ClickProcessing (colorSpectrum.ArcX, 
			                                 colorSpectrum.ArcY);
			colorSaturation.ClickProcessing (colorSpectrum.NativeRed, 
			                                 colorSpectrum.NativeGreen, 
			                                 colorSpectrum.NativeBlue);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y, 
			                            colorBrightness.SpectrX);
		} else {
			colorCodeHandler.IsItEventOrigin = true;
		}
	}
	
	protected void OnRedScaleValueChanged (object sender, EventArgs e)
	{
		if(colorScalesHandler.IsRedEventOrigin) {
			colorViewHandler.Red = (ushort)redScale.Value;
			colorCodeHandler.SetupCode(colorViewHandler.Color);
			colorSpectrum.ClickProcessing (colorViewHandler.Red, 
			                               colorViewHandler.Green,
			                               colorViewHandler.Blue);
			colorBrightness.ClickProcessing (colorSpectrum.ArcX, 
			                                 colorSpectrum.ArcY);
			colorSaturation.ClickProcessing (colorSpectrum.NativeRed, 
			                                 colorSpectrum.NativeGreen, 
			                                 colorSpectrum.NativeBlue);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y, 
			                            colorBrightness.SpectrX);
		} else {
			colorScalesHandler.IsRedEventOrigin = true;
		}
	}
	
	
	protected void OnGreenScaleValueChanged (object sender, EventArgs e)
	{
		if(colorScalesHandler.IsGreenEventOrigin) {
			colorViewHandler.Green = (ushort)greenScale.Value;
			colorCodeHandler.SetupCode(colorViewHandler.Color);
			colorSpectrum.ClickProcessing (colorViewHandler.Red, 
			                               colorViewHandler.Green,
			                               colorViewHandler.Blue);
			colorBrightness.ClickProcessing (colorSpectrum.ArcX, 
			                                 colorSpectrum.ArcY);
			colorSaturation.ClickProcessing (colorSpectrum.NativeRed, 
			                                 colorSpectrum.NativeGreen, 
			                                 colorSpectrum.NativeBlue);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y, 
			                            colorBrightness.SpectrX);
		} else {
			colorScalesHandler.IsGreenEventOrigin = true;
		}
	}
	
	protected void OnBlueScaleValueChanged (object sender, EventArgs e)
	{
		if(colorScalesHandler.IsBlueEventOrigin) {
			colorViewHandler.Blue = (ushort)blueScale.Value;
			colorCodeHandler.SetupCode(colorViewHandler.Color);
			colorSpectrum.ClickProcessing (colorViewHandler.Red, 
			                               colorViewHandler.Green,
			                               colorViewHandler.Blue);
			colorBrightness.ClickProcessing (colorSpectrum.ArcX, 
			                                 colorSpectrum.ArcY);
			colorSaturation.ClickProcessing (colorSpectrum.NativeRed, 
			                                 colorSpectrum.NativeGreen, 
			                                 colorSpectrum.NativeBlue);
			colorSquare.ClickProcessing(colorSaturation.X, colorBrightness.Y, 
			                            colorBrightness.SpectrX);
		} else {
			colorScalesHandler.IsBlueEventOrigin = true;
		}
	}
	
	/*protected void OnAlphaScaleValueChanged (object sender, EventArgs e)
	{
		colorViewHandler.Alpha = (ushort)alphaScale.Value;
	}*/
}
