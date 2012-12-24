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
	public class ColorCode
	{
		private static ColorCode inst;
		private Entry colorCode;
		private bool isItEventOrigin = true;
		
		public ColorCode (Entry colorCode)
		{
			this.colorCode = colorCode;
			inst = this;
		}
		
		public bool IsItEventOrigin 
		{
			set 
			{
				isItEventOrigin = value;
			}
			get 
			{
				return isItEventOrigin;
			}
		}
		
		public void SetupCode(string code)
		{
			isItEventOrigin = false;
			colorCode.Text = code;
		}

		public static ColorCode Inst 
		{
			get 
			{
				return inst;
			}
		}
	}
}

