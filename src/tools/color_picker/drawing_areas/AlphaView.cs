// \author		Макс
// \date		19.12.2012 (c)Макс
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gdk;

namespace ColorPicker
{
	[System.ComponentModel.ToolboxItem(true)]
	public class AlphaView : Gtk.DrawingArea
	{
		private static AlphaView inst;
		private static readonly int WIDTH = 60, HEIGHT = 60;
		private uint color;
		private Window window;
		private Gdk.GC gc;
		private Pixbuf pixbuf;
		private bool firstDraw = true;
		private ushort red, green, blue, alpha;

		public AlphaView()
		{
			color = 0xffffffff;
			red = 0xff;
			green = 0xff;
			blue = 0xff;
			alpha = 0xff;
			inst = this;
		}

		public void ClickProcessing(ushort red, ushort green, ushort blue)
		{
			this.red = red;
			this.green = green;
			this.blue = blue;
			redrawView();
		}

		public void ClickProcessing(ushort alpha)
		{
			this.alpha = alpha;
			redrawView();
		}

		protected override bool OnButtonPressEvent(Gdk.EventButton ev)
		{
			return base.OnButtonPressEvent(ev);
		}

		protected override bool OnExposeEvent(Gdk.EventExpose ev)
		{
			base.OnExposeEvent(ev);

			if (firstDraw) {
				window = this.GdkWindow;
				gc = new Gdk.GC(window);
				pixbuf = new Pixbuf(Gdk.Colorspace.Rgb, true, 8, WIDTH, HEIGHT);
				pixbuf.Fill(0xffffffff);
				drawView();

				firstDraw = false;
			}

			drawView();

			return true;
		}

		protected override void OnSizeAllocated(Gdk.Rectangle allocation)
		{
			base.OnSizeAllocated(allocation);
		}

		protected override void OnSizeRequested(ref Gtk.Requisition requisition)
		{
			requisition.Height = HEIGHT;
			requisition.Width = WIDTH;
		}

		private void redrawView()
		{
			uint color = (uint)(((red & 0xff) << 24) + 
			                    ((green & 0xff) << 16) + 
			                    ((blue & 0xff) << 8) + 
			                    (byte)(alpha & 0xff));

			if (color != this.color) {
				this.color = color;
				pixbuf.Fill(this.color);
			}
		}

		private void drawView()
		{
			window.DrawPixbuf(gc, pixbuf, 0, 0, 0, 0, WIDTH, HEIGHT, 0, 0, 0);
		}

		public static AlphaView Inst {
			get {
				return inst;
			}
		}
	}
}

