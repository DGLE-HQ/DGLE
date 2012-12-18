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
		private static readonly int WIDTH = 128, HEIGHT = 128;
		private Window window;
		private Gdk.GC gc;
		private bool firstDraw = true;
		private int arcX, arcY;
		private Pixbuf originColumn, copy, area;
		private Image panelImage;
		
		public ColorSquare ()
		{
			originColumn = new Pixbuf (Gdk.Colorspace.Rgb, false, 8, 1, HEIGHT);
			copy = new Pixbuf (Gdk.Colorspace.Rgb, false, 8, 1, HEIGHT);
			area = new Pixbuf (Gdk.Colorspace.Rgb, false, 8, WIDTH, HEIGHT);
			panelImage = new Image (ImageType.Normal, Visual, WIDTH, HEIGHT);
			this.Events = Gdk.EventMask.ButtonPressMask;
		}
		
		public void ClickProcessing (int arcX, int arcY)
		{
			clickProcessing(arcX, arcY);
		}
		
		public void ClickProcessing (int arcX, int arcY, int spectrX)
		{
			this.arcX = arcX;
			this.arcY = arcY;
			redrawPanel(spectrX);
			drawClickArc();
		}
		
		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
		{
			int x = (int)ev.X, y = (int)ev.Y;
			clickProcessing(x, y);
			ClickEventHandler.Inst.SquareClicked();
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
			requisition.Height = 50;
			requisition.Width = 50;
		}
		
		private void clickProcessing (int arcX, int arcY)
		{
			//for satur., bright.
			this.arcX = arcX;
			this.arcY = arcY;
			redrawPanel();
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
	}
}

