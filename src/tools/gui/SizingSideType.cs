/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Collections.Generic;

namespace Gui
{
	public static class SizingSide
	{
		public enum SizingSideType
		{
			E_NONE = 0,
			E_LEFT_SIDE = 1,
			E_RIGHT_SIDE = 2,
			E_TOP_SIDE = 4,
			E_TOP_LEFT_CONER = 5,
			E_TOP_RIGHT_CONER = 6,
			E_BOTTOM_SIDE = 8,
			E_BOTTOM_LEFT_CONER = 9,
			E_BOTTOM_RIGHT_CONER = 10
		}
		
		public static readonly Dictionary<SizingSideType, Gdk.CursorType> CursorTypeMaps = new Dictionary<SizingSideType, Gdk.CursorType>()
		{
			{ SizingSideType.E_NONE, Gdk.CursorType.Arrow },
			{ SizingSideType.E_LEFT_SIDE, Gdk.CursorType.LeftSide },
			{ SizingSideType.E_RIGHT_SIDE, Gdk.CursorType.RightSide },
			{ SizingSideType.E_TOP_SIDE, Gdk.CursorType.TopSide },
			{ SizingSideType.E_TOP_LEFT_CONER, Gdk.CursorType.TopLeftCorner },
			{ SizingSideType.E_TOP_RIGHT_CONER, Gdk.CursorType.TopRightCorner },
			{ SizingSideType.E_BOTTOM_SIDE, Gdk.CursorType.BottomSide },
			{ SizingSideType.E_BOTTOM_LEFT_CONER, Gdk.CursorType.BottomLeftCorner },
			{ SizingSideType.E_BOTTOM_RIGHT_CONER, Gdk.CursorType.BottomRightCorner }
		};
	}
}

