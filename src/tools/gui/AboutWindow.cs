/**
\author		MIKE
\date		22.10.2012 (c)MIKE

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using System.Reflection;

namespace Gui
{
	public partial class AboutWindow : CustomWindow
	{
		public AboutWindow(Gtk.Window parent, string[] authors) : this(parent, authors, "")
		{
		}

		public AboutWindow(Gtk.Window parent, string[] authors, string text) : 
			base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			base.Parent = parent;
			base.TransientFor = parent;
			base.SetPosition(Gtk.WindowPosition.CenterOnParent);

			base.Decorated = parent.Decorated;

			Assembly assembly = Assembly.GetEntryAssembly();
			AssemblyName assemblyName = assembly.GetName();

			this.textviewAbout.Buffer.Text = String.Format("{0} v{1}.{2}\n", 
			                           assemblyName.Name,
			                           assemblyName.Version.Major,
			                           assemblyName.Version.Minor);

			this.textviewAbout.Buffer.Text += "*Authors*\n";
			foreach(string author in authors) {
				this.textviewAbout.Buffer.Text += String.Format("{0}\n", author);
			}
			this.textviewAbout.Buffer.Text += text;
		}
	}
}

