// \author		Макс
// \date		18.12.2012 (c)Макс
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
		private HScale redScale, greenScale, blueScale, alphaScale;
		private bool isRedEventOrigin = true, isGreenEventOrigin = true, 
		isBlueEventOrigin = true;
		
		public ColorScales (HScale redScale, HScale greenScale, 
		                    HScale blueScale, HScale alphaScale)
		{
			this.redScale = redScale;
			this.greenScale = greenScale;
			this.blueScale = blueScale;
			this.alphaScale = alphaScale;
		}
		
		public void SetupScales (double red, double green, double blue)
		{
			if (redScale.Value != red) {
				isRedEventOrigin = false;
				redScale.Value = red;
			}
			
			if (greenScale.Value != green) {
				isGreenEventOrigin = false;
				greenScale.Value = green;
			}
			
			if (blueScale.Value != blue) {
				isBlueEventOrigin = false;
				blueScale.Value = blue;
			}
		}
		
		public void SetupRedScale (double red)
		{
			isRedEventOrigin = false;
			redScale.Value = red;
		}
		
		public void SetupGreenScale (double green)
		{
			isGreenEventOrigin = false;
			greenScale.Value = green;
		}
		
		public void SetupBlueScale (double blue)
		{
			isBlueEventOrigin = false;
			blueScale.Value = blue;
		}
		
		public bool IsRedEventOrigin {
			get {
				return isRedEventOrigin;
			}
			set {
				isRedEventOrigin = value;
			}
		}
		
		public bool IsGreenEventOrigin {
			get {
				return isGreenEventOrigin;
			}
			set {
				isGreenEventOrigin = value;
			}
		}
		
		public bool IsBlueEventOrigin {
			get {
				return isBlueEventOrigin;
			}
			set {
				isBlueEventOrigin = value;
			}
		}
	}
}

