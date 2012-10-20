/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using Gtk;

namespace FontTool
{
	public partial class FontBuildWindow :
		Gui.CustomWindow// use custom theme
	{
		private DftUtil dftUtil;
		private FontService fontService;
		private Gdk.Pixbuf tablePreviewBuf;

		public FontBuildWindow (FontService fontService) : 
				base(Gtk.WindowType.Toplevel)
		{
			this.Build ();

			this.dftUtil = new DftUtil ();
			this.fontService = fontService;

			this.fileSaveWidget.SelectMultiple = false;

			using (FileFilter filter = new FileFilter()) {
				filter.Name = "DGLE2 Bitmap Fonts";
				filter.AddMimeType ("font/dft");
				filter.AddPattern ("*.dft");
				this.fileSaveWidget.AddFilter (filter);
			}

			using (FileFilter filter = new FileFilter()) {
				filter.Name = "All";
				filter.AddPattern ("*.*");
				this.fileSaveWidget.AddFilter (filter);
			}

			this.fileSaveWidget.CurrentName = 
				String.Format(@"{0}_{1}", fontService.Family.Name, fontService.Size);

			tablePreviewBuf = this.dftUtil.BuildImage (fontService);
			this.drawingarea3.SetSizeRequest (tablePreviewBuf.Width, tablePreviewBuf.Height);
			this.eWidth.Text = tablePreviewBuf.Width.ToString();
			this.eHeight.Text = tablePreviewBuf.Height.ToString();
			
			this.DeleteEvent += HandleDeleteEvent;
			this.drawingarea3.ExposeEvent += DrawPreviewFontTable;

			this.btnCancel.Clicked += delegate (object sender, EventArgs e) {
				this.Destroy();
			};

			this.btnSave.Clicked += HandleSave;
		}

		protected void HandleSave (object sender, EventArgs e)
		{
			if (null == fileSaveWidget.Filename ||
			    	fileSaveWidget.Filename.Length == 0) {
				Gtk.MessageDialog infoDlg = new MessageDialog (
					this, DialogFlags.Modal, MessageType.Info, ButtonsType.Ok,
					"Set file name");
				infoDlg.Run ();
				infoDlg.Destroy ();
				return;
			}

			string fileName = System.IO.Path.GetFullPath(fileSaveWidget.Filename);
			this.dftUtil.Save(fileName, fontService);

			/*
			Gtk.MessageDialog successDlg = new MessageDialog (
				this, DialogFlags.Modal, MessageType.Info, ButtonsType.Ok,
				"Done");
			successDlg.Run ();
			successDlg.Destroy ();
			*/

			this.Destroy ();
		}

		protected void DrawPreviewFontTable (object o, Gtk.ExposeEventArgs args)
		{
			tablePreviewBuf.RenderToDrawable (
				this.drawingarea3.GdkWindow, 
				this.drawingarea3.Style.BackgroundGC(Gtk.StateType.Normal), 
				0, 0, 0, 0, -1, -1, 
				Gdk.RgbDither.Normal, 0, 0);
		}

		protected void HandleDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{
			(tablePreviewBuf as IDisposable).Dispose ();
		}
	}
}

