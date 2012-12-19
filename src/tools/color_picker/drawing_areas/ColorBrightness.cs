// \author		Макс
// \date		18.12.2012 (c)Макс
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gdk;

namespace ColorPicker
{
	[System.ComponentModel.ToolboxItem(true)]
	public class ColorBrightness : Gtk.DrawingArea
	{
		private static ColorBrightness inst;
		private static readonly int WIDTH = 14, HEIGHT = 128;
		private Pixbuf copy;
		private bool firstDraw = true;
		private int spectrX, arcY;
		private Window window;
		private Gdk.GC gc;
		
		public ColorBrightness ()
		{
			copy = new Pixbuf (Colorspace.Rgb, false, 8, 1, HEIGHT);
			this.Events = Gdk.EventMask.ButtonPressMask;
			inst = this;
		}
		
		public void ClickProcessing (int x, int y)
		{
			this.spectrX = x;
			clickProcessing(y);
		}
		
		public void ClickProcessing (int y)
		{
			clickProcessing(y);
		}
		
		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
		{
			int y = (int)ev.Y;
			clickProcessing(y);
			ClickEventHandler.Inst.BrightnessClicked(y);
			return base.OnButtonPressEvent (ev);
		}
		
		protected override bool OnExposeEvent (Gdk.EventExpose ev)
		{
			base.OnExposeEvent (ev);
			
			if (firstDraw) {
				window = this.GdkWindow;
				gc = new Gdk.GC (window);
				
				firstDraw = false;
			} else {
				redrawPanel();
				drawClickArc();
			}
			
			return true;
		}
		
		protected override void OnSizeAllocated (Gdk.Rectangle allocation)
		{
			base.OnSizeAllocated (allocation);
		}
		
		protected override void OnSizeRequested (ref Gtk.Requisition requisition)
		{
			requisition.Height = HEIGHT;
			requisition.Width = WIDTH;
		}
		
		private void clickProcessing (int y)
		{
			this.arcY = y;
			redrawPanel();
			drawClickArc();
		}
		
		private void redrawPanel ()
		{
			SpectrumVerticalPixbuf.Pixbuf.CopyArea (spectrX, 0, 1, HEIGHT, copy, 0, 0);
			window.DrawPixbuf(gc, copy.ScaleSimple(WIDTH, HEIGHT, InterpType.Bilinear),
			                  0, 0, 0, 0, -1, -1, 0, 0, 0);
		}
		
		private void drawClickArc()
		{
			gc.RgbFgColor = new Gdk.Color(255, 255, 255);
			window.DrawArc(gc, false, WIDTH / 2 - 3, arcY - 3, 6, 6, 0, 360 * 64);
			gc.RgbFgColor = new Gdk.Color(0, 0, 0);
			window.DrawArc(gc, false, WIDTH / 2 - 4, arcY - 4, 8, 8, 0, 360 * 64);
		}
		
		public int SpectrX {
			get {
				return spectrX;
			}
		}
		
		public int Y {
			get {
				return arcY;
			}
		}

		public static ColorBrightness Inst {
			get {
				return inst;
			}
		}
	}
}

