/**
\author		Shestakov Mikhail aka MIKE
\date		23.9.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;

namespace Template
{
	public partial class AboutWindow : Gtk.Window
	{
		// Resizing helper
		private GuiWidgets.SizingHelper _sizingHelper;

		public AboutWindow() : 
				base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			// nonclient window border width
			uint borderWidth = 4;
			this.eventboxClient.BorderWidth = borderWidth;
			// init resizing hepler with gtk window and border widget
			_sizingHelper = new GuiWidgets.SizingHelper(this, this.eventboxNonClient, false, borderWidth);
			// Uncheck in window property or in code directly
			this.Decorated = false;

			this.DeleteEvent += HandleDeleteEvent;
		}


		protected void HandleDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{

		}
	}
}

