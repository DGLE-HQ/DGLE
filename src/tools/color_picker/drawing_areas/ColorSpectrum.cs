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
	public class ColorSpectrum : Gtk.DrawingArea
	{
		private static ColorSpectrum inst;
		private static readonly int WIDTH = 256, HEIGHT = 128, 
		HALF_HEIGHT = HEIGHT / 2;
		private byte[,] redBuf, greenBuf, blueBuf;
		private ushort red, green, blue, 
		nativeRed, nativeGreen, nativeBlue;
		private bool spectrumDrawn = false, buttonPressed = false;
		private Window window;
		private Gdk.GC gc;
		private int arcX, arcY, aimX, aimY;
		
		public ColorSpectrum ()
		{
			createPointsArrays();
			this.Events = Gdk.EventMask.ButtonPressMask | 
				Gdk.EventMask.PointerMotionMask | 
					Gdk.EventMask.ButtonReleaseMask;
			inst = this;
		}
		
		public void ClickProcessing (ushort red, ushort green, ushort blue)
		{
			findThisColor(red, green, blue);
			clickProcessing(aimX, aimY);
		}
		
		public void ClickProcessing (int x, int y)
		{
			clickProcessing(x, y);
		}
		
		public void ClickProcessing (int y)
		{
			clickProcessing(arcX, y);
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
			
			if (!spectrumDrawn) {
				drawSpectrum();
				clickProcessing(0, HEIGHT - 1);
			} else {
				redrawSpectrum();
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
			requisition.Width = WIDTH;
			requisition.Height = HEIGHT;
		}

		private void motionClickProcessing(int x, int y)
		{
			if ((x >= 0) && (x < WIDTH) && (y >= 0) && (y < HEIGHT)) {
				clickProcessing(x, y);
				ClickEventHandler.Inst.SpectrClicked(x, y);
			}
		}
		
		private void clickProcessing (int x, int y)
		{
			x = Math.Min(x, WIDTH - 1);
			arcX = x;
			arcY = y;
			
			red = redBuf[x,y];
			green = greenBuf[x,y];
			blue = blueBuf[x,y];
			nativeRed = redBuf[x,HALF_HEIGHT];
			nativeGreen = greenBuf[x,HALF_HEIGHT];
			nativeBlue = blueBuf[x,HALF_HEIGHT];
			
			redrawSpectrum();
			drawClickArc ();
		}
		
		private void createPointsArrays ()
		{
			int r = 255, g = 0, b = 0, dr = 0, dg = 5, db = 0;
			float redV, greenV, blueV, redPath, greenPath, bluePath,
			halfHeight = HEIGHT / 2.0f;
			
			redBuf = new byte[WIDTH,HEIGHT];
			greenBuf = new byte[WIDTH,HEIGHT];
			blueBuf = new byte[WIDTH,HEIGHT];
			
			for (int i = 0; i < WIDTH; i++) {
				redV = r / halfHeight;
				blueV = b / halfHeight;
				greenV = g / halfHeight;
				redPath = 0;
				greenPath = 0;
				bluePath = 0;
				
				for (int k = 0; k < HEIGHT; k++) 
				{
					redBuf[i,k] = (byte)redPath;
					greenBuf[i,k] = (byte)greenPath;
					blueBuf[i,k] = (byte)bluePath;
					
					if(k == (int)halfHeight) {
						redV = (255 - r) / (halfHeight - 1);
						blueV = (255 - b) / (halfHeight - 1);
						greenV = (255 - g) / (halfHeight - 1);
					}
					
					redPath += redV;
					greenPath += greenV;
					bluePath += blueV;
				}
				
				r += dr;
				g += dg;
				b += db;
				
				switch(i) {
					case 24: r = 255; g = 127; b = 0; break;
					case 49: r = 255; g = 255; b = 0; dr = -5; dg = 0; break;
					case 100: r = 0; g = 255; b = 0; dr = 0; db = 5; break;
					case 151: r = 0; g = 255; b = 255; dg = -5;  db = 0; break;
					case 202: r = 0; g = 0; b = 255; dr = 5; dg = 0; break;
					case 253: r = 255; g = 0; b = 255; dr = 0; break;
					default : break;
				}
			}
		}
		
		private void drawSpectrum ()
		{
			spectrumDrawn = true;
			window = this.GdkWindow;
			gc = new Gdk.GC (window);
			
			for (int i = 0; i < WIDTH; i++) {
				for(int k = 0; k < HEIGHT; k++) {
					gc.RgbFgColor = new Gdk.Color(redBuf[i,k],
					                              greenBuf[i,k],
					                              blueBuf[i,k]);
					window.DrawPoint(gc, i, k);
				}
			}
			
			SpectrumVerticalPixbuf.Pixbuf = SpectrumVerticalPixbuf.Pixbuf.GetFromDrawable(window, Gdk.Colormap.System, 
			                                                                              0, 0, 0, 0, WIDTH, HEIGHT);
		}
		
		private void redrawSpectrum ()
		{
			window.DrawPixbuf(gc, SpectrumVerticalPixbuf.Pixbuf, 0, 0, 0, 0, -1, -1, 0, 0, 0);
		}
		
		private void drawClickArc ()
		{
			gc.RgbFgColor = new Gdk.Color(255, 255, 255);
			window.DrawArc(gc, false, arcX - 3, arcY - 3, 6, 6, 0, 360 * 64);
			gc.RgbFgColor = new Gdk.Color(0, 0, 0);
			window.DrawArc(gc, false, arcX - 4, arcY - 4, 8, 8, 0, 360 * 64);
		}
		
		private void findThisColor (ushort red, ushort green, ushort blue)
		{
			int currX = 0, currY = 0, 
			redDev = 255, greenDev = 255, blueDev = 255, allDev = 255*3,
			tempRedDev, tempGreenDev, tempBlueDev;
			byte redB = (byte)red, greenB = (byte)green, blueB = (byte)blue;
			
			for (int i = 0; i < WIDTH; i++) {
				for(int k = 0; k < HEIGHT; k++) {
					tempRedDev = Math.Abs(redBuf[i,k] - redB);
					tempGreenDev = Math.Abs(greenBuf[i,k] - greenB);
					tempBlueDev = Math.Abs(blueBuf[i,k] - blueB);
					
					if(((tempRedDev <= redDev) && (tempGreenDev <= greenDev) &&
					    (tempBlueDev <= blueDev)) || (tempRedDev + tempGreenDev + 
					                              tempBlueDev < allDev)) {
						currX = i;
						currY = k;
						
						redDev = tempRedDev;
						greenDev = tempGreenDev;
						blueDev = tempBlueDev;
						allDev = redDev + greenDev + blueDev;
					}
				}
			}
			
			aimX = currX;
			aimY = currY;
		}
		
		public ushort Red {
			get {
				return red;
			}
			set {
				red = value;
			}
		}
		
		public ushort Green {
			get {
				return green;
			}
			set {
				green = value;
			}
		}
		
		public ushort Blue {
			get {
				return blue;
			}
			set {
				blue = value;
			}
		}
		
		public ushort NativeRed {
			get {
				return nativeRed;
			}
		}
		
		public ushort NativeGreen {
			get {
				return nativeGreen;
			}
		}
		
		public ushort NativeBlue {
			get {
				return nativeBlue;
			}
		}
		
		public int ArcY {
			get {
				return arcY;
			}
		}
		
		public int ArcX {
			get {
				return arcX;
			}
		}

		public static ColorSpectrum Inst {
			get {
				return inst;
			}
		}
	}
}


