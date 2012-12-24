// \author		Kuzmin Maxim aka eshkin_kot
// \date		18.12.2012 (c)Andrey Korotkov
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gdk;

namespace ColorPicker
{
	public static class SpectrumVerticalPixbuf
	{
		private static Pixbuf pixBuff;
		
		static SpectrumVerticalPixbuf ()
		{
			pixBuff = new Pixbuf(Gdk.Colorspace.Rgb, false, 8, 256, 128);
			pixBuff.Fill(0xffffff);
		}
		
		public static Pixbuf Pixbuf 
		{
			get 
			{
				return pixBuff;
			}
			set 
			{
				pixBuff = value;
			}
		}
	}
}

