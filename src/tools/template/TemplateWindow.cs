/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;

namespace Template
{
	public partial class TemplateWindow : Gui.CustomWindow
	{
		public TemplateWindow() : base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			this.CustomStyleAction.Active = !base.Decorated;
			this.DecoratedCustomStyleAction.Active = base.Decorated;

			this.DeleteEvent += HandleDeleteEvent;

			this.CustomStyleAction.Activated += (sender, e) => {
				if (this.CustomStyleAction.Active)
					base.Decorated = false;
			};

			this.DecoratedCustomStyleAction.Activated += (sender, e) => {
				if (this.DecoratedCustomStyleAction.Active)
					base.Decorated = true;
			};

			this.CloseAction.Activated += (sender, e) =>
				Gtk.Application.Quit();

			this.AboutAction.Activated += (sender, e) =>
				new Gui.AboutWindow(this, About.Authors).Show();
		}

		private void HandleDeleteEvent(object o, Gtk.DeleteEventArgs args)
		{
			Gtk.Application.Quit();
			args.RetVal = true;
		}
	}
}

