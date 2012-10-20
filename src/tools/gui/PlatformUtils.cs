/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using System.Runtime.InteropServices;

namespace Gui
{
	// from mono develop
	public static class PlatformUtils
	{
		static PlatformUtils ()
		{
			IsWindows = System.IO.Path.DirectorySeparatorChar == '\\';
			IsMac = !IsWindows && IsRunningOnMac();
			IsX11 = !IsMac && System.Environment.OSVersion.Platform == PlatformID.Unix;
		}
		
		static Gdk.Keymap keymap = Gdk.Keymap.Default;

		// for mono 2.0 compatibility
		private static bool isMac;
		public static bool IsMac { get { return isMac; } private set { isMac = value; } }
		private static bool isX11;
		public static bool IsX11 { get { return isX11; } private set { isX11 = value; } }
		private static bool isWindows;
		public static bool IsWindows { get { return isWindows; } private set { isWindows = value; } }
		
		//From Managed.Windows.Forms/XplatUI
		static bool IsRunningOnMac ()
		{
			IntPtr buf = IntPtr.Zero;
			try {
				buf = Marshal.AllocHGlobal (8192);
				// This is a hacktastic way of getting sysname from uname ()
				if (uname (buf) == 0) {
					string os = Marshal.PtrToStringAnsi (buf);
					if (os == "Darwin")
						return true;
				}
			} catch {
			} finally {
				if (buf != IntPtr.Zero)
					Marshal.FreeHGlobal (buf);
			}
			
			return false;
		}
		
		[DllImport ("libc")]
		static extern int uname (IntPtr buf);
	}
}