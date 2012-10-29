/**
\author		Shestakov Mikhail aka MIKE
\date		28.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;

namespace FontTool
{
	[System.ComponentModel.ToolboxItem(true)]
	public class FontPreviewWidget : Gtk.DrawingArea
	{
		public Gdk.Size PreviewSize {
			get { return size; }
		}

		public long FileSize {
			get { return fileSize; }
		}

		public DftUtil DftUtil {
			get { return dftUtil; }
		}

		private DftUtil dftUtil;
		private Gdk.Pixbuf tablePreviewBuf;
		private Gdk.Size size = Gdk.Size.Empty;
		private long fileSize = 0;

		public FontPreviewWidget ()
		{
			this.dftUtil = new DftUtil ();
		}

		public void BuildPreview (FontService fontService)
		{
			tablePreviewBuf = dftUtil.BuildImage (fontService);
			base.SetSizeRequest (tablePreviewBuf.Width, tablePreviewBuf.Height);
			size = new Gdk.Size (tablePreviewBuf.Width, tablePreviewBuf.Height);
			fileSize = dftUtil.CalcFileSize (tablePreviewBuf);
			base.QueueDraw ();
		}

		protected override bool OnExposeEvent (Gdk.EventExpose ev)
		{
			if (null != tablePreviewBuf) {
				tablePreviewBuf.RenderToDrawable (
					base.GdkWindow, 
					base.Style.BackgroundGC (Gtk.StateType.Normal), 
					0, 0, 0, 0, -1, -1, 
					Gdk.RgbDither.Normal, 0, 0);
			
				return true;
			} else {
				return base.OnExposeEvent (ev);
			}
		}
	}
}

