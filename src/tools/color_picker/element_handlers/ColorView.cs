// \author		Макс
// \date		18.12.2012 (c)Макс
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gtk;
using Gdk;

namespace ColorPicker
{
	public class ColorView
	{
		private Color color = new Gdk.Color();
		private DrawingArea colorView;
		private ushort red = 255, green = 255, blue = 255, alpha = 255;
		/*private Pixbuf pixbuf;
		private Gdk.Window window;
		private Gdk.GC gc;*/
		
		public ColorView (DrawingArea colorView, string colorCode)
		{
			this.colorView = colorView;
			//this.alphaColorView = alphaColorView;
			setupColor (colorCode);
			//setupForAlpha();
		}
		
		public void SetupColor (String colorCode)
		{
			setupColor(colorCode);
		}
		
		public void SetupColor (ushort red, ushort green, ushort blue)
		{
			setupColor(ref this.red, ref color.Red, red);
			setupColor(ref this.green, ref color.Green, green);
			setupColor(ref this.blue, ref color.Blue, blue);
			updateColor();
			//updateAlphaColor();
		}
		
		public string Color {
			get {
				return "#" + red.ToString("X2") + green.ToString("X2") + 
					blue.ToString("X2");
			}
		}
		
		public ushort Red {
			get {
				return red;
			}
			set {
				setupColor(ref red, ref color.Red, value);
				updateColor();
				//updateAlphaColor();
			}
		}
		
		public ushort Green {
			get {
				return green;
			}
			set {
				setupColor(ref green, ref color.Green, value);
				updateColor();
				//updateAlphaColor();
			}
		}
		
		public ushort Blue {
			get {
				return blue;
			}
			set {
				setupColor(ref blue, ref color.Blue, value);
				updateColor();
				//updateAlphaColor();
			}
		}
		
		public ushort Alpha {
			get {
				return alpha;
			}
			set {
				alpha = value;
				//updateAlphaColor();
			}
		}
		
		private void setupColor(string colorCode)
		{
			Gdk.Color.Parse(colorCode, ref color);
			setupShortValues();
			updateColor ();
		}
		
		/*private void setupForAlpha ()
		{
			window = alphaColorView.GdkWindow;
			gc = new Gdk.GC(window);
			pixbuf = new Gdk.Pixbuf(Gdk.Colorspace.Rgb, true, 8, 
			                        alphaColorView.WidthRequest, alphaColorView.HeightRequest);
			alphaColorView.ModifyBg (Gtk.StateType.Normal, color);
		}*/
		
		private void setupColor (ref ushort color, ref ushort refInColor, 
		                         ushort value)
		{
			color = value;
			refInColor = (ushort)((value / 255.0) * 65535);
		}
		
		private void setupShortValues()
		{
			red = (ushort)((color.Red / 65535.0) * 255);
			green = (ushort)((color.Green / 65535.0) * 255);
			blue = (ushort)((color.Blue / 65535.0) * 255);
		}
		
		private void updateColor ()
		{
			colorView.ModifyBg (Gtk.StateType.Normal, color);
		}
		
		/*private void updateAlphaColor()
		{
			pixbuf.Fill((uint)(((red & 0xff) << 24) + ((green & 0xff) << 16) + ((blue & 0xff) << 8) + 0x02));				
			window.DrawPixbuf(gc, pixbuf, 0, 0, 0, 0, alphaColorView.WidthRequest, 
			                  alphaColorView.HeightRequest, 0, 0, 0);
		}*/
	}
}

