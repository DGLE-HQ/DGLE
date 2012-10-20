/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;

namespace Gui
{
	public partial class CustomWindow : Gtk.Window
	{
		[GLib.Property ("MinimizeImage")]
		public Gtk.Widget MinimizeImage
		{
			get
			{
				return this.btnMinimize.Image;
			}
			set
			{
				this.btnMinimize.Image = value;
			}
		}
		
		[GLib.Property ("MaximizeImage")]
		public Gtk.Widget MaximizeImage
		{
			get
			{
				return this.btnMaximize.Image;
			}
			set
			{
				this.btnMaximize.Image = value;
			}
		}
		
		[GLib.Property ("CloseImage")]
		public Gtk.Widget CloseImage
		{
			get
			{
				return this.btnClose.Image;
			}
			set
			{
				this.btnClose.Image = value;
			}
		}

		[GLib.Property ("FrameBorderWidth")]
		public int FrameBorderWidth {
			get {
				return frameBorderWidth;
			}
			set {
				frameBorderWidth = value;
				base.BorderWidth = (uint) frameBorderWidth;
			}
		}

		[GLib.Property ("decorated")]
		new public bool Decorated
		{
			get
			{
				return base.Decorated;
			}
			set
			{
				if (base.Decorated == value)
					return;

				base.Decorated = value;
				ShowTitleBar = !base.Decorated;
			}
		}

		[GLib.Property ("title")]
		new public string Title
		{
			get
			{
				return base.Title;
			}
			set
			{
				base.Title = value;
				this.labelCaption.Text = value;
			}
		}


		[GLib.Property ("resizable")]
		new public bool Resizable
		{
			get
			{
				return base.Resizable;
			}
			set
			{
				base.Resizable = value;
				ShowMaximizeBtn = base.Resizable;
				ShowMinimizeBtn = base.Resizable;
			}
		}

		private bool ShowTitleBar {
			set {
				this.evntboxTitleBar.Visible = value;
				this.evntboxTitleBar.ChildVisible = value;
				this.evntboxTitleBar.Sensitive = value;
				
				Gtk.Box.BoxChild boxChild = ((Gtk.Box.BoxChild)(this.vboxWindow [this.evntboxTitleBar]));
				boxChild.Fill = value;
				
				base.QueueDraw();
			}
		}

		private bool ShowMinimizeBtn {
			set {
				this.btnMinimize.Visible = value;
				this.btnMinimize.ChildVisible = value;
				this.btnMinimize.Sensitive = value;
			}
		}

		private bool ShowMaximizeBtn {
			set {
				this.btnMaximize.Visible = value;
				this.btnMaximize.ChildVisible = value;
				this.btnMaximize.Sensitive = value;
			}
		}

		private int frameBorderWidth = 4;
		private bool isMoving = false;
		private bool isSizing = false;
		// absolute
		private Gdk.Point lastMousePos = Gdk.Point.Zero;
		private Gdk.Size lastWindowSize = Gdk.Size.Empty;
		private SizingSide.SizingSideType sizingSide = SizingSide.SizingSideType.E_NONE;

		private Gdk.Color bgColor = Gdk.Color.Zero;
		private Gdk.Color fgColor = Gdk.Color.Zero;


		public CustomWindow () : base(Gtk.WindowType.Toplevel)
		{
			CustomInit();
		}
		
		public CustomWindow (Gtk.WindowType type) : base(type)
		{
			CustomInit();
		}
		
		public CustomWindow (string title) : base(title)
		{
			CustomInit();
		}

