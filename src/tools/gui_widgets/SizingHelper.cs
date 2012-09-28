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
	public class SizingHelper
	{
		public enum E_SIZING_SIDE
		{
			E_NONE = 0,
			E_LEFT_SIDE = 1,
			E_RIGHT_SIDE = 2,
			E_TOP_SIDE = 4,
			E_TOP_LEFT_CONER = 5,
			E_TOP_RIGHT_CONER = 6,
			E_BOTTOM_SIDE = 8,
			E_BOTTOM_LEFT_CONER = 9,
			E_BOTTOM_RIGHT_CONER = 10
		}

		private uint _marginSize = 4;
		private Gtk.Widget _window;
		public Gtk.Widget Window {
			get {
				return _window;
			}
		}
		private bool _allow_sizing = true;
		public bool AllowSizing {
			get {
				return _allow_sizing;
			}
		}

		private Gtk.Widget _border;
		public Gtk.Widget Border {
			get {
				return _border;
			}
		}

		private bool _isSizing = false;
		public bool IsSizing {
			get {
				return _isSizing;
			}
		}

		private E_SIZING_SIDE _isSizingSide = 0;
		// absolute mouse pos
		private Gdk.Point _lastMousePos = Gdk.Point.Zero;
		// absolute window size
		private Gdk.Size _windowSize = Gdk.Size.Empty;





		public SizingHelper(Gtk.Widget window, Gtk.Widget border, bool allow_sizing, uint marginSize)
		{
			this._window = window;
			this._border = border;
			this._marginSize = marginSize;
			this._allow_sizing = allow_sizing;

			// check size request
			{
				int vw, vh;
				this._window.GetSizeRequest(out vw, out vh);
				vw = vw <= 0 ? 100 : vw;
				vh = vh <= 0 ? 100 : vh;
				this._window.SetSizeRequest(100, 100);
			}

			if (_allow_sizing) {
				this._border.AddEvents((int)Gdk.EventMask.PointerMotionMask);
				
				this._border.EnterNotifyEvent += HandleEnterNotifyEvent;
				this._border.LeaveNotifyEvent += HandleLeaveNotifyEvent;
				this._border.ButtonPressEvent += HandleButtonPressEvent;
				this._border.ButtonReleaseEvent += HandleButtonReleaseEvent;
				this._border.MotionNotifyEvent += HandleMotionNotifyEvent;
			}

			// Set style for border from resource style
			Gdk.Color bgc = this._window.Style.Background(Gtk.StateType.Selected);
			//Gdk.Color.Parse("black", ref bgc);
			this._border.ModifyBg(Gtk.StateType.Normal, bgc);
		}

		public void SelectSizingSide (Gdk.Point localMousePos)
		{
			Gdk.Size windowSize = Gdk.Size.Empty;
			{
				int vw, vh;
				this._window.GdkWindow.GetSize (out vw, out vh);
				windowSize.Width = vw;
				windowSize.Height = vh;
			}

			_isSizingSide = E_SIZING_SIDE.E_NONE;
			if (localMousePos.X < _marginSize)
				_isSizingSide |= E_SIZING_SIDE.E_LEFT_SIDE;
			else if (localMousePos.X >= windowSize.Width - _marginSize)
				_isSizingSide |= E_SIZING_SIDE.E_RIGHT_SIDE;
			
			if (localMousePos.Y < _marginSize)
				_isSizingSide |= E_SIZING_SIDE.E_TOP_SIDE;
			else if (localMousePos.Y >= windowSize.Height - _marginSize)
				_isSizingSide |= E_SIZING_SIDE.E_BOTTOM_SIDE;
		}



		public void SelectCursor ()
		{
			Gdk.CursorType cursorType = Gdk.CursorType.Arrow;
			switch (_isSizingSide) {
			case E_SIZING_SIDE.E_LEFT_SIDE:
				cursorType = Gdk.CursorType.LeftSide;
				break;
			case E_SIZING_SIDE.E_RIGHT_SIDE:
				cursorType = Gdk.CursorType.RightSide;
				break;
			case E_SIZING_SIDE.E_TOP_SIDE:
				cursorType = Gdk.CursorType.TopSide;
				break;
			case E_SIZING_SIDE.E_TOP_LEFT_CONER:
				cursorType = Gdk.CursorType.TopLeftCorner;
				break;
			case E_SIZING_SIDE.E_TOP_RIGHT_CONER:
				cursorType = Gdk.CursorType.TopRightCorner;
				break;
			case E_SIZING_SIDE.E_BOTTOM_SIDE:
				cursorType = Gdk.CursorType.BottomSide;
				break;
			case E_SIZING_SIDE.E_BOTTOM_LEFT_CONER:
				cursorType = Gdk.CursorType.BottomLeftCorner;
				break;
			case E_SIZING_SIDE.E_BOTTOM_RIGHT_CONER:
				cursorType = Gdk.CursorType.BottomRightCorner;
				break;
			default:
				cursorType = Gdk.CursorType.Arrow;
				break;
			}
			
			this._window.GdkWindow.Cursor = new Gdk.Cursor(cursorType);
		}
		
		public void ResetCursor()
		{
			this._window.GdkWindow.Cursor = new Gdk.Cursor(Gdk.CursorType.Arrow);
		}

		public void StartResize ()
		{
			_isSizing = true;
			{
				int vx;
				int vy;
				Gdk.ModifierType modifierType;
				this._window.Screen.RootWindow.GetPointer(out vx, out vy, out modifierType);
				_lastMousePos.X = vx;
				_lastMousePos.Y = vy;
			}
			{
				int vw, vh;
				this._window.GdkWindow.GetSize (out vw, out vh);
				_windowSize.Width = vw;
				_windowSize.Height = vh;
			}
		}

		public void Resize ()
		{
			// absolute window rect
			Gdk.Rectangle windowRect = Gdk.Rectangle.Zero;
			Gdk.Point windowPos = Gdk.Point.Zero;
			{
				windowRect.Width = _windowSize.Width;
				windowRect.Height = _windowSize.Height;
				int vx, vy;
				this._window.GdkWindow.GetPosition (out vx, out vy);
				windowRect.X = vx;
				windowRect.Y = vy;
				windowPos.X = vx;
				windowPos.Y = vy;
			}
			
			// absolute mouse pos
			Gdk.Point curMousePos = Gdk.Point.Zero;
			{
				int vx;
				int vy;
				Gdk.ModifierType modifierType;
				this._window.Screen.RootWindow.GetPointer (out vx, out vy, out modifierType);
				curMousePos.X = vx;
				curMousePos.Y = vy;
			}
			
			switch (_isSizingSide) {
			case E_SIZING_SIDE.E_LEFT_SIDE:
				windowRect.X = curMousePos.X;
				windowRect.Width -= curMousePos.X - _lastMousePos.X;
				break;
			case E_SIZING_SIDE.E_RIGHT_SIDE:
				windowRect.Width += curMousePos.X - _lastMousePos.X;
				break;
			case E_SIZING_SIDE.E_TOP_SIDE:
				windowRect.Y = curMousePos.Y;
				windowRect.Height -= curMousePos.Y - _lastMousePos.Y;
				break;
			case E_SIZING_SIDE.E_TOP_LEFT_CONER:
				windowRect.X = curMousePos.X;
				windowRect.Y = curMousePos.Y;
				windowRect.Width -= curMousePos.X - _lastMousePos.X;
				windowRect.Height -= curMousePos.Y - _lastMousePos.Y;
				break;
			case E_SIZING_SIDE.E_TOP_RIGHT_CONER:
				windowRect.Y = curMousePos.Y;
				windowRect.Width += curMousePos.X - _lastMousePos.X;
				windowRect.Height -= curMousePos.Y - _lastMousePos.Y;
				break;
			case E_SIZING_SIDE.E_BOTTOM_SIDE:
				windowRect.Height += curMousePos.Y - _lastMousePos.Y;
				break;
			case E_SIZING_SIDE.E_BOTTOM_LEFT_CONER:
				windowRect.X = curMousePos.X;
				windowRect.Width -= curMousePos.X - _lastMousePos.X;
				windowRect.Height += curMousePos.Y - _lastMousePos.Y;
				break;
			case E_SIZING_SIDE.E_BOTTOM_RIGHT_CONER:
				windowRect.Width += curMousePos.X - _lastMousePos.X;
				windowRect.Height += curMousePos.Y - _lastMousePos.Y;
				break;
			default:
				break;
			}

			Gdk.Size minSize = Gdk.Size.Empty;
			{
				int vw, vh;
				this._window.GetSizeRequest (out vw, out vh);
				minSize.Width = vw;
				minSize.Height = vh;
			}

			if (minSize.Width >= windowRect.Width)
			{
				windowRect.X = windowPos.X;
				windowRect.Width = minSize.Width;
			}
			if (minSize.Height >= windowRect.Height)
			{
				windowRect.Y = windowPos.Y;
				windowRect.Height = minSize.Height;
			}
			
			this._window.GdkWindow.FreezeUpdates();
			this._window.GdkWindow.MoveResize(windowRect);
			this._window.GdkWindow.ThawUpdates();
		}

		public void StopResize()
		{
			_isSizing = false;
			_isSizingSide = E_SIZING_SIDE.E_NONE;
		}





		private void HandleEnterNotifyEvent(object o, Gtk.EnterNotifyEventArgs args)
		{
			// not sizing
			if (this._window.GdkWindow.State != Gdk.WindowState.Maximized) {
				if (!IsSizing) {
					Gdk.Point localMousePos = Gdk.Point.Zero;
					//new Gdk.Point((int)args.Event.X, (int)args.Event.Y);
					{
						int vx, vy;
						this._border.GetPointer(out vx, out vy);
						localMousePos.X = vx;
						localMousePos.Y = vy;
					}
					SelectSizingSide(localMousePos);
					SelectCursor();
				}
			}
		}
		
		private void HandleLeaveNotifyEvent (object o, Gtk.LeaveNotifyEventArgs args)
		{
			// not sizing
			if (!IsSizing)
				ResetCursor();
		}
		
		private void HandleButtonPressEvent(object o, Gtk.ButtonPressEventArgs args)
		{
			// start resizing
			if (this._window.GdkWindow.State != Gdk.WindowState.Maximized) {
				if (args.Event.Type == Gdk.EventType.ButtonPress &&
				    args.Event.Button == 1) {
					StartResize();
				}
			}
		}
		
		private void HandleButtonReleaseEvent (object o, Gtk.ButtonReleaseEventArgs args)
		{
			// stop resizing
			StopResize();
		}
		
		
		private void HandleMotionNotifyEvent(object o, Gtk.MotionNotifyEventArgs args)
		{
			// not sizing
			if (this._window.GdkWindow.State != Gdk.WindowState.Maximized) {
				if (!IsSizing) {
					// local mouse pos
					Gdk.Point localMousePos = new Gdk.Point((int)args.Event.X, (int)args.Event.Y);
					SelectSizingSide(localMousePos);
				}
				SelectCursor();
				
				// resizing
				if (IsSizing) {
					Resize();
				}
			}
		}
	}
}

