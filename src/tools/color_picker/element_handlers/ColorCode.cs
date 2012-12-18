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
	public class ColorCode
	{
		private Entry colorCode;
		private bool isItEventOrigin = true;
		
		public ColorCode (Entry colorCodeEntry)
		{
			this.colorCode = colorCodeEntry;
		}
		
		public bool IsItEventOrigin {
			set {
				isItEventOrigin = value;
			}
			get {
				return isItEventOrigin;
			}
		}
		
		public void SetupCode(string code)
		{
			isItEventOrigin = false;
			colorCode.Text = code;
		}
	}
}

