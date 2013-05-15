/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;

namespace Gui
{
	public partial class CustomWindow : Gtk.Window
	{
		[GLib.Property ("FrameBorderWidth")]
		public int FrameBorderWidth
		{
			get
			{
				return frameBorderWidth;
			}
			set
			{
				frameBorderWidth = value;
				frameBorderMargin = frameBorderWidth > 2 ? 0 : 2;
				frameBorderMargin = 0;
			}
		}

		[GLib.Property ("decorated")]
		new public bool Decorated
		{
			get { return base.Decorated; }
			set
			{
				if (Gui.ThemeHelper.ForceDecoration)
					base.Decorated = true;
				else 
					base.Decorated = value;

				ShowTitleBar = !base.Decorated;
				this.vboxWindow.BorderWidth = base.Decorated ? 0 : (uint)frameBorderWidth;
			}
		}
		//[GLib.Property ("type-hint")]
		new public Gdk.WindowTypeHint TypeHint
		{
			get { return base.TypeHint; }
			set
			{
				base.TypeHint = value;
				switch (base.TypeHint)
				{
					case Gdk.WindowTypeHint.Normal:
						ShowTitleBar = true;
						ShowMinimizeBtn = true;
						ShowMaximizeBtn = true;
						ShowCloseBtn = true;
						break;
					case Gdk.WindowTypeHint.Dialog:
						ShowTitleBar = true;
						ShowMinimizeBtn = false;
						ShowMaximizeBtn = false;
						ShowCloseBtn = true;
						break;
					case Gdk.WindowTypeHint.Utility:
						ShowTitleBar = true;
						ShowMinimizeBtn = false;
						ShowMaximizeBtn = false;
						ShowCloseBtn = true;
						break;
					default:
						ShowTitleBar = false;
						ShowMinimizeBtn = false;
						ShowMaximizeBtn = false;
						ShowCloseBtn = false;
						break;
				}
			}
		}

		[GLib.Property ("title")]
		new public string Title
		{
			get { return base.Title; }
			set
			{
				base.Title = value;
				this.labelCaption.Text = value;
			}
		}

		[GLib.Property ("resizable")]
		new public bool Resizable
		{
			get { return base.Resizable; }
			set
			{
				base.Resizable = value;
				ShowMaximizeBtn = base.Resizable;
			}
		}

		private bool ShowTitleBar
		{
			set
			{
				this.evntboxTitleBar.Visible = value;
				this.evntboxTitleBar.ChildVisible = value;
				this.evntboxTitleBar.Sensitive = value;
				
				Gtk.Box.BoxChild boxChild = ((Gtk.Box.BoxChild)(this.vboxWindow[this.evntboxTitleBar]));
				boxChild.Fill = value;
				
				base.QueueDraw();
			}
		}

		private bool ShowMinimizeBtn
		{
			set
			{
				this.btnMinimize.Visible = value;
				this.btnMinimize.ChildVisible = value;
				this.btnMinimize.Sensitive = value;
				Gtk.Box.BoxChild boxChild = ((Gtk.Box.BoxChild)(this.hboxTitleBar[this.btnMinimize]));
				boxChild.Position = this.btnMinimize.Visible ? 2 : 5;
			}
		}

		private bool ShowMaximizeBtn
		{
			set
			{
				this.btnMaximize.Visible = value;
				this.btnMaximize.ChildVisible = value;
				this.btnMaximize.Sensitive = value;
				Gtk.Box.BoxChild boxChild = ((Gtk.Box.BoxChild)(this.hboxTitleBar[this.btnMaximize]));
				boxChild.Position = this.btnMaximize.Visible ? 1 : 4;
			}
		}

		private bool ShowCloseBtn
		{
			set
			{
				this.btnClose.Visible = value;
				this.btnClose.ChildVisible = value;
				this.btnClose.Sensitive = value;
				Gtk.Box.BoxChild boxChild = ((Gtk.Box.BoxChild)(this.hboxTitleBar[this.btnClose]));
				boxChild.Position = this.btnClose.Visible ? 0 : 3;
			}
		}

		private int frameBorderWidth = 0;
		private int frameBorderMargin = 0;
		private bool isMoving = false;
		private bool isSizing = false;
		private bool isInBorder = false;
		// absolute
		private Gdk.Point lastMousePos = Gdk.Point.Zero;
		private Gdk.Size lastWindowSize = Gdk.Size.Empty;
		private Gdk.Point lastWindowPos = Gdk.Point.Zero;
		private SizingSide.SizingSideType sizingSide = SizingSide.SizingSideType.E_NONE;
		private Gdk.CursorType cursorType = Gdk.CursorType.Arrow;

