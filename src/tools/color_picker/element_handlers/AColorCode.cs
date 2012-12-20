// \author		Макс
// \date		20.12.2012 (c)Макс
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gtk;

namespace ColorPicker
{
	public class AColorCode
	{
		private static AColorCode inst;
		private Entry aColorCode;
		private bool isItEventOrigin = true;

		public AColorCode(Entry aColorCode)
		{
			this.aColorCode = aColorCode;
			inst = this;
		}

		public bool IsItEventOrigin {
			get {
				return isItEventOrigin;
			}
			set {
				isItEventOrigin = value;
			}
		}

		public void SetupCode(string code, ushort alpha)
		{
			isItEventOrigin = false;
			string alphaChannel = alpha.ToString("X2");
			aColorCode.Text = code + alphaChannel;
		}

		public static AColorCode Inst {
			get {
				return inst;
			}
		}
	}
}

