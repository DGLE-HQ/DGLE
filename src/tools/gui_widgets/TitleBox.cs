/**
\author		Shestakov Mikhail aka MIKE
\date		21.9.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using System.IO;

namespace GuiWidgets
{
	[System.ComponentModel.ToolboxItem(true)]
	public partial class TitleBox : Gtk.Bin
	{
		private MovingHelper _movingHelper;
		private MovingHelper MovingHelper {
			get {
				if (null == _movingHelper) {
					_movingHelper = new MovingHelper(this.Toplevel);
				}
				return _movingHelper;
			}
		}

		[GLib.Property ("Title")]
		public string Title {
			get {
				return this.labelCaption.Text;
			} set {
				this.labelCaption.LabelProp = value;
				this.labelCaption.Text = value;
			}
		}

		private bool _quitOnClose = true;
		[GLib.Property ("QuitOnClose")]
		public bool QuitOnClose {
			get {
				return _quitOnClose;
			} set {
				_quitOnClose = value;
			}
		}

		[GLib.Property ("MinimizeVisible")]
		private bool _minimize_visible = true;
		public bool MinimizeVisible {
			get {
				return _minimize_visible;
			} set {
				_minimize_visible = value;
			}
		}
		private bool _maximize_visible = true;
		[GLib.Property ("MaximizeVisible")]
		public bool MaximizeVisible {
			get {
				return _maximize_visible;
			} set {
				_maximize_visible = value;
			}
		}

		public TitleBox()
		{
			this.Build();

			this.eventboxTitleBar.ButtonPressEvent += eventboxTitle_HandleButtonPressEvent;
			this.eventboxTitleBar.ButtonReleaseEvent += eventboxTitle_HandleButtonReleaseEvent;
			this.eventboxTitleBar.MotionNotifyEvent += eventboxTitle_HandleMotionNotifyEvent;
			
			this.buttonMinimize.Clicked += buttonMinimize_HandleClicked;
			this.buttonMaximize.Clicked += buttonMaximize_HandleClicked;
			this.buttonClose.Clicked += buttonClose_HandleClicked;

			this.buttonMinimize.Image = ThemingHelper.LoadImageMinBtn();
			this.buttonMaximize.Image = ThemingHelper.LoadImageMaxBtn();
			this.buttonClose.Image = ThemingHelper.LoadImageCloseBtn();

			// Set style for title box from resource style
			this.StyleSet+= delegate(object o, Gtk.StyleSetArgs args) {
				Gdk.Color bgc = this.Style.Background(Gtk.StateType.Selected);
				this.eventboxCaption.ModifyBg(Gtk.StateType.Normal, bgc);
				this.eventboxButtons.ModifyBg(Gtk.StateType.Normal, bgc);
				this.eventboxTitleBarBottom.ModifyBg(Gtk.StateType.Normal, bgc);

				Gdk.Color fgc = this.Style.Base(Gtk.StateType.Active);
				this.labelCaption.ModifyFg(Gtk.StateType.Normal, fgc);
			};

			// Init moving helper with top level window
			this.Realized += delegate(object sender, EventArgs e) {
				_movingHelper = new MovingHelper(this.Toplevel);
				MovingHelper.Window.GdkWindow.Title = this.labelCaption.Text;
			};

			// buttons visibility
			this.buttonMaximize.Realized += delegate(object sender, EventArgs e) {
				this.buttonMaximize.Visible = _maximize_visible;
				this.buttonMaximize.Sensitive = _maximize_visible;
			};
			this.buttonMinimize.Realized += delegate(object sender, EventArgs e) {
				this.buttonMinimize.Visible = _minimize_visible;
				this.buttonMinimize.Sensitive = _minimize_visible;
			};
		}

		private void buttonClose_HandleClicked(object sender, EventArgs e)
		{
			if (_quitOnClose)
				Gtk.Application.Quit();
			else {
				MovingHelper.Window.HideOnDelete();
			}
		}
		
		private void buttonMaximize_HandleClicked (object sender, EventArgs e)
		{
			if (MovingHelper.Window.GdkWindow.State == Gdk.WindowState.Maximized) {
				MovingHelper.Window.GdkWindow.Unmaximize();
			} else {
				MovingHelper.Window.GdkWindow.Maximize();
			}
		}
		
		private void buttonMinimize_HandleClicked (object sender, EventArgs e)
		{
			if (MovingHelper.Window.GdkWindow.State == Gdk.WindowState.Iconified) {
				MovingHelper.Window.GdkWindow.Raise();
			} else {
				MovingHelper.Window.GdkWindow.Iconify();
			}
		}

		private void eventboxTitle_HandleButtonPressEvent(object o, Gtk.ButtonPressEventArgs args)
		{
			// start moving
			if (MovingHelper.Window.GdkWindow.State != Gdk.WindowState.Maximized) {
				if (args.Event.Type == Gdk.EventType.ButtonPress &&
					args.Event.Button == 1) {
					MovingHelper.StartMove();
				}
			}

			// maximize
			if (MaximizeVisible &&
			    args.Event.Type == Gdk.EventType.TwoButtonPress &&
			    args.Event.Button == 1) {
				if (MovingHelper.Window.GdkWindow.State == Gdk.WindowState.Maximized) {
					MovingHelper.Window.GdkWindow.Unmaximize();
				} else {
					MovingHelper.Window.GdkWindow.Maximize();
				}
			}
		}
		
		private void eventboxTitle_HandleButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
		{
			// stop move
			MovingHelper.StopMove();
		}
		
		private void eventboxTitle_HandleMotionNotifyEvent (object o, Gtk.MotionNotifyEventArgs args)
		{
			// move
			if (MovingHelper.IsMoving)
				MovingHelper.Move();
		}
	}
}

