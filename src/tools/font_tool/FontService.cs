/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;

namespace FontTool
{
	public class FontService
	{
		public static readonly int FONT_DEFAULT_SIZE = 12;
		public static readonly int FONT_MAX_SIZE = 72;
		public static readonly int FONT_MIN_SIZE = 4;

		private Pango.FontFamily family;
		public Pango.FontFamily Family {
			get { return family; }
			set { family = value; }
		}

		// in pt
		private int size = 12;
		public int Size {
			get {
				return size;
			}
			set {
				size = value;
				size = Math.Max(size, FONT_MIN_SIZE);
				size = Math.Min(size, FONT_MAX_SIZE);
			}
		}

		// x100
		// Default is Normal
		private int weight = 400;
		public int Weight {
			get { return weight; }
			set { weight = value; }
		}

		// in pt
		private int spacing = 0;
		public int Spacing {
			get {
				return spacing;
			}
			set {
				spacing = value;
				letterSpacing = new Pango.AttrLetterSpacing((int) (spacing * Pango.Scale.PangoScale));
			}
		}

		private Pango.Style style = Pango.Style.Normal;
		public Pango.Style Style {
			get {
				return style;
			}
		}

		private Pango.AttrUnderline underline = new Pango.AttrUnderline(Pango.Underline.None);
		public Pango.AttrUnderline Underline {
			get {
				return underline;
			}
		}

		private Pango.AttrStrikethrough strikeout = new Pango.AttrStrikethrough(false);
		public Pango.AttrStrikethrough Strikeout {
			get {
				return strikeout;
			}
		}

		private Pango.AttrLetterSpacing letterSpacing = new Pango.AttrLetterSpacing(0);
		public Pango.AttrLetterSpacing LetterSpacing {
			get {
				return letterSpacing;
			}
		}

		public bool OnlyEnglish = false;

		public void SetItalic (bool italic)
		{
			if (italic)
				this.style = Pango.Style.Italic;
			else this.style = Pango.Style.Normal;
		}

		public void SetStrikeout (bool strikeout)
		{
			this.strikeout = new Pango.AttrStrikethrough(strikeout);
		}

		public void SetUnderline (bool underline)
		{
			if (underline)
				this.underline = new Pango.AttrUnderline(Pango.Underline.Single);
			else this.underline = new Pango.AttrUnderline(Pango.Underline.None);
		}

		public void AssignLayout (Pango.Layout layout)
		{
			Pango.FontDescription desc = Pango.FontDescription.FromString(family.Name);
			desc.Size = (int) (size * Pango.Scale.PangoScale);
			desc.Style = style;
			desc.Weight = (Pango.Weight) weight;
			layout.FontDescription = desc;

			Pango.AttrList attributes = new Pango.AttrList();
			attributes.Insert(underline);
			attributes.Insert(strikeout);
			attributes.Insert(letterSpacing);
			layout.Attributes = attributes;
		}

		public void DrawSample (Gdk.Drawable drawable, string text)
		{
			using (Cairo.Context ctx = Gdk.CairoHelper.Create (drawable)) {
				using (Pango.Layout layout = Pango.CairoHelper.CreateLayout (ctx)) {
				
					AssignLayout (layout);
					layout.SetText (text);

					// dest size
					int width, height;
					drawable.GetSize (out width, out height);

					// set pos
					Pango.Rectangle te = Pango.Rectangle.Zero;
					if (text.Length > 0) {
						Pango.Rectangle unused;
						layout.GetPixelExtents (out unused, out te);
						te.X = (width - te.Width) / 2;
						te.Y = (height - te.Height) / 2;
					}
					
					// fill background
					ctx.Save ();
					int boxSize = 20, padding = 0;
					Cairo.Color clr1 = Gui.CairoExtensions.RgbToColor(0xE77817);
					Cairo.Color clr2 = Gui.CairoExtensions.RgbToColor(0x383431);
					for (int i = 0; i < width; i++) {
						for (int j = 0; j < height; j++) {
							ctx.Rectangle (i * (boxSize + padding), j * (boxSize + padding), boxSize, boxSize);
							if ((i + j) % 2 == 0)
								ctx.SetSourceRGBA (clr1.R, clr1.G, clr1.B, clr1.A);
							else
								ctx.SetSourceRGBA (clr2.R, clr2.G, clr2.B, clr2.A);
							ctx.Fill();
						}
					}	
					ctx.Restore ();
					
					// show text
					if (text.Length > 0) {
						ctx.Save ();
						ctx.MoveTo (te.X, te.Y);
						ctx.Color = new Cairo.Color(1.0, 1.0, 1.0, 1.0);
						ctx.Antialias = Cairo.Antialias.Gray;
						ctx.Operator = Cairo.Operator.Source;
						Pango.CairoHelper.ShowLayout (ctx, layout);
						ctx.Restore ();
					}
				}
			}
		}
	}
}

