// \author		Kuzmin Maxim aka eshkin_kot
// \date		18.12.2012 (c)Andrey Korotkov
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gtk;

namespace ColorPicker
{
	public class ColorScales
	{
		private static ColorScales inst;
		private HScale redScale, greenScale, blueScale, alphaScale;
		private bool isRedEventOrigin = true, isGreenEventOrigin = true, 
		isBlueEventOrigin = true, isAlphaEventOrigin = true;
		
		public ColorScales (HScale redScale, HScale greenScale, 
		                    HScale blueScale, HScale alphaScale)
		{
			this.redScale = redScale;
			this.greenScale = greenScale;
			this.blueScale = blueScale;
			this.alphaScale = alphaScale;
			inst = this;
		}

		public void SetupScales (double red, double green, double blue)
		{
			setupScales(red, green, blue, -1);
		}

		public void SetupScales (double red, double green, double blue, double alpha)
		{
			setupScales(red, green, blue, alpha);
		}

		private void setupScales(double red, double green, double blue, double alpha)
		{
			if (redScale.Value != red) 
			{
				isRedEventOrigin = false;
				redScale.Value = red;
			}
			
			if (greenScale.Value != green) 
			{
				isGreenEventOrigin = false;
				greenScale.Value = green;
			}
			
			if (blueScale.Value != blue) 
			{
				isBlueEventOrigin = false;
				blueScale.Value = blue;
			}
			if ((alpha != -1) && (alphaScale.Value != alpha)) 
			{
				isAlphaEventOrigin = false;
				alphaScale.Value = alpha;
			}
		}
		
		public bool IsRedEventOrigin 
		{
			get 
			{
				return isRedEventOrigin;
			}
			set 
			{
				isRedEventOrigin = value;
			}
		}
		
		public bool IsGreenEventOrigin 
		{
			get 
			{
				return isGreenEventOrigin;
			}
			set 
			{
				isGreenEventOrigin = value;
			}
		}
		
		public bool IsBlueEventOrigin 
		{
			get 
			{
				return isBlueEventOrigin;
			}
			set 
			{
				isBlueEventOrigin = value;
			}
		}

		public bool IsAlphaEventOrigin 
		{
			get 
			{
				return isAlphaEventOrigin;
			}
			set 
			{
				isAlphaEventOrigin = value;
			}
		}

		public double Alpha 
		{
			get 
			{
				return alphaScale.Value;
			}
		}

		public static ColorScales Inst 
		{
			get 
			{
				return inst;
			}
		}
	}
}

