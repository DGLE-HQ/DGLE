// \author		Shestakov Mikhail aka MIKE
// \date		21.9.2012 (c) Shestakov Mikhail
//
// This file is a part of DGLE2 project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE2.h" for more details.
using System;

namespace DGLE2GuiWidgets
{
	[System.ComponentModel.ToolboxItem(true)]
	public partial class TitleBox : Gtk.Bin
	{
		private MovingHelper _movingHelper;
		private MovingHelper MovingHelper {
			get {

				// TopLevel Window is avaliable after stetic initialization
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

			// temporary
			Gdk.Color bgc = Gdk.Color.Zero;
			Gdk.Color.Parse("#262729", ref bgc);
			Gdk.Color fgc = Gdk.Color.Zero;
			Gdk.Color.Parse("#e04613", ref fgc);
			this.eventboxCaption.ModifyBg(Gtk.StateType.Normal, bgc);
			this.eventboxButtons.ModifyBg(Gtk.StateType.Normal, bgc);
			this.labelCaption.ModifyFg(Gtk.StateType.Normal, fgc);
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

		private void eventboxCaption_HandleButtonPressEvent (object o, Gtk.ButtonPressEventArgs args)
		{
			// start moving
			if (args.Event.Type == Gdk.EventType.ButtonPress &&
			    args.Event.Button == 1) {
				MovingHelper.StartMove();
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