		private void CustomInit()
		{
			CustomBuild();

			this.StyleSet += HandleStyleSet;

			base.AddEvents((int) (
				Gdk.EventMask.PointerMotionMask | 
				Gdk.EventMask.ButtonPressMask | 
				Gdk.EventMask.ButtonReleaseMask
				));
			
			this.evntboxTitleBar.AddEvents ((int) (
				Gdk.EventMask.PointerMotionMask | 
				Gdk.EventMask.ButtonPressMask | 
				Gdk.EventMask.ButtonReleaseMask
				));
			this.evntboxTitleBar.Shown += delegate(object sender, EventArgs e) {
				// hook for main window showall
				this.evntboxTitleBar.Visible = !this.Decorated;
			};
			this.evntboxTitleBar.MotionNotifyEvent += HandleMotionNotifyEventTitleBar;
			this.evntboxTitleBar.ButtonPressEvent += HandleButtonPressEventTitleBar;
			this.evntboxTitleBar.ButtonReleaseEvent += HandleButtonReleaseEventTitleBar;
			
			//this.vboxClient.BorderWidth = (uint) frameBorderWidth;
			this.hboxTitleBar.BorderWidth = (uint) frameBorderWidth;
			this.labelCaption.Xpad = frameBorderWidth;
			
			this.btnMinimize.Clicked += HandleClickedBtnMinimize;
			this.btnMaximize.Clicked += HandleClickedBtnMaximize;
			this.btnClose.Clicked += HandleClickedBtnClose;
			
			this.Add (this.vboxWindow);
			this.Hide ();

		}

		protected override void OnAdded (Gtk.Widget widget)
		{
			if (widget.Equals(this.vboxWindow))
				base.OnAdded (widget);
			
			this.vboxClient.PackStart (widget, true, true, 0);
			widget.Show();
		}

		private void HandleStyleSet (object o, Gtk.StyleSetArgs args)
		{
			bgColor = this.Style.Background(Gtk.StateType.Selected);
			fgColor = this.Style.Foreground(Gtk.StateType.Selected);
			bgColor = new Gdk.Color (0, 0, 0);
			fgColor = new Gdk.Color (255, 255, 255);

			this.evntboxTitleBar.ModifyBg(Gtk.StateType.Normal, bgColor);
			this.labelCaption.ModifyFg(Gtk.StateType.Normal, fgColor);

			this.btnMinimize.ModifyBg(Gtk.StateType.Normal, bgColor);
			this.btnMaximize.ModifyBg(Gtk.StateType.Normal, bgColor);
			this.btnClose.ModifyBg(Gtk.StateType.Normal, bgColor);
		}

		protected override bool OnExposeEvent (Gdk.EventExpose evnt)
		{
			bool bres = base.OnExposeEvent (evnt);
			
			if (!base.Decorated) {
				using (Cairo.Context cr = Gdk.CairoHelper.Create (this.GdkWindow)) {
					cr.LineWidth = frameBorderWidth;
					cr.Color = CairoExtensions.GdkColorToCairoColor (bgColor);
					cr.Rectangle (Allocation.X, Allocation.Y, Allocation.Width, Allocation.Height);
					cr.Stroke ();
				}
			}
			
			return bres;
		}

		protected override void OnSizeRequested (ref Gtk.Requisition requisition)
		{
			base.OnSizeRequested (ref requisition);
			base.QueueDraw ();
		}

		private void SelectCursor ()
		{
			Gdk.CursorType cursorType = Gdk.CursorType.Arrow;
			if (!SizingSide.CursorTypeMaps.TryGetValue(sizingSide, out cursorType))
				cursorType = Gdk.CursorType.Arrow;
			
			GdkWindow.Cursor = new Gdk.Cursor(cursorType);
		}


		#region TitleButtonsEvents
		private void HandleClickedBtnClose (object sender, EventArgs e)
		{
			GdkEventAny se = new GdkEventAny ();
			se.Type = Gdk.EventType.Delete;
			se.Window = this.GdkWindow;
			se.SendEvent = true;
			
			IntPtr ptr = GLib.Marshaller.StructureToPtrAlloc (se); 
			Gdk.Event evnt = new Gdk.Event (ptr);
			Gdk.EventHelper.Put (evnt);
		}
		
		private void HandleClickedBtnMinimize (object sender, EventArgs e)
		{
			if (base.GdkWindow.State == Gdk.WindowState.Iconified) {
				base.GdkWindow.Raise();
			} else {
				base.GdkWindow.Iconify();
			}
		}
		
