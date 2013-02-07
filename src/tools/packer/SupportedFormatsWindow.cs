/**
\author		Shestakov Mikhail aka MIKE
\date		02.02.2013 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Linq;
using Gtk;

namespace Packer
{
	public partial class SupportedFormatsWindow : Gui.CustomWindow
	{
		private ListStore formatsStore;

		public SupportedFormatsWindow(Gtk.Window parent) : 
				base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			base.TransientFor = parent;
			base.SetPosition(Gtk.WindowPosition.CenterOnParent);
			base.Decorated = parent.Decorated;

			InitFormatsView();
		}

		private void InitFormatsView()
		{
			formatsStore = new ListStore(typeof(string), typeof(string));
			formatsView.Model = formatsStore;
			formatsView.HeadersVisible = true;
			formatsView.Selection.Mode = SelectionMode.Browse;
			formatsView.EnableSearch = true;
			formatsView.SearchColumn = 0;
			
			formatsView.AppendColumn("Extension", new CellRendererText(), "text", 0);
			TreeViewColumn columnDesc = formatsView.AppendColumn("Description", new CellRendererText(), "text", 1);
			columnDesc.Sizing = TreeViewColumnSizing.Autosize;

			Packer.SupportedFileSystems.Values.ToList().ForEach(fileSystem => {
				formatsStore.AppendValues(fileSystem.Extension, fileSystem.Description);
			});
		}
	}
}

