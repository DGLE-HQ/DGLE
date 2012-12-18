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
	public class ColorSaturation : Gtk.DrawingArea
	{
		private static readonly int WIDTH = 128, HEIGHT = 14;
		private bool firstDraw = true;
		private Window window;
		private Gdk.GC gc;
		private ushort red, green, blue;
		private int arcX;
		private Pixbuf area, column, copy;
		private Image panelImage;
		
		public ColorSaturation ()
		{
			area = new Pixbuf(Gdk.Colorspace.Rgb, false, 8, WIDTH, HEIGHT); 
			column = new Pixbuf(Gdk.Colorspace.Rgb, false, 8, 1, HEIGHT);
			copy = new Pixbuf(Gdk.Colorspace.Rgb, false, 8, 1, HEIGHT);
			panelImage = new Image(ImageType.Normal, Visual, WIDTH, HEIGHT);
			this.Events = Gdk.EventMask.ButtonPressMask;
		}
		
		public void ClickProcessing (ushort red, ushort green, ushort blue)
		{
			arcX = 0;
			
			if ((red != this.red) || (green != this.green) || (blue != this.blue)) {
				redrawPanel (red, green, blue);
			} else {
				redrawPanel();
			}
			drawClickArc();
			
			this.red = red;
			this.green = green;
			this.blue = blue;
		}
		
		public void ClickProcessing (int x)
		{
			clickProcessing(X);
		}
		
		protected override bool OnButtonPressEvent (Gdk.EventButton ev)
		{
			clickProcessing((int)ev.X);
			ClickEventHandler.Inst.SaturationClicked();
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
		
		private void clickProcessing (int x)
		{
			this.arcX = x;
			redrawPanel();
			drawClickArc();
		}
		
		private void redrawPanel (ushort red, ushort green, ushort blue)
		{
			createSourceColumn(red, green, blue);
			createArea();
			drawArea();
		}
		
		private void createSourceColumn (ushort red, ushort green, ushort blue)
		{
			column.Fill((uint)(((red & 0xff) << 24) + ((green & 0xff) << 16) + ((blue & 0xff) << 8)));
		}
		
		private void createArea ()
		{
			float allSatr = 1.0f, saturV = allSatr / (WIDTH - 2);
			
			for (int i = 0; i < WIDTH; i++) {
				column.SaturateAndPixelate(copy, allSatr, false);
				copy.CopyArea (0, 0, 1, HEIGHT, area, i, 0);
				allSatr -= saturV;
				allSatr = Math.Max(allSatr, 0);
			}
		}
		
		private void drawArea ()
		{
			window.DrawPixbuf(gc, area, 0, 0, 0, 0, WIDTH, HEIGHT, 0, 0, 0);
			window.CopyToImage(panelImage, 0, 0, 0, 0, -1, -1);
		}
		
		private void redrawPanel()
		{
			window.DrawPixbuf(gc, area, 0, 0, 0, 0, -1, -1, 0, 0, 0);
		}
		
		private void drawClickArc()
		{
			gc.RgbFgColor = new Gdk.Color(255, 255, 255);
			window.DrawArc(gc, false, arcX - 3, HEIGHT / 2 - 3, 6, 6, 0, 360 * 64);
			gc.RgbFgColor = new Gdk.Color(0, 0, 0);
			window.DrawArc(gc, false, arcX - 4, HEIGHT / 2 - 4, 8, 8, 0, 360 * 64);
		}
		
		public ushort Red {
			get {
				return (ushort)((panelImage.GetPixel(arcX, 0) & 0xff0000) >> 16);
			}
		}
		
		public ushort Green {
			get {
				return (ushort)((panelImage.GetPixel(arcX, 0) & 0xff00) >> 8);
			}
		}
		
		public ushort Blue {
			get {
				return (ushort)(panelImage.GetPixel(arcX, 0) & 0xff);
			}
		}
		
		public int X {
			get {
				return arcX;
			}
		}
	}
}