		private void HandleClickedBtnMaximize (object sender, EventArgs e)
		{
			if (base.GdkWindow.State == Gdk.WindowState.Maximized) {
				base.GdkWindow.Unmaximize();
				ShowTitleBar = true;
			} else {
				base.GdkWindow.Maximize();
				if (PlatformUtils.IsX11 || PlatformUtils.IsMac)
					ShowTitleBar = false;
			}
		}
		#endregion TitleButtonsEvents

		#region TitleBarEvents
		private void HandleButtonReleaseEventTitleBar (object o, Gtk.ButtonReleaseEventArgs args)
		{
			isMoving = false;
		}
		
		private void HandleButtonPressEventTitleBar(object o, Gtk.ButtonPressEventArgs args)
		{
			// sizing
			if (base.GdkWindow.State != Gdk.WindowState.Maximized &&
				!isSizing && 
				sizingSide == SizingSide.SizingSideType.E_NONE) {
				
				if (args.Event.Type == Gdk.EventType.ButtonPress &&
					args.Event.Button == 1) {
					isMoving = true;
					lastMousePos.X = (int)args.Event.XRoot;
					lastMousePos.Y = (int)args.Event.YRoot;
				}
			}
			
			// maximize
			if (args.Event.Type == Gdk.EventType.TwoButtonPress &&
				args.Event.Button == 1) {
				if (base.GdkWindow.State == Gdk.WindowState.Maximized) {
					base.GdkWindow.Unmaximize();
					ShowTitleBar = true;
				} else {
					base.GdkWindow.Maximize();
					if (PlatformUtils.IsX11 || PlatformUtils.IsMac)
						ShowTitleBar = false;
				}
				base.QueueDraw ();
			}
		}
		
		private void HandleMotionNotifyEventTitleBar (object o, Gtk.MotionNotifyEventArgs args)
		{
			if (!isMoving)
				return;
			
			Gdk.Point curMousePos = Gdk.Point.Zero;
			curMousePos.X = (int) args.Event.XRoot;
			curMousePos.Y = (int) args.Event.YRoot;
			
			Gdk.Point windowPos = Gdk.Point.Zero;
			GdkWindow.GetPosition (out windowPos.X, out windowPos.Y);
			
			windowPos.X += curMousePos.X - lastMousePos.X;
			windowPos.Y += curMousePos.Y - lastMousePos.Y;
			lastMousePos.X = curMousePos.X;
			lastMousePos.Y = curMousePos.Y;
			
			base.GdkWindow.Move (windowPos.X, windowPos.Y);
		}
		#endregion TitleBarEvents

