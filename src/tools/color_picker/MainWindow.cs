// \author		Макс
// \date		18.12.2012 (c)Макс
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gtk;
using ColorPicker;

public partial class MainWindow : 
	Gui.CustomWindow
{
	private static readonly string TITLE = "Color Picker", DEFAULT_COLOR_CODE = "#FFFFFF";
	private ColorView colorViewHandler;
	private ColorCode colorCodeHandler;
	private ColorScales colorScalesHandler;
	private ClickEventHandler clickEventHandler;
	
	public MainWindow () : 
		base (Gtk.WindowType.Toplevel)
	{
		this.Build();

		colorViewHandler = new ColorView(colorView, DEFAULT_COLOR_CODE);
		colorCodeHandler = new ColorCode(colorCode);
		colorScalesHandler = new ColorScales(redScale, greenScale, 
		                                     blueScale,
		                                     alphaScale);
		clickEventHandler = new ClickEventHandler(colorSpectrum, colorBrightness, 
		                                          colorSaturation, colorSquare, 
		                                          colorViewHandler, colorCodeHandler, 
		                                          colorScalesHandler);

		base.Decorated = false;
		base.Resizable = false;
		base.Title = TITLE;
	}
	
	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
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
			updateOnScaleChange();
		} else {
			colorScalesHandler.IsRedEventOrigin = true;
		}
	}
	
	
	protected void OnGreenScaleValueChanged (object sender, EventArgs e)
	{
		if(colorScalesHandler.IsGreenEventOrigin) {
			colorViewHandler.Green = (ushort)greenScale.Value;
			updateOnScaleChange();
		} else {
			colorScalesHandler.IsGreenEventOrigin = true;
		}
	}
	
	protected void OnBlueScaleValueChanged (object sender, EventArgs e)
	{
		if(colorScalesHandler.IsBlueEventOrigin) {
			colorViewHandler.Blue = (ushort)blueScale.Value;
			updateOnScaleChange();
		} else {
			colorScalesHandler.IsBlueEventOrigin = true;
		}
	}

	protected void OnCloseActionActivated (object sender, EventArgs e)
	{
		Gtk.Application.Quit ();
	}

	private void updateOnScaleChange()
	{
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
	}
	
	/*protected void OnAlphaScaleValueChanged (object sender, EventArgs e)
	{
		colorViewHandler.Alpha = (ushort)alphaScale.Value;
	}*/
}
