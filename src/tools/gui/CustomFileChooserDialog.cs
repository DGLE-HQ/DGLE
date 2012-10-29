/**
\author		Shestakov Mikhail aka MIKE
\date		28.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
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

		public event EventHandler Ok
		{
			add { this.btnOk.Clicked += value; }
			remove { this.btnOk.Clicked -= value; }
		}

		public event EventHandler Cancel
		{
			add { this.btnCancel.Clicked += value; }
			remove { this.btnCancel.Clicked -= value; }
		}

		protected virtual void OnOk (object sender, EventArgs e)
		{
		}

		protected virtual void OnCancel (object sender, EventArgs e)
		{
		}

		public CustomFileChooserDialog (Gtk.Window parent) : this (parent, "Open", Gtk.FileChooserAction.Open)
		{
		}

		public CustomFileChooserDialog (Gtk.Window parent, string title, Gtk.FileChooserAction action) : 
			base(Gtk.WindowType.Toplevel)
		{
			this.Build ();

			base.TransientFor = parent;
			base.SetPosition (Gtk.WindowPosition.CenterOnParent);
			base.Decorated = parent.Decorated;

			base.Title = title;
			this.fileChooser.Action = action;
			this.btnOk.Label = action.ToString ();

			this.btnOk.Clicked += OnOk;
			this.btnCancel.Clicked += OnCancel;

			this.DeleteEvent += delegate(object o, Gtk.DeleteEventArgs args) {
				this.btnCancel.Click ();
			};
		}
	}
}

