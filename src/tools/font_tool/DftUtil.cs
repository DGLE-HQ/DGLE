/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Text;
using System.IO;

namespace FontTool
{
	public class DftUtil
	{
		public static readonly string HEADER = "DFT 1.1";
		private const int WIDTH = 512;
		private const int HEIGHT = 8192 * 2;
		private const int PADDING = 2;
		private char[] alphabet;
		private Pango.Rectangle[] boxes = new Pango.Rectangle[224];

		public DftUtil()
		{
			byte[] encoded = new byte[224];
			for (int k = 32; k < 256; k++)
				encoded[k - 32] = (byte)k;
			
			// cp1251
			encoded = Encoding.Convert(Encoding.GetEncoding(1251), Encoding.Unicode, encoded);
			alphabet = Encoding.Unicode.GetChars(encoded);
		}

		public void Save(string filename, FontService fontService)
		{
			Gdk.Pixbuf pixbuf = null;
			BinaryWriter file = null;
			try
			{
				pixbuf = BuildImage(fontService);
				//pixbuf.Save (String.Format("{0}.bmp", filename), "bmp");
				file = new BinaryWriter(File.Create(String.Format("{0}.dft", filename)), Encoding.ASCII);

				TFontHeader header = new TFontHeader(1, pixbuf.Width, pixbuf.Height);
				file.Write(header);
				file.Write(boxes);
				file.Write(pixbuf);
			}
			finally
			{
				if (file != null)
					file.Close();
				// manual dispose
				if (pixbuf != null)
					(pixbuf as IDisposable).Dispose();
			}
		}

		public long CalcFileSize(Gdk.Pixbuf pixbuf)
		{
			long fileSize = 0L;
			fileSize += 1 + sizeof(Int32) * 2;
			fileSize += sizeof(Int32) * 4 * boxes.Length;
			fileSize += pixbuf.Width * pixbuf.Height;

			return fileSize;
		}

		public void DrawLayout(Gdk.Drawable drawable, Gdk.GC gc, FontService fontService)
		{
			Gdk.Pixbuf pixbuf = BuildImage(fontService);

			int w = -1, h = -1;
			drawable.GetSize(out w, out h);

			pixbuf.RenderToDrawable(drawable, gc, 0, 0, 0, 0, w, h, Gdk.RgbDither.Normal, 0, 0);

			// manual dispose
			(pixbuf as IDisposable).Dispose();
		}

		public Gdk.Pixbuf BuildImage(FontService fontService)
		{
			Cairo.ImageSurface image = new Cairo.ImageSurface(Cairo.Format.ARGB32, WIDTH, HEIGHT);
			Cairo.Context ctx = new Cairo.Context(image);
			
			Pango.Layout layout = Pango.CairoHelper.CreateLayout(ctx);
			fontService.AssignLayout(layout);
			
			// fill background
			ctx.Save();
			ctx.Color = new Cairo.Color(0.0, 0.0, 0.0, 1.0);
			ctx.Paint();
			ctx.Restore();
			
			int charCode = 0;
			int maxHeight = 0;
			Cairo.Point pos = new Cairo.Point(PADDING, PADDING);
			while ((!fontService.OnlyEnglish && charCode < 224) || 
			       (fontService.OnlyEnglish && charCode < (224 - 66)))
			{

				layout.SetText(alphabet[charCode].ToString());

				Pango.Rectangle te = GetTextExtents(layout, pos);

				// next line
				if (pos.X + te.Width + fontService.Spacing + PADDING > image.Width)
				{
					pos.X = PADDING;
					pos.Y = te.Y + maxHeight + PADDING;
				}
				te = DrawText(ctx, layout, pos);
				boxes[charCode] = te;
				
				pos.X = te.X + te.Width + fontService.Spacing + PADDING;
				maxHeight = Math.Max(maxHeight, te.Height);

				charCode++;
			}
			
			int cropHeight = NextP2(boxes[charCode - 1].Y + boxes[charCode - 1].Height - 1);
			Gdk.Pixbuf pixbuf = new Gdk.Pixbuf(
				image.Data, true, 8,
				image.Width, 
				cropHeight,
				image.Stride);			
			
			// manual dispose
			(image as IDisposable).Dispose();
			(layout as IDisposable).Dispose();
			(ctx.Target as IDisposable).Dispose();
			(ctx as IDisposable).Dispose();

			return pixbuf;
		}

		private Pango.Rectangle DrawText(Cairo.Context ctx, Pango.Layout layout, Cairo.Point p)
		{
			ctx.Save();

			ctx.MoveTo(p.X, p.Y);
			ctx.Color = new Cairo.Color(1.0, 1.0, 1.0, 1.0);
			ctx.Antialias = Cairo.Antialias.Gray;
			ctx.Operator = Cairo.Operator.Source;

			Pango.CairoHelper.ShowLayout(ctx, layout);

			ctx.Restore();

			return GetTextExtents(layout, p);
		}

		private Pango.Rectangle GetTextExtents(Pango.Layout layout, Cairo.Point p)
		{
			Pango.Rectangle unused = Pango.Rectangle.Zero;
			Pango.Rectangle te = Pango.Rectangle.Zero;
			layout.GetPixelExtents(out unused, out te);

			te.X += p.X;
			te.Y += p.Y;

			return te;
		}

		public static int NextP2(int a)
		{
			int rval = 1;
			while (rval < a)
				rval <<= 1;
			return rval;
		}
	}

	public struct TFontHeader
	{
		public byte BitDepth;
		public Int32 Width, Height;

		public TFontHeader(byte bitDepth, int width, int height)
		{
			this.BitDepth = bitDepth;
			this.Width = width;
			this.Height = height;
		}
	}

	public static class DFTExtensions
	{
		public static void Write(this BinaryWriter file, TFontHeader header)
		{            
			// "DFT 1.1"
			file.Write(DftUtil.HEADER.ToCharArray ());
			file.Write(header.BitDepth);
			file.Write(header.Width);
			file.Write(header.Height);
		}

		public static void Write(this BinaryWriter file, Pango.Rectangle box)
		{
			file.Write((Int32) box.X);
			file.Write((Int32) box.Y);
			file.Write((Int32) box.Width);
			file.Write((Int32) box.Height);
		}

		public static void Write(this BinaryWriter file, Pango.Rectangle[] boxes)
		{
			foreach (Pango.Rectangle box in boxes)
				Write(file, box);
		}

		public static void Write(this BinaryWriter file, Gdk.Pixbuf pixbuf)
		{
			byte[] buf = pixbuf.SaveToBuffer("bmp");

            for (int i = 1; i <= pixbuf.Height; ++i)
                for (int j = 0; j < pixbuf.Width; ++j)
                {
                    int pos = buf.Length - i * pixbuf.Width * 3 + j * 3;
                    byte data = (byte)(((int)buf[pos + 0] + (int)buf[pos + 1] + (int)buf[pos + 2]) / 3);
                    file.Write(data);
                }
		}
	}
}

