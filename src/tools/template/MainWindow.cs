// \author		Shestakov Mikhail aka MIKE
// \date		20.9.2012 (c) Shestakov Mikhail
//
// This file is a part of DGLE2 project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE2.h" for more details.
using System;
using Gtk;

public partial class MainWindow: Gtk.Window
{
	// Resizing helper
	private GuiWidgets.SizingHelper _sizingHelper;

	public MainWindow (): base (Gtk.WindowType.Toplevel)
	{
		Build ();

		// nonclient window border width
		uint borderWidth = 4;
		this.eventboxClient.BorderWidth = borderWidth;
		// init resizing hepler with gtk window and border widget
		_sizingHelper = new GuiWidgets.SizingHelper(this, this.eventboxNonClient, borderWidth);
		// Uncheck in window property or in code directly
		this.Decorated = false;
	}
	
	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}

	protected void OnExitAction1Activated (object sender, EventArgs e)
	{
		Application.Quit();
	}
}