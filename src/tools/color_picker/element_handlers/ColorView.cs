// \author		Kuzmin Maxim aka eshkin_kot
// \date		18.12.2012 (c)Andrey Korotkov
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
		private static ColorView inst;
		private Color color = new Gdk.Color();
		private DrawingArea colorView;
		private ushort red = 255, green = 255, blue = 255, alpha = 255;

		public ColorView (DrawingArea colorView, string colorCode)
		{
			this.colorView = colorView;
			setupColor (colorCode);
			inst = this;
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
		}
		
		public string Color 
		{
			get 
			{
				return "#" + red.ToString("X2") + green.ToString("X2") + 
					blue.ToString("X2");
			}
		}
		
		public ushort Red 
		{
			get 
			{
				return red;
			}
			set 
			{
				setupColor(ref red, ref color.Red, value);
				updateColor();
			}
		}
		
		public ushort Green 
		{
			get 
			{
				return green;
			}
			set 
			{
				setupColor(ref green, ref color.Green, value);
				updateColor();
			}
		}
		
		public ushort Blue 
		{
			get
			{
				return blue;
			}
			set 
			{
				setupColor(ref blue, ref color.Blue, value);
				updateColor();
			}
		}
		
		public ushort Alpha 
		{
			get 
			{
				return alpha;
			}
			set 
			{
				alpha = value;
			}
		}
		
		private void setupColor(string colorCode)
		{
			Gdk.Color.Parse(colorCode, ref color);
			setupShortValues();
			updateColor ();
		}

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
	
		public static ColorView Inst 
		{
			get 
			{
				return inst;
			}
		}
	}
}

