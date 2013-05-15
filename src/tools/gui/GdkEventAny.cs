/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Runtime.InteropServices;

namespace Gui
{
	[StructLayout(LayoutKind.Sequential)]
	internal struct GdkEventAny
	{
		// GdkEventType
		public int type;
		// GdkWindow
		public IntPtr window;
		public sbyte sendEvent;
		
		public Gdk.EventType Type
		{
			set { type = (int)value; }
			get { return (Gdk.EventType)type; }
		}
		
		public Gdk.Window Window
		{
			set { window = value == null ? IntPtr.Zero : value.Handle; }
			get { return window == IntPtr.Zero ? null : new Gdk.Window(window); }
		}
		
		public bool SendEvent
		{
			set { sendEvent = value ? (sbyte)1 : (sbyte)0; }
			get { return sendEvent != 0; }
		}
	};
}