		public CustomWindow() : base(Gtk.WindowType.Toplevel)
		{
			CustomInit();
		}

		public CustomWindow(Gtk.WindowType type) : base(type)
		{
			CustomInit();
		}

		public CustomWindow(string title) : base(title)
		{
			CustomInit();
		}

		private void CustomInit()
		{
			CustomBuild();
			FrameBorderWidth = 3;

			if (ThemeHelper.ForceDecoration)
				Decorated = true;

			base.AddEvents((int) (
				Gdk.EventMask.PointerMotionMask | 
				Gdk.EventMask.ButtonPressMask | 
				Gdk.EventMask.ButtonReleaseMask
				));

			this.evntboxTitleBar.AddEvents((int) (
				Gdk.EventMask.PointerMotionMask | 
				Gdk.EventMask.ButtonPressMask | 
				Gdk.EventMask.ButtonReleaseMask
				));
			this.evntboxTitleBar.Shown += delegate(object sender, EventArgs e)
			{
				// hook for main window showall
				this.evntboxTitleBar.Visible = !this.Decorated;
			};
			this.evntboxTitleBar.MotionNotifyEvent += HandleMotionNotifyEventTitleBar;
			this.evntboxTitleBar.ButtonPressEvent += HandleButtonPressEventTitleBar;
			this.evntboxTitleBar.ButtonReleaseEvent += HandleButtonReleaseEventTitleBar;

			this.btnMinimize.Clicked += HandleClickedBtnMinimize;
			this.btnMaximize.Clicked += HandleClickedBtnMaximize;
			this.btnClose.Clicked += HandleClickedBtnClose;

			this.Add(this.vboxWindow);
			this.Hide();
		}

		protected override void OnAdded(Gtk.Widget widget)
		{
			if (widget.Equals(this.vboxWindow))
			{
				base.OnAdded(widget);
				return;
			}
			
			this.vboxClient.PackStart(widget, true, true, 0);
			widget.Show();
		}

		protected override bool OnExposeEvent(Gdk.EventExpose evnt)
		{
			bool bres = base.OnExposeEvent(evnt);
			
			if (!base.Decorated)
			{
				Gdk.GC gc = this.vboxWindow.Style.BackgroundGC(Gtk.StateType.Normal);
				gc.SetLineAttributes(FrameBorderWidth, Gdk.LineStyle.Solid, Gdk.CapStyle.Butt, Gdk.JoinStyle.Miter);
				base.GdkWindow.DrawRectangle(
					gc, 
					false,
					Allocation.X + FrameBorderWidth / 2, 
					Allocation.Y + FrameBorderWidth / 2,
					Allocation.Width - FrameBorderWidth, 
					Allocation.Height - FrameBorderWidth
				);
			}
			
			return bres;
		}

		protected override void OnSizeRequested(ref Gtk.Requisition requisition)
		{
			base.OnSizeRequested(ref requisition);
			base.QueueDraw();
		}

		private void SelectCursor()
		{
			Gdk.CursorType selCursorType = Gdk.CursorType.Arrow;
			if (!SizingSide.CursorTypeMaps.TryGetValue(sizingSide, out selCursorType))
				selCursorType = Gdk.CursorType.Arrow;
			
			GdkWindow.Cursor = new Gdk.Cursor(cursorType);
			if (cursorType != selCursorType)
			{
				cursorType = selCursorType;
				GdkWindow.Cursor = new Gdk.Cursor(cursorType);
				GdkWindow.Screen.RootWindow.Cursor = new Gdk.Cursor(cursorType);
			}
		}
		#region TitleButtonsEvents
		private void HandleClickedBtnClose(object sender, EventArgs e)
		{
			GdkEventAny se = new GdkEventAny();
			se.Type = Gdk.EventType.Delete;
			se.Window = this.GdkWindow;
			se.SendEvent = true;
			
			IntPtr ptr = GLib.Marshaller.StructureToPtrAlloc(se); 
			Gdk.Event evnt = new Gdk.Event(ptr);
			Gdk.EventHelper.Put(evnt);
		}

		private void HandleClickedBtnMinimize(object sender, EventArgs e)
		{
			if (base.GdkWindow.State == Gdk.WindowState.Iconified)
			{
				base.GdkWindow.Raise();
			}
			else
			{
				base.GdkWindow.Iconify();
			}
		}

