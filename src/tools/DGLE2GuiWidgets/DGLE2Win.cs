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
	public partial class DGLE2Win : Gtk.Bin
	{
		private SizingHelper _sizingHelper;
		private MovingHelper _movingHelper;

		public DGLE2Win () : base()
		{
			Build ();
			
			// set parent window decorations
			if (null != this.ParentWindow)
				this.ParentWindow.SetDecorations(Gdk.WMDecoration.Menu);
			
			// window border width
			uint borderWidth = 4;
			this.eventboxClient.BorderWidth = borderWidth;
			
			// sizing helper
			_sizingHelper = new SizingHelper(this, this.eventboxNonClient, borderWidth);
			
			// moving helper
			_movingHelper = new MovingHelper(this.Toplevel);
			
			
			
			// temporary
			Gdk.Color bg = Gdk.Color.Zero;
			Gdk.Color.Parse("black", ref bg);
			this.eventboxNonClient.ModifyBg(Gtk.StateType.Normal, bg);

			// temporary
			Gdk.Color bgc = Gdk.Color.Zero;
			Gdk.Color.Parse("#262729", ref bgc);
			Gdk.Color fgc = Gdk.Color.Zero;
			Gdk.Color.Parse("#e04613", ref fgc);
			this.eventboxCaption.ModifyBg(Gtk.StateType.Normal, bgc);
			this.labelCaption.ModifyFg(Gtk.StateType.Normal, fgc);


			
			this.eventboxNonClient.AddEvents((int) Gdk.EventMask.PointerMotionMask);
			
			this.eventboxNonClient.EnterNotifyEvent += eventboxNonClient_HandleEnterNotifyEvent;
			this.eventboxNonClient.LeaveNotifyEvent += eventboxNonClient_HandleLeaveNotifyEvent;
			this.eventboxNonClient.ButtonPressEvent += eventboxNonClient_HandleButtonPressEvent;
			this.eventboxNonClient.ButtonReleaseEvent += eventboxNonClient_HandleButtonReleaseEvent;
			this.eventboxNonClient.MotionNotifyEvent += eventboxNonClient_HandleMotionNotifyEvent;
			
			this.eventboxCaption.ButtonPressEvent += eventboxCaption_HandleButtonPressEvent;
			this.eventboxCaption.ButtonReleaseEvent += eventboxCaption_HandleButtonReleaseEvent;
			this.eventboxCaption.MotionNotifyEvent += eventboxCaption_HandleMotionNotifyEvent;

			this.buttonMinimize.Clicked += buttonMinimize_HandleClicked;
			this.buttonMaximize.Clicked += buttonMaximize_HandleClicked;;
			this.buttonClose.Clicked += buttonClose_HandleClicked;
		}

		private void buttonClose_HandleClicked (object sender, EventArgs e)
		{
			Gtk.Application.Quit();
		}

		private void buttonMaximize_HandleClicked (object sender, EventArgs e)
		{
			if (this.ParentWindow.State == Gdk.WindowState.Maximized) {
				this.ParentWindow.Unmaximize();
			} else {
				this.ParentWindow.Maximize();
			}
		}

		private void buttonMinimize_HandleClicked (object sender, EventArgs e)
		{
			if (this.ParentWindow.State == Gdk.WindowState.Iconified) {
				this.ParentWindow.Raise();
			} else {
				this.ParentWindow.Iconify();
			}
		}
		
		
		private void eventboxNonClient_HandleEnterNotifyEvent(object o, Gtk.EnterNotifyEventArgs args)
		{
			// not sizing
			if (this.ParentWindow.State != Gdk.WindowState.Maximized) {
				if (!_sizingHelper.IsSizing) {
					Gdk.Point localMousePos = new Gdk.Point((int)args.Event.X, (int)args.Event.Y);
					_sizingHelper.SelectSizingSide(localMousePos);
					_sizingHelper.SelectCursor();
				}
			}
		}
		
		private void eventboxNonClient_HandleLeaveNotifyEvent (object o, Gtk.LeaveNotifyEventArgs args)
		{
			// not sizing
			if (!_sizingHelper.IsSizing)
				_sizingHelper.ResetCursor();
		}
		
		private void eventboxNonClient_HandleButtonPressEvent(object o, Gtk.ButtonPressEventArgs args)
		{
			// start resizing
			if (this.ParentWindow.State != Gdk.WindowState.Maximized) {
				if (args.Event.Type == Gdk.EventType.ButtonPress &&
					args.Event.Button == 1) {
					_sizingHelper.StartResize();
				}
			}
		}
		
		private void eventboxNonClient_HandleButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
		{
			// stop resizing
			_sizingHelper.StopResize();
		}
		
		
		private void eventboxNonClient_HandleMotionNotifyEvent(object o, Gtk.MotionNotifyEventArgs args)
		{
			// not sizing
			if (this.ParentWindow.State != Gdk.WindowState.Maximized) {
				if (!_sizingHelper.IsSizing) {
					// local mouse pos
					Gdk.Point localMousePos = new Gdk.Point((int)args.Event.X, (int)args.Event.Y);
					_sizingHelper.SelectSizingSide(localMousePos);
				}
				_sizingHelper.SelectCursor();
				
				// resizing
				if (_sizingHelper.IsSizing) {
					_sizingHelper.Resize();
				}
			}
		}
		
		private void eventboxCaption_HandleButtonPressEvent (object o, Gtk.ButtonPressEventArgs args)
		{
			// start moving
			if (args.Event.Type == Gdk.EventType.ButtonPress &&
			    args.Event.Button == 1) {
				_movingHelper.StartMove();
			}

			if (args.Event.Type == Gdk.EventType.TwoButtonPress &&
			    args.Event.Button == 1) {
				if (this.ParentWindow.State == Gdk.WindowState.Maximized) {
					this.ParentWindow.Unmaximize();
				} else {
					this.ParentWindow.Maximize();
				}
			}
		}
		
		private void eventboxCaption_HandleButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
		{
			// stop move
			_movingHelper.StopMove();
		}
		
		private void eventboxCaption_HandleMotionNotifyEvent (object o, Gtk.MotionNotifyEventArgs args)
		{
			// move
			if (_movingHelper.IsMoving)
				_movingHelper.Move();
		}
	}

}

