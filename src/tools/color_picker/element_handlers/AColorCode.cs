// \author		Kuzmin Maxim aka eshkin_kot
// \date		20.12.2012 (c)Andrey Korotkov
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
		private bool isItEventOrigin = true, valueIsCorrect = true;
		private string color;
		private ushort alpha;

		public AColorCode(Entry aColorCode)
		{
			this.aColorCode = aColorCode;
			inst = this;
		}

		public void SetupCode(string code, ushort alpha)
		{
			isItEventOrigin = false;
			string alphaChannel = alpha.ToString("X2");
			aColorCode.Text = code + alphaChannel;
		}

		public void ParseValue()
		{
			if ((aColorCode.Text.Length == 9) && (aColorCode.Text.Substring(0,1) == "#")) 
			{
				valueIsCorrect = true;
				string alphaCode = aColorCode.Text.Substring(7, 2);

				try 
				{
					int alphaInt = 
						Int32.Parse(alphaCode, System.Globalization.NumberStyles.HexNumber);
					alpha = (ushort)alphaInt;
				} 
				catch (Exception) 
				{
					valueIsCorrect = false;
				}

				color = aColorCode.Text.Substring(1, 6);
			} 
			else 
				valueIsCorrect = false;		 
		}

		public static AColorCode Inst 
		{
			get 
			{
				return inst;
			}
		}

		public bool IsItEventOrigin 
		{
			get 
			{
				return isItEventOrigin;
			}
			set 
			{
				isItEventOrigin = value;
			}
		}

		public bool ValueIsCorrect 
		{
			get 
			{
				return valueIsCorrect;
			}
		}

		public ushort Alpha 
		{
			get 
			{
				return alpha;
			}
		}

		public string Color 
		{
			get 
			{
				return "#" + color;
			}
		}
	}
}

