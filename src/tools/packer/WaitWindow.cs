/**
\author		Shestakov Mikhail aka MIKE
\date		04.02.2013 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;

namespace Packer
{
	public partial class WaitWindow : Gui.CustomWindow
	{
		public string ActionText
		{
			get { return Title; }
			set { Title = value; }
		}

		public string FileText
		{
			get { return lFile.Text; }
			set { lFile.Text = value; }
		}

		public WaitWindow(Gtk.Window parent) :
			this(parent, "", "")
		{
		}

		public WaitWindow(Gtk.Window parent, string action, string file) : 
			base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			base.TransientFor = parent;
			base.SetPosition(Gtk.WindowPosition.CenterOnParent);
			base.Decorated = parent.Decorated;
			base.Title = action;

			base.DeleteEvent += (o, args) =>
			{
				// prevent closing
				args.RetVal = true;
			};

			lFile.Text = System.IO.Path.GetFileName(file);
			imgAnimation.PixbufAnimation = Gdk.PixbufAnimation.LoadFromResource("Packer.resources.progress.gif");
		}
	}
}

