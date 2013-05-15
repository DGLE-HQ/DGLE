/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;

namespace FontTool
{
	public partial class FontStyleWindow :
		Gui.CustomWindow
	{
		private Gtk.ListStore fontListStore;
		private FontTool.FontService fontService;

		public FontStyleWindow() : 
				base (Gtk.WindowType.Toplevel)
		{
			this.Build();

			this.btnNextPreview.ImagePosition = Gtk.PositionType.Right;
			this.btnPrevSettings.ImagePosition = Gtk.PositionType.Left;

			this.DeleteEvent += (o, args) =>
			{
				Gtk.Application.Quit();
				args.RetVal = true;
			};

			this.SaveAction.Activated += HandleSaveAction;

			this.CloseAction.Activated += (sender, e) =>
				Gtk.Application.Quit();

			this.AboutAction.Activated += (sender, e) =>
				new Gui.AboutWindow(this, About.Authors).Show();

			this.btnNextPreview.Clicked += (sender, e) =>
			{
				notebookFont.NextPage();
				BuildPreview();
			};
			
			this.btnPrevSettings.Clicked += (sender, e) =>
				notebookFont.PrevPage();

			fontService = new FontTool.FontService();
			BuildFontFamilyTree();
			AssignSignals();

			this.fontService.OnlyEnglish = this.OnlyEnglishAction.Active;
		}

		private void BuildFontFamilyTree()
		{
			fontListStore = new Gtk.ListStore(typeof(string), typeof(Pango.FontFamily));
			treeFont.Model = fontListStore;
			treeFont.HeadersVisible = false;
			treeFont.Selection.Changed += HandleSelectionFontFamily;

			Gtk.TreeViewColumn fontNameColumn = new Gtk.TreeViewColumn();
			Gtk.CellRendererText fontNameCell = new Gtk.CellRendererText();
			fontNameColumn.Title = "Font";
			fontNameColumn.PackStart(fontNameCell, false);
			fontNameColumn.SetCellDataFunc(fontNameCell, new Gtk.TreeCellDataFunc(RenderCellFamilyName));

			treeFont.AppendColumn(fontNameColumn);

			LoadFonts();
		}

		private void AssignSignals()
		{
			this.sbSize.ValueChanged += (sender, e) =>
			{
				fontService.Size = (int)(sender as Gtk.SpinButton).Value;
				preview.QueueDraw();
			};
			
			this.sbStretch.ValueChanged += (sender, e) =>
			{
				fontService.Spacing = (int)(sender as Gtk.SpinButton).Value;
				preview.QueueDraw();
			};
			
			this.cbItalic.Clicked += (sender, e) =>
			{
				fontService.SetItalic(((Gtk.CheckButton) sender).Active);
				preview.QueueDraw();
			};

			this.cbBold.Clicked += (sender, e) =>
			{
				if (((Gtk.CheckButton)sender).Active)
					fontService.Weight = (int)Pango.Weight.Bold;
				else
					fontService.Weight = (int)Pango.Weight.Normal;
				preview.QueueDraw();
			};

			this.cbUnderline.Clicked += (sender, e) =>
			{
				fontService.SetUnderline(((Gtk.CheckButton) sender).Active);
				preview.QueueDraw();
			};
			
			this.cbStrikeout.Clicked += (sender, e) =>
			{
				fontService.SetStrikeout(((Gtk.CheckButton) sender).Active);
				preview.QueueDraw();
			};
			
			this.preview.ExposeEvent += (o, args) =>
				fontService.DrawSample(this.preview.GdkWindow, this.ePreviewSample.Text);
			
			this.ePreviewSample.Changed += (sender, e) =>
				preview.QueueDraw();
			
			this.OnlyEnglishAction.Toggled += (sender, e) =>
			{
				fontService.OnlyEnglish = ((Gtk.ToggleAction)sender).Active;
				if (fontPreviewImage != null && 
				    notebookFont.Page >= notebookFont.PageNum(fontPreviewImage))
					BuildPreview();
			};
		}

		private void BuildPreview()
		{
			fontPreviewImage.BuildPreview(fontService);
			lImageSize.Text = String.Format("{0} x {1}", 
			                                fontPreviewImage.PreviewSize.Width, 
			                                fontPreviewImage.PreviewSize.Height);
			lFileSize.Text = String.Format("{0} KB", 
			                               fontPreviewImage.FileSize / 1024);
		}

		private void HandleSelectionFontFamily(object sender, EventArgs e)
		{
			Gtk.TreeSelection selection = (sender as Gtk.TreeSelection);
			
			Gtk.TreeModel model;
			Gtk.TreeIter iter;
			
			if (selection.GetSelected(out model, out iter))
			{
				fontService.Family = model.GetValue(iter, 1) as Pango.FontFamily;
				preview.QueueDraw();
			}
		}

		private void RenderCellFamilyName(Gtk.TreeViewColumn column, Gtk.CellRenderer cell, Gtk.TreeModel model, Gtk.TreeIter iter)
		{
			Pango.FontFamily family = model.GetValue(iter, 1) as Pango.FontFamily;
			(cell as Gtk.CellRendererText).Text = family.Name;
			
			Pango.FontDescription desc = new Pango.FontDescription();
			desc.Family = family.Name;
			desc.Size = (int)(FontTool.FontService.FONT_DEFAULT_SIZE * Pango.Scale.PangoScale);
			
			(cell as Gtk.CellRendererText).FontDesc = desc;
		}

		private void LoadFonts()
		{
			fontListStore.Clear();
			
			foreach (Pango.FontFamily family in this.PangoContext.Families)
				fontListStore.AppendValues(family.Name, family);
			fontListStore.SetSortColumnId(0, Gtk.SortType.Ascending);
			
			Gtk.TreeIter iter;
			if (fontListStore.GetIterFirst(out iter))
				treeFont.Selection.SelectIter(iter);
		}

		private void HandleSaveAction(object sender, EventArgs e)
		{
			Gui.CustomFileChooserDialog dlg = 
				new Gui.CustomFileChooserDialog(this, "Save", Gtk.FileChooserAction.Save);
			
			dlg.FileChooser.SelectMultiple = false;
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter())
			{
				filter.Name = "DGLE Bitmap Fonts";
				filter.AddMimeType("font/dft");
				filter.AddPattern("*.dft");
				dlg.FileChooser.AddFilter(filter);
			}
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter())
			{
				filter.Name = "All";
				filter.AddPattern("*.*");
				dlg.FileChooser.AddFilter(filter);
			}
			
			dlg.FileChooser.CurrentName = 
				String.Format(@"{0}_{1}", fontService.Family.Name, fontService.Size);
			
			dlg.Ok += (sender1, e1) =>
			{
				string fileName = System.IO.Path.GetFullPath(dlg.FileChooser.Filename);
				fontPreviewImage.DftUtil.Save(fileName, fontService);
				dlg.Destroy();
			};

			dlg.Cancel += (sender2, e2) =>
				dlg.Destroy();

			dlg.Show();
		}
	}
}

