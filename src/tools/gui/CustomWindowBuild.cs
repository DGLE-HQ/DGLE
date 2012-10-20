/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using System.Reflection;

namespace Gui
{
	public partial class CustomWindow
	{
		private Gtk.VBox vboxWindow;
		private Gtk.EventBox evntboxTitleBar;
		private Gtk.VBox vboxClient;
		private Gtk.HBox hboxTitleBar;
		private Gtk.Label labelCaption;
		private Gtk.Button btnMinimize;
		private Gtk.Button btnMaximize;
		private Gtk.Button btnClose;

		public void CustomBuild ()
		{
			this.vboxWindow = new Gtk.VBox ();
			this.vboxWindow.Name = "vboxWindow";
			
			this.evntboxTitleBar = new Gtk.EventBox ();
			this.evntboxTitleBar.Name = "evntboxTitleBar";
			this.evntboxTitleBar.Visible = false;
			this.evntboxTitleBar.ChildVisible = false;
			this.evntboxTitleBar.Sensitive = false;
			this.vboxWindow.PackStart(this.evntboxTitleBar, false, false, 0);
			
			this.vboxClient = new Gtk.VBox ();
			this.vboxClient.Name = "vboxClient";
			this.vboxWindow.PackStart(this.vboxClient, true, true, 0);
			
			this.hboxTitleBar = new Gtk.HBox ();
			this.hboxTitleBar.Name = "hboxTitleBar";
			this.evntboxTitleBar.Add (this.hboxTitleBar);
			
			this.labelCaption = new Gtk.Label ();
			this.labelCaption.Name = "labelCaption";
			this.labelCaption.Xalign = 0f;
			this.labelCaption.LabelProp = Mono.Unix.Catalog.GetString ("Title");
			this.labelCaption.SingleLineMode = true;
			this.hboxTitleBar.PackStart (this.labelCaption, true, true, 0);
			
			this.btnMinimize = new Gtk.Button ();
			this.btnMinimize.Name = "btnMinimize";
			this.btnMinimize.WidthRequest = 22;
			this.btnMinimize.HeightRequest = 22;
			this.btnMinimize.CanFocus = false;
			this.hboxTitleBar.PackStart (this.btnMinimize, false, false, 0);
			
			this.btnMaximize = new Gtk.Button ();
			this.btnMaximize.Name = "btnMaximize";
			this.btnMaximize.WidthRequest = 22;
			this.btnMaximize.HeightRequest = 22;
			this.btnMaximize.CanFocus = false;
			this.hboxTitleBar.PackStart (this.btnMaximize, false, false, 0);
			
			this.btnClose = new Gtk.Button ();
			this.btnClose.Name = "btnClose";
			this.btnClose.WidthRequest = 22;
			this.btnClose.HeightRequest = 22;
			this.btnClose.CanFocus = false;
			this.hboxTitleBar.PackStart (this.btnClose, false, false, 0);

			this.btnMinimize.Image = new Gtk.Image(Assembly.GetExecutingAssembly(), ThemeHelper.minimizeBtnId);
			this.btnMaximize.Image = new Gtk.Image(Assembly.GetExecutingAssembly(), ThemeHelper.maximizeBtnId);
			this.btnClose.Image = new Gtk.Image(Assembly.GetExecutingAssembly(), ThemeHelper.closeBtnId);

		}
	}
}

