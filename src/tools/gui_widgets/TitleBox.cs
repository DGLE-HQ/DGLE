/**
\author		Shestakov Mikhail aka MIKE
\date		21.9.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;

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

		[GLib.Property ("title")]
		public string Title {
			get {
				return this.labelCaption.Text;
			} set {
				this.labelCaption.LabelProp = value;
				this.labelCaption.Text = value;
			}
		}

		public TitleBox()
		{
			this.Build();

			this.eventboxCaption.ButtonPressEvent += eventboxCaption_HandleButtonPressEvent;
			this.eventboxCaption.ButtonReleaseEvent += eventboxCaption_HandleButtonReleaseEvent;
			this.eventboxCaption.MotionNotifyEvent += eventboxCaption_HandleMotionNotifyEvent;
			
			this.buttonMinimize.Clicked += buttonMinimize_HandleClicked;
			this.buttonMaximize.Clicked += buttonMaximize_HandleClicked;;
			this.buttonClose.Clicked += buttonClose_HandleClicked;

			// Set style for title box from resource style
			this.StyleSet+= delegate(object o, Gtk.StyleSetArgs args) {
				Gdk.Color bgc = this.Style.Background(Gtk.StateType.Selected);
				this.eventboxCaption.ModifyBg(Gtk.StateType.Normal, bgc);
				this.eventboxButtons.ModifyBg(Gtk.StateType.Normal, bgc);

				Gdk.Color fgc = this.Style.Base(Gtk.StateType.Active);
				this.labelCaption.ModifyFg(Gtk.StateType.Normal, fgc);
			};

			// Init moving helper with top level window
			this.Realized += delegate(object sender, EventArgs e) {
				_movingHelper = new MovingHelper(this.Toplevel);
				MovingHelper.Window.GdkWindow.Title = this.labelCaption.Text;
			};
		}

		private void SetBg()
		{

		}

		private void buttonClose_HandleClicked (object sender, EventArgs e)
		{
			Gtk.Application.Quit();
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

		private void eventboxCaption_HandleButtonPressEvent(object o, Gtk.ButtonPressEventArgs args)
		{
			// start moving
			if (MovingHelper.Window.GdkWindow.State != Gdk.WindowState.Maximized) {
				if (args.Event.Type == Gdk.EventType.ButtonPress &&
					args.Event.Button == 1) {
					MovingHelper.StartMove();
				}
			}
			
			if (args.Event.Type == Gdk.EventType.TwoButtonPress &&
			    args.Event.Button == 1) {
				if (MovingHelper.Window.GdkWindow.State == Gdk.WindowState.Maximized) {
					MovingHelper.Window.GdkWindow.Unmaximize();
				} else {
					MovingHelper.Window.GdkWindow.Maximize();
				}
			}
		}
		
		private void eventboxCaption_HandleButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
		{
			// stop move
			MovingHelper.StopMove();
		}
		
		private void eventboxCaption_HandleMotionNotifyEvent (object o, Gtk.MotionNotifyEventArgs args)
		{
			// move
			if (MovingHelper.IsMoving)
				MovingHelper.Move();
		}
	}
}