		private void HandleClickedBtnMaximize(object sender, EventArgs e)
		{
			if (base.GdkWindow.State == Gdk.WindowState.Maximized)
			{
				base.GdkWindow.Unmaximize();
				ShowTitleBar = true;
			}
			else
			{
				base.GdkWindow.Maximize();
				if (PlatformUtils.IsX11 || PlatformUtils.IsMac)
					ShowTitleBar = false;
			}
		}
		#endregion TitleButtonsEvents

		#region TitleBarEvents
		private void HandleButtonReleaseEventTitleBar(object o, Gtk.ButtonReleaseEventArgs args)
		{
			isMoving = false;
		}

		private void HandleButtonPressEventTitleBar(object o, Gtk.ButtonPressEventArgs args)
		{
			// sizing
			if (base.GdkWindow.State != Gdk.WindowState.Maximized &&
				!isSizing && 
				sizingSide == SizingSide.SizingSideType.E_NONE)
			{
				
				if (args.Event.Type == Gdk.EventType.ButtonPress &&
					args.Event.Button == 1)
				{
					isMoving = true;
					lastMousePos.X = (int)args.Event.XRoot;
					lastMousePos.Y = (int)args.Event.YRoot;
					base.GdkWindow.GetPosition(out lastWindowPos.X, out lastWindowPos.Y);
				}
			}
			
			// maximize
			if (base.Resizable && args.Event.Type == Gdk.EventType.TwoButtonPress &&
				args.Event.Button == 1)
			{
				if (base.GdkWindow.State == Gdk.WindowState.Maximized)
				{
					base.GdkWindow.Unmaximize();
					ShowTitleBar = true;
				}
				else
				{
					base.GdkWindow.Maximize();
					if (PlatformUtils.IsX11 || PlatformUtils.IsMac)
						ShowTitleBar = false;
				}
				base.QueueDraw();
			}
		}

		private void HandleMotionNotifyEventTitleBar(object o, Gtk.MotionNotifyEventArgs args)
		{
			if (!isMoving)
				return;
			
			Gdk.Point curMousePos = Gdk.Point.Zero;
			curMousePos.X = (int)args.Event.XRoot;
			curMousePos.Y = (int)args.Event.YRoot;
			
			Gdk.Point curWindowPos = Gdk.Point.Zero;
			base.GdkWindow.GetPosition(out curWindowPos.X, out curWindowPos.Y);
			
			curWindowPos.X = lastWindowPos.X + curMousePos.X - lastMousePos.X;
			curWindowPos.Y = lastWindowPos.Y + curMousePos.Y - lastMousePos.Y;
			
			base.GdkWindow.Move(curWindowPos.X, curWindowPos.Y);
		}
		#endregion TitleBarEvents

