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
	public class MovingHelper
	{

		private Gtk.Widget _window;
		public Gtk.Widget Window {
			get {
				return _window;
			}
		}

		private bool _isMoving = false;
		public bool IsMoving
		{
			get
			{
				return this._isMoving;
			}
		}
		// absolute mouse pos
		private Gdk.Point _lastMousePos;

		public MovingHelper (Gtk.Widget window)
		{
			this._window = window;
		}


		public void StartMove()
		{
			_isMoving = true;
			int vx, vy;
			Gdk.ModifierType modifierType;
			this._window.Screen.RootWindow.GetPointer(out vx, out vy, out modifierType);
			_lastMousePos.X = vx;
			_lastMousePos.Y = vy;
		}

		public void Move ()
		{
			Gdk.Point curMousePos = Gdk.Point.Zero;
			{
				int vx, vy;
				Gdk.ModifierType modifierType;
				this._window.GdkWindow.Screen.RootWindow.GetPointer (out vx, out vy, out modifierType);
				curMousePos.X = vx;
				curMousePos.Y = vy;
			}
			Gdk.Point windowPos = Gdk.Point.Zero;
			{
				int vx, vy;
				this._window.GdkWindow.GetPosition (out vx, out vy);
				windowPos.X = vx;
				windowPos.Y = vy;
			}
			windowPos.X += curMousePos.X - _lastMousePos.X;
			windowPos.Y += curMousePos.Y - _lastMousePos.Y;
			_lastMousePos.X = curMousePos.X;
			_lastMousePos.Y = curMousePos.Y;
			this._window.GdkWindow.Move (windowPos.X, windowPos.Y);
		}

		public void StopMove()
		{
			_isMoving = false;
		}


	}
}

