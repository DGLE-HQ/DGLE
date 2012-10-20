/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
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
		
		public static Dictionary<SizingSideType, Gdk.CursorType> CursorTypeMaps = GetCurstorTypeMaps ();
		// static initialization doesn't work on .net 2.0
		private static Dictionary<SizingSideType, Gdk.CursorType> GetCurstorTypeMaps ()
		{
			Dictionary<SizingSideType, Gdk.CursorType> сursorTypeMaps = 
				new Dictionary<SizingSideType, Gdk.CursorType>();
			сursorTypeMaps.Add (SizingSideType.E_NONE, Gdk.CursorType.Arrow);
			сursorTypeMaps.Add (SizingSideType.E_LEFT_SIDE, Gdk.CursorType.LeftSide);
			сursorTypeMaps.Add (SizingSideType.E_RIGHT_SIDE, Gdk.CursorType.RightSide);
			сursorTypeMaps.Add (SizingSideType.E_TOP_SIDE, Gdk.CursorType.TopSide);
			сursorTypeMaps.Add (SizingSideType.E_TOP_LEFT_CONER, Gdk.CursorType.TopLeftCorner);
			сursorTypeMaps.Add (SizingSideType.E_TOP_RIGHT_CONER, Gdk.CursorType.TopRightCorner);
			сursorTypeMaps.Add (SizingSideType.E_BOTTOM_SIDE, Gdk.CursorType.BottomSide);
			сursorTypeMaps.Add (SizingSideType.E_BOTTOM_LEFT_CONER, Gdk.CursorType.BottomLeftCorner);
			сursorTypeMaps.Add (SizingSideType.E_BOTTOM_RIGHT_CONER, Gdk.CursorType.BottomRightCorner);
			
			return сursorTypeMaps;
		}
	}
}

