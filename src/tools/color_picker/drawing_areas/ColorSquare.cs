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
	public class ColorSquare : Gtk.DrawingArea
	{
		private static ColorSquare inst;
		private static readonly int WIDTH = 128, HEIGHT = 128;
		private Window window;
		private Gdk.GC gc;
		private bool firstDraw = true, buttonPressed = false;
		private int arcX, arcY;
		private Pixbuf originColumn, copy, area;
		private Image panelImage;
		
		public ColorSquare ()
		{
			originColumn = new Pixbuf (Gdk.Colorspace.Rgb, false, 8, 1, HEIGHT);
			copy = new Pixbuf (Gdk.Colorspace.Rgb, false, 8, 1, HEIGHT);
			area = new Pixbuf (Gdk.Colorspace.Rgb, false, 8, WIDTH, HEIGHT);
			panelImage = new Image (ImageType.Normal, Visual, WIDTH, HEIGHT);

			this.Events = Gdk.EventMask.ButtonPressMask | 
				Gdk.EventMask.PointerMotionMask | 
					Gdk.EventMask.ButtonReleaseMask;

			inst = this;
		}
		
		public void ClickProcessing (int arcX, int arcY)
		{
			clickProcessing(arcX, arcY);
		}
		
		public void ClickProcessing (int arcX, int arcY, int spectrX)
		{
			clickProcessing(arcX, arcY, spectrX);
		}

		protected override bool OnButtonPressEvent (EventButton evnt)
		{
			buttonPressed = true;
			
			motionClickProcessing((int)evnt.X, (int)evnt.Y);
			
			return base.OnButtonPressEvent (evnt);
		}
		
		protected override bool OnButtonReleaseEvent(EventButton evnt)
		{
			buttonPressed = false;
			
			motionClickProcessing((int)evnt.X, (int)evnt.Y);
			
			return base.OnButtonReleaseEvent(evnt);
		}
		
		protected override bool OnMotionNotifyEvent(EventMotion evnt)
		{
			if (buttonPressed) {
				motionClickProcessing((int)evnt.X, (int)evnt.Y);
			}
			
			return base.OnMotionNotifyEvent(evnt);
		}
		
		protected override bool OnExposeEvent (Gdk.EventExpose ev)
		{
			base.OnExposeEvent (ev);
			
			if (firstDraw) {
				window = this.GdkWindow;
				gc = new Gdk.GC (window);
				clickProcessing(0, HEIGHT - 1, 0);
				
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
			requisition.Height = 50;
			requisition.Width = 50;
		}

		private void motionClickProcessing(int x, int y)
		{
			if ((x >= 0) && (x < WIDTH) && (y >= 0) && (y < HEIGHT)) {
				clickProcessing(x, y);
				ClickEventHandler.Inst.SquareClicked();
			}
		}
		
		private void clickProcessing (int arcX, int arcY)
		{
			//for satur., bright.
			this.arcX = arcX;
			this.arcY = arcY;
			redrawPanel();
			drawClickArc();
		}

		private void clickProcessing (int arcX, int arcY, int spectrX)
		{
			this.arcX = arcX;
			this.arcY = arcY;
			redrawPanel(spectrX);
			drawClickArc();
		}
		
		private void redrawPanel(int spectrX)
		{
			SpectrumVerticalPixbuf.Pixbuf.CopyArea (spectrX, 0, 1, HEIGHT, originColumn, 0, 0);
			
			float allSatr = 1.0f, saturV = allSatr / WIDTH;
			
			for (int i = 0; i < WIDTH; i++) {
				originColumn.SaturateAndPixelate(copy, allSatr, false);
				copy.CopyArea (0, 0, 1, HEIGHT, area, i, 0);
				allSatr -= saturV;
				allSatr = Math.Max(allSatr, 0);
			}
			
			window.DrawPixbuf(gc, area, 0, 0, 0, 0, WIDTH, HEIGHT, 0, 0, 0);
			window.CopyToImage(panelImage, 0, 0, 0, 0, -1, -1);
		}
		
		private void redrawPanel()
		{
			window.DrawPixbuf(gc, area, 0, 0, 0, 0, WIDTH, HEIGHT, 0, 0, 0);
		}
		
		private void drawClickArc()
		{
			gc.RgbFgColor = new Gdk.Color(255, 255, 255);
			window.DrawArc(gc, false, arcX - 3, arcY - 3, 6, 6, 0, 360 * 64);
			gc.RgbFgColor = new Gdk.Color(0, 0, 0);
			window.DrawArc(gc, false, arcX - 4, arcY - 4, 8, 8, 0, 360 * 64);
		}
		
		public ushort Red {
			get {
				return (ushort)((panelImage.GetPixel(arcX, arcY) & 0xff0000) >> 16);
			}
		}
		
		public ushort Green {
			get {
				return (ushort)((panelImage.GetPixel(arcX, arcY) & 0xff00) >> 8);
			}
		}
		
		public ushort Blue {
			get {
				return (ushort)(panelImage.GetPixel(arcX, arcY) & 0xff);
			}
		}
		
		public int ArcX {
			get {
				return arcX;
			}
		}
		
		public int ArcY {
			get {
				return arcY;
			}
		}

		public static ColorSquare Inst {
			get {
				return inst;
			}
		}
	}
}

