/**
\author		Shestakov Mikhail aka MIKE
\date		28.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;

namespace Gui
{
	public partial class CustomFileChooserDialog : Gui.CustomWindow
	{
		public Gtk.FileChooserWidget FileChooser {
			get {
				return fileChooser;
			}
		}

		public event EventHandler Ok;
		public event EventHandler Cancel;

		protected virtual void OnOk(object sender, EventArgs e)
		{
			if (fileChooser.Filename == null || fileChooser.Filename.Length == 0)
				return;

			this.Hide();
			if (Ok != null)
				Ok(sender, e);
		}

		protected virtual void OnCancel(object sender, EventArgs e)
		{
			this.Hide();
			if (Cancel != null)
				Cancel(sender, e);
		}

		public CustomFileChooserDialog(Gtk.Window parent) : 
			this(parent, "Open", Gtk.FileChooserAction.Open)
		{
		}

		public CustomFileChooserDialog(Gtk.Window parent, string title, Gtk.FileChooserAction action) : 
			base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			base.TransientFor = parent;
			base.SetPosition(Gtk.WindowPosition.CenterOnParent);
			base.Decorated = parent.Decorated;

			base.Title = title;
			this.fileChooser.Action = action;
			this.btnOk.Label = action.ToString();

			// hack for correct expand file chooser
			Gtk.Expander expander = GetFileChooserExpander(this.fileChooser);
			if (expander != null) {
				base.AllowGrow = expander.Expanded;
				expander.Activated += delegate(object sender, EventArgs e) {
					base.AllowGrow = expander.Expanded;
					if (expander.Expanded) {
						base.Resize(base.DefaultWidth, base.DefaultHeight);
					}
				};
			}

			this.btnOk.Clicked += OnOk;
			this.btnCancel.Clicked += OnCancel;

			this.DeleteEvent += delegate(object o, Gtk.DeleteEventArgs args) {
				this.btnCancel.Click();
			};
		}

		private Gtk.Expander GetFileChooserExpander(Gtk.Container parent)
		{
			if (parent is Gtk.Expander)
				return parent as Gtk.Expander;

			foreach (Gtk.Widget child in parent.AllChildren) {
				if (child is Gtk.Container) {
					Gtk.Container container = child as Gtk.Container; 
					Gtk.Expander expander = GetFileChooserExpander(container); 
					if (expander != null)
						return expander;
				}
			}

			return null;
		} 
	}
}

