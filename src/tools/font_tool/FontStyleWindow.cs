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
	public partial class FontStyleWindow :
		Gui.CustomWindow// use custom theme
	{

		public static string[] authors = new string[]
		{
			"Shestakov Mikhail aka MIKE"
		};

		private Gtk.ListStore fontListStore;
		private FontTool.FontService fontService;


		public FontStyleWindow() : 
				base (Gtk.WindowType.Toplevel)
		{
			this.Build();
			// force to use custom theme
			base.Decorated = false;

			this.DeleteEvent += delegate (object o, Gtk.DeleteEventArgs args) {
				Gtk.Application.Quit ();
				args.RetVal = true;
			};

			this.SaveAction.Activated += delegate(object sender, EventArgs e) {
				FontBuildWindow fbwin = new FontBuildWindow(fontService);
				fbwin.Decorated = base.Decorated;
				fbwin.Show();
			};

			this.CloseAction.Activated += delegate(object sender, EventArgs e) {
				Gtk.Application.Quit ();
			};

			this.AboutAction.Activated += delegate(object sender, EventArgs e) {
				new Gui.AboutWindow(this, authors).Show();
			};

			/*
			this.CustomStyleAction.Activated += delegate(object sender, EventArgs e) {
				if (this.CustomStyleAction.Active) {
					base.Decorated = false;
				}
			};

			this.DecoratedCustomStyleAction.Activated += delegate(object sender, EventArgs e) {
				if (this.DecoratedCustomStyleAction.Active) {
					base.Decorated = true;
				}
			};
			*/

			fontService = new FontTool.FontService ();
			BuildFontFamilyTree ();
			AssignSignals ();

			this.fontService.OnlyEnglish = this.OnlyEnglishAction.Active;
		}
		
		private void BuildFontFamilyTree()
		{
			// Create a column
			Gtk.TreeViewColumn fontNameColumn = new Gtk.TreeViewColumn ();
			fontNameColumn.Title = "Font";			
			
			// Create the text cell that will display content
			Gtk.CellRendererText fontNameCell = new Gtk.CellRendererText ();
			
			// Add the cell to the column
			fontNameColumn.PackStart (fontNameCell, false);
			
			// Add the columns to the TreeView
			treeFont.AppendColumn (fontNameColumn);
			
			// Create a model
			this.fontListStore = new Gtk.ListStore (typeof(string), typeof(Pango.FontFamily));
			
			// Assign the model to the TreeView
			treeFont.Model = fontListStore;
			
			fontNameColumn.SetCellDataFunc(fontNameCell, new Gtk.TreeCellDataFunc (RenderCellFamilyName));
			
			treeFont.Selection.Changed += HandleSelectionFontFamily;
			
			LoadFonts();
		}
		
		private void AssignSignals ()
		{
			this.sbSize.ValueChanged += delegate(object sender, EventArgs e) {
				fontService.Size = (int) (sender as Gtk.SpinButton).Value;
				preview.QueueDraw();
			};
			
			this.sbStretch.ValueChanged += delegate(object sender, EventArgs e) {
				fontService.Spacing = (int) (sender as Gtk.SpinButton).Value;
				preview.QueueDraw();
			};
			
			this.cbItalic.Clicked += delegate(object sender, EventArgs e) {
				fontService.SetItalic(((Gtk.CheckButton) sender).Active);
				preview.QueueDraw();
			};

			this.cbBold.Clicked += delegate(object sender, EventArgs e) {
				if (((Gtk.CheckButton) sender).Active)
					fontService.Weight = (int) Pango.Weight.Bold;
				else fontService.Weight = (int) Pango.Weight.Normal;
				preview.QueueDraw();
			};

			this.cbUnderline.Clicked += delegate(object sender, EventArgs e) {
				fontService.SetUnderline(((Gtk.CheckButton) sender).Active);
				preview.QueueDraw();
			};
			
			this.cbStrikeout.Clicked += delegate(object sender, EventArgs e) {
				fontService.SetStrikeout(((Gtk.CheckButton) sender).Active);
				preview.QueueDraw();
			};
			
			this.preview.ExposeEvent += delegate(object o, Gtk.ExposeEventArgs args) {
				fontService.DrawSample(this.preview.GdkWindow, this.ePreviewSample.Text);
			};
			
			this.ePreviewSample.Changed += delegate(object sender, EventArgs e) {
				preview.QueueDraw();
			};
			
			this.OnlyEnglishAction.Toggled += delegate(object sender, EventArgs e) {
				fontService.OnlyEnglish = ((Gtk.ToggleAction) sender).Active;
			};
		}
		
		protected void HandleSelectionFontFamily (object sender, EventArgs e)
		{
			Gtk.TreeSelection selection = (sender as Gtk.TreeSelection);
			
			Gtk.TreeModel model;
			Gtk.TreeIter iter;
			
			if (selection.GetSelected(out model, out iter)) {
				fontService.Family = model.GetValue (iter, 1) as Pango.FontFamily;
				preview.QueueDraw();
			}
		}
		
		protected void RenderCellFamilyName (Gtk.TreeViewColumn column, Gtk.CellRenderer cell, Gtk.TreeModel model, Gtk.TreeIter iter)
		{
			Pango.FontFamily family = model.GetValue (iter, 1) as Pango.FontFamily;
			(cell as Gtk.CellRendererText).Text = family.Name;
			
			Pango.FontDescription desc = new Pango.FontDescription();
			desc.Family = family.Name;
			desc.Size = (int) (FontTool.FontService.FONT_DEFAULT_SIZE * Pango.Scale.PangoScale);
			
			(cell as Gtk.CellRendererText).FontDesc = desc;
		}
		
		protected void LoadFonts ()
		{
			fontListStore.Clear();
			
			foreach (Pango.FontFamily family in this.PangoContext.Families) {
				fontListStore.AppendValues(family.Name, family);
			}
			fontListStore.SetSortColumnId(0, Gtk.SortType.Ascending);
			
			Gtk.TreeIter iter;
			if (fontListStore.GetIterFirst(out iter))
				fontService.Family = fontListStore.GetValue(iter, 1) as Pango.FontFamily;
		}
	}
}