		#region FrameBorderEvents
		protected override bool OnMotionNotifyEvent(Gdk.EventMotion evnt)
		{
			if (!evnt.Window.Equals(base.GdkWindow) &&
				!evnt.Window.Equals(this.evntboxTitleBar.GdkWindow))
				return base.OnMotionNotifyEvent(evnt);

			if (base.Decorated || 
				base.GdkWindow.State == Gdk.WindowState.Maximized ||
				!base.Resizable)
				return base.OnMotionNotifyEvent(evnt);

			if (!isSizing && isInBorder)
			{
				sizingSide = SizingSide.SizingSideType.E_NONE;

				if (evnt.X < Allocation.X + frameBorderWidth + frameBorderMargin)
					sizingSide |= SizingSide.SizingSideType.E_LEFT_SIDE;
				else if (evnt.X >= Allocation.X + Allocation.Width - frameBorderWidth - frameBorderMargin)
					sizingSide |= SizingSide.SizingSideType.E_RIGHT_SIDE;

				int cornerMargin = (sizingSide == SizingSide.SizingSideType.E_NONE ? 0 : frameBorderWidth * 2);

				if (evnt.Y < Allocation.Y + frameBorderWidth + frameBorderMargin + cornerMargin)
					sizingSide |= SizingSide.SizingSideType.E_TOP_SIDE;
				else if (evnt.Y >= Allocation.Y + Allocation.Height - frameBorderWidth - frameBorderMargin - cornerMargin)
					sizingSide |= SizingSide.SizingSideType.E_BOTTOM_SIDE;

				cornerMargin = (sizingSide == SizingSide.SizingSideType.E_NONE ? 0 : frameBorderWidth * 2);

				if (evnt.X < Allocation.X + frameBorderWidth + frameBorderMargin + cornerMargin)
					sizingSide |= SizingSide.SizingSideType.E_LEFT_SIDE;
				else if (evnt.X >= Allocation.X + Allocation.Width - frameBorderWidth - frameBorderMargin - cornerMargin)
					sizingSide |= SizingSide.SizingSideType.E_RIGHT_SIDE;
			}

			if (isSizing)
			{

				Gdk.Point windowPos = Gdk.Point.Zero;
				GdkWindow.GetPosition(out windowPos.X, out windowPos.Y);
				
				Gdk.Rectangle windowRect = Gdk.Rectangle.Zero;
				windowRect.X = windowPos.X;
				windowRect.Y = windowPos.Y;
				windowRect.Width = lastWindowSize.Width;
				windowRect.Height = lastWindowSize.Height;
				
				Gdk.Point curMousePos = Gdk.Point.Zero;
				curMousePos.X = (int)evnt.XRoot;
				curMousePos.Y = (int)evnt.YRoot;

				Gdk.Size minSize = Gdk.Size.Empty;
				minSize.Width = base.SizeRequest().Width;
				minSize.Height = base.SizeRequest().Height;
				
				switch (sizingSide)
				{
					case SizingSide.SizingSideType.E_LEFT_SIDE:
						windowRect.Width -= curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						windowRect.X = lastWindowPos.X + lastWindowSize.Width - windowRect.Width;
						break;
					case SizingSide.SizingSideType.E_RIGHT_SIDE:
						windowRect.Width += curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						break;
					case SizingSide.SizingSideType.E_TOP_SIDE:
						windowRect.Height -= curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						windowRect.Y = lastWindowPos.Y + lastWindowSize.Height - windowRect.Height;
						break;
					case SizingSide.SizingSideType.E_TOP_LEFT_CONER:
						windowRect.Width -= curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						windowRect.X = lastWindowPos.X + lastWindowSize.Width - windowRect.Width;

						windowRect.Height -= curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						windowRect.Y = lastWindowPos.Y + lastWindowSize.Height - windowRect.Height;
						break;
					case SizingSide.SizingSideType.E_TOP_RIGHT_CONER:
						windowRect.Width += curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);

						windowRect.Height -= curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						windowRect.Y = lastWindowPos.Y + lastWindowSize.Height - windowRect.Height;
						break;
					case SizingSide.SizingSideType.E_BOTTOM_SIDE:
						windowRect.Height += curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						break;
					case SizingSide.SizingSideType.E_BOTTOM_LEFT_CONER:
						windowRect.Width -= curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						windowRect.X = lastWindowPos.X + lastWindowSize.Width - windowRect.Width;

						windowRect.Height += curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						break;
					case SizingSide.SizingSideType.E_BOTTOM_RIGHT_CONER:
						windowRect.Width += curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);

						windowRect.Height += curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						break;
					default:
						break;
				}
				
				base.GdkWindow.MoveResize(windowRect);
			}

			SelectCursor();

			return base.OnMotionNotifyEvent(evnt);
		}

		protected override bool OnEnterNotifyEvent(Gdk.EventCrossing evnt)
		{
			isInBorder = true;
			return base.OnEnterNotifyEvent(evnt);
		}

		protected override bool OnLeaveNotifyEvent(Gdk.EventCrossing evnt)
		{
			isInBorder = false;
			if (!isSizing)
			{
				sizingSide = SizingSide.SizingSideType.E_NONE;
				SelectCursor();
			}
			return base.OnLeaveNotifyEvent(evnt);
		}

		protected override bool OnButtonPressEvent(Gdk.EventButton evnt)
		{
			if (evnt.Window.Equals(base.GdkWindow))
			{

				if (!base.Decorated &&
					GdkWindow.State != Gdk.WindowState.Maximized)
				{
					
					if (evnt.Type == Gdk.EventType.ButtonPress &&
						evnt.Button == 1)
					{
						
						isSizing = true;
						lastMousePos.X = (int)evnt.XRoot;
						lastMousePos.Y = (int)evnt.YRoot;
						GetPosition(out lastWindowPos.X, out lastWindowPos.Y);
						lastWindowSize.Width = Allocation.Width;
						lastWindowSize.Height = Allocation.Height;
					}
				}
			}
			
			return base.OnButtonPressEvent(evnt);
		}

		protected override bool OnButtonReleaseEvent(Gdk.EventButton evnt)
		{
			if (evnt.Window.Equals(base.GdkWindow) ||
				evnt.Window.Equals(this.evntboxTitleBar.GdkWindow))
				isSizing = false;
			
			return base.OnButtonReleaseEvent(evnt);
		}
		#endregion FrameBorderEvents
	}
}

