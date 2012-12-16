/**
\author		Shestakov Mikhail aka MIKE
\date		15.12.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Runtime.InteropServices;

namespace DCPPacker
{
	public class MarshalUtils
	{
		public static string MarshalString(System.Func<IntPtr, uint, uint> action, uint capacity = 512) {
			
			string marshalString = "";
			IntPtr pnt = IntPtr.Zero;
			try {
				uint length = capacity;
				int size = Marshal.SystemDefaultCharSize * (int)length;
				pnt = Marshal.AllocHGlobal(size);
				length = action(pnt, length);
				if (length > 0 && !IntPtr.Zero.Equals(pnt))
					marshalString = Marshal.PtrToStringAnsi(pnt);
			} catch {
			} finally {
				Marshal.FreeHGlobal(pnt);
			}
			
			return marshalString;
		}
	}
}

