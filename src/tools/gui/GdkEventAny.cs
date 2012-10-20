/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using System.Runtime.InteropServices;

namespace Gui
{
	[StructLayout(LayoutKind.Sequential)]
	internal struct GdkEventAny
	{
		public int type;	// GdkEventType
		public IntPtr window;	// GdkWindow
		public sbyte send_event;
		
		public Gdk.EventType Type
		{
			set { type = (int)value; }
			get { return (Gdk.EventType)type; }
		}
		
		public Gdk.Window Window
		{
			set { window = value == null ? IntPtr.Zero : value.Handle; }
			get { return window == IntPtr.Zero ? null : new Gdk.Window (window); }
		}
		
		public bool SendEvent
		{
			set { send_event = value ? (sbyte)1 : (sbyte)0; }
			get { return send_event != 0; }
		}
	};
}