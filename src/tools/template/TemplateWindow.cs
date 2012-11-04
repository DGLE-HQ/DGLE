/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;

namespace Template
{
	public partial class TemplateWindow :
		Gui.CustomWindow// use custom decoration
	{
		public TemplateWindow() : 
				base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			this.CustomStyleAction.Active = !base.Decorated;
			this.DecoratedCustomStyleAction.Active = base.Decorated;

			this.DeleteEvent += HandleDeleteEvent;

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
			this.CloseAction.Activated += delegate(object sender, EventArgs e) {
				Gtk.Application.Quit ();
			};
			this.AboutAction.Activated += delegate(object sender, EventArgs e) {
				new Gui.AboutWindow(this, About.Authors).Show();
			};
		}

		private void HandleDeleteEvent (object o, Gtk.DeleteEventArgs args)
		{
			Gtk.Application.Quit ();
			args.RetVal = true;
		}
	}
}

