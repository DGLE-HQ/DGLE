/**
\author		Shestakov Mikhail aka MIKE
\date		20.9.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using Gtk;

public partial class MainWindow: Gtk.Window
{
	// Resizing helper
	private GuiWidgets.SizingHelper _sizingHelper;
	private Template.AboutWindow about_win = new Template.AboutWindow();

	public MainWindow(): base (Gtk.WindowType.Toplevel)
	{
		Build();

		// nonclient window border width
		uint borderWidth = 4;
		this.eventboxClient.BorderWidth = borderWidth;
		// init resizing hepler with gtk window and border widget
		_sizingHelper = new GuiWidgets.SizingHelper(this, this.eventboxNonClient, true, borderWidth);
		// Uncheck in window property or in code directly
		this.Decorated = false;

		about_win.Modal = true;
		about_win.TransientFor = this;
		about_win.Hide();

		string[] paths = Gtk.Rc.DefaultFiles;
		foreach (string path in paths) {
			this.textview3.Buffer.Text += Environment.NewLine + path;
		}

		this.textview3.Buffer.Text += Environment.NewLine + Gtk.Settings.Default.ThemeName;
		this.textview3.Buffer.Text += Environment.NewLine + Gtk.Settings.Default.KeyThemeName;
	}
	
	protected void OnDeleteEvent (object sender, DeleteEventArgs a)
	{
		Application.Quit ();
		a.RetVal = true;
	}

	protected void OnAboutActionActivated(object sender, EventArgs e)
	{
		Gdk.Rectangle rect_parent = GetRect(this.GdkWindow);
		Gdk.Rectangle rect_child = GetRect(about_win.GdkWindow);
		CenterOnParent(ref rect_child, rect_parent);
		about_win.Move(rect_child.X, rect_child.Y);
		about_win.Show();
	}

	private static Gdk.Rectangle GetRect(Gdk.Window window)
	{
		Gdk.Rectangle rect = Gdk.Rectangle.Zero;
		int vx, vy;
		window.GetPosition(out vx, out vy);
		rect.X = vx;
		rect.Y = vy;
		window.GetSize(out vx, out vy);
		rect.Width = vx;
		rect.Height = vy;
		return rect;
	}
	private static void CenterOnParent(ref Gdk.Rectangle child, Gdk.Rectangle parent)
	{
		child.X = parent.X + parent.Width / 2 - child.Width / 2;
		child.Y = parent.Y + parent.Height / 2 - child.Height / 2;
	}

	protected void OnQuitActionActivated (object sender, EventArgs e)
	{
		Application.Quit();
	}
}