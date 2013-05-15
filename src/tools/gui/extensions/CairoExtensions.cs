/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;

namespace Gui
{
	public class CairoExtensions
	{
		public static Cairo.Color AlphaBlend(Cairo.Color ca, Cairo.Color cb, double alpha)
		{
			return new Cairo.Color(
				(1.0 - alpha) * ca.R + alpha * cb.R,
				(1.0 - alpha) * ca.G + alpha * cb.G,
				(1.0 - alpha) * ca.B + alpha * cb.B);
		}
		public static Gdk.Color CairoColorToGdkColor(Cairo.Color color)
		{
			return new Gdk.Color((byte)(color.R * 255), (byte)(color.G * 255), (byte)(color.B * 255));
		}
		
		public static Cairo.Color GdkColorToCairoColor(Gdk.Color color)
		{
			return GdkColorToCairoColor(color, 1.0);
		}
		
		public static Cairo.Color GdkColorToCairoColor(Gdk.Color color, double alpha)
		{
			return new Cairo.Color(
				(double)(color.Red >> 8) / 255.0,
				(double)(color.Green >> 8) / 255.0,
				(double)(color.Blue >> 8) / 255.0,
				alpha);
		}
		
		public static Cairo.Color RgbToColor(uint rgbColor)
		{
			return RgbaToColor((rgbColor << 8) | 0x000000ff);
		}
		
		public static Cairo.Color RgbaToColor(uint rgbaColor)
		{
			return new Cairo.Color(
				(byte)(rgbaColor >> 24) / 255.0,
				(byte)(rgbaColor >> 16) / 255.0,
				(byte)(rgbaColor >> 8) / 255.0,
				(byte)(rgbaColor & 0x000000ff) / 255.0);
		}
	}
}