		#region FrameBorderEvents
		protected override bool OnMotionNotifyEvent (Gdk.EventMotion evnt)
		{
			if (!evnt.Window.Equals(base.GdkWindow) &&
			    !evnt.Window.Equals(this.evntboxTitleBar.GdkWindow))
				return base.OnMotionNotifyEvent (evnt);

			if (base.Decorated || 
			    base.GdkWindow.State == Gdk.WindowState.Maximized ||
			    !base.Resizable)
				return base.OnMotionNotifyEvent (evnt);


			if (!isSizing) {

				sizingSide = SizingSide.SizingSideType.E_NONE;
				if (evnt.X < Allocation.X + frameBorderWidth)
					sizingSide |= SizingSide.SizingSideType.E_LEFT_SIDE;
				else if (evnt.X >= Allocation.X + Allocation.Width - frameBorderWidth)
					sizingSide |= SizingSide.SizingSideType.E_RIGHT_SIDE;
				
				if (evnt.Y < Allocation.Y + frameBorderWidth)
					sizingSide |= SizingSide.SizingSideType.E_TOP_SIDE;
				else if (evnt.Y >= Allocation.Y + Allocation.Height - frameBorderWidth)
					sizingSide |= SizingSide.SizingSideType.E_BOTTOM_SIDE;

			} else {

				Gdk.Point windowPos = Gdk.Point.Zero;
				GdkWindow.GetPosition (out windowPos.X, out windowPos.Y);
				
				Gdk.Rectangle windowRect = Gdk.Rectangle.Zero;
				windowRect.X = windowPos.X;
				windowRect.Y = windowPos.Y;
				windowRect.Width = lastWindowSize.Width;
				windowRect.Height = lastWindowSize.Height;
				
				Gdk.Point curMousePos = Gdk.Point.Zero;
				curMousePos.X = (int) evnt.XRoot;
				curMousePos.Y = (int) evnt.YRoot;

				Gdk.Size minSize = Gdk.Size.Empty;
				minSize.Width = base.SizeRequest().Width;
				minSize.Height = base.SizeRequest().Height;
				
				switch (sizingSide) {
					case SizingSide.SizingSideType.E_LEFT_SIDE:
						windowRect.Width -= curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						windowRect.X = lastMousePos.X + lastWindowSize.Width - windowRect.Width;
						break;
					case SizingSide.SizingSideType.E_RIGHT_SIDE:
						windowRect.Width += curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						break;
					case SizingSide.SizingSideType.E_TOP_SIDE:
						windowRect.Height -= curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						windowRect.Y = lastMousePos.Y + lastWindowSize.Height - windowRect.Height;
						break;
					case SizingSide.SizingSideType.E_TOP_LEFT_CONER:
						windowRect.Width -= curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						windowRect.X = lastMousePos.X + lastWindowSize.Width - windowRect.Width;

						windowRect.Height -= curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						windowRect.Y = lastMousePos.Y + lastWindowSize.Height - windowRect.Height;
						break;
					case SizingSide.SizingSideType.E_TOP_RIGHT_CONER:
						windowRect.Width += curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);

						windowRect.Height -= curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						windowRect.Y = lastMousePos.Y + lastWindowSize.Height - windowRect.Height;
						break;
					case SizingSide.SizingSideType.E_BOTTOM_SIDE:
						windowRect.Height += curMousePos.Y - lastMousePos.Y;
						windowRect.Height = Math.Max(windowRect.Height, minSize.Height);
						break;
					case SizingSide.SizingSideType.E_BOTTOM_LEFT_CONER:
						windowRect.Width -= curMousePos.X - lastMousePos.X;
						windowRect.Width = Math.Max(windowRect.Width, minSize.Width);
						windowRect.X = lastMousePos.X + lastWindowSize.Width - windowRect.Width;

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
				
				base.GdkWindow.FreezeUpdates();
				base.GdkWindow.MoveResize(windowRect);
				base.GdkWindow.ThawUpdates();
			}

			SelectCursor ();
			
			
			return base.OnMotionNotifyEvent (evnt);
		}
		
		protected override bool OnButtonPressEvent(Gdk.EventButton evnt)
		{
			if (evnt.Window.Equals(base.GdkWindow) || 
			    evnt.Window.Equals(this.evntboxTitleBar.GdkWindow)) {

				if (!base.Decorated &&
					GdkWindow.State != Gdk.WindowState.Maximized) {
					
					if (evnt.Type == Gdk.EventType.ButtonPress &&
						evnt.Button == 1) {
						
						isSizing = true;
						lastMousePos.X = (int)evnt.XRoot;
						lastMousePos.Y = (int)evnt.YRoot;
						lastWindowSize.Width = Allocation.Width;
						lastWindowSize.Height = Allocation.Height;
					}
				}
			}
			
			return base.OnButtonPressEvent (evnt);
		}
		
		protected override bool OnButtonReleaseEvent (Gdk.EventButton evnt)
		{
			if (evnt.Window.Equals(base.GdkWindow) ||
			    evnt.Window.Equals(this.evntboxTitleBar.GdkWindow))
				isSizing = false;
			
			return base.OnButtonReleaseEvent (evnt);
		}
		#endregion FrameBorderEvents
	}
}

