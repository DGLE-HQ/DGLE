/**
\author		Shestakov Mikhail aka MIKE
\date		22.10.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Reflection;

namespace Gui
{
	public partial class AboutWindow : CustomWindow
	{

		public AboutWindow(Gtk.Window parent, string[] authors) : 
			this(parent, authors, new string[] {}, Gdk.Size.Empty)
		{
		}

		public AboutWindow(Gtk.Window parent, string[] authors, string[] lines, Gdk.Size size) : 
			base(Gtk.WindowType.Toplevel)
		{
			this.Build();

			base.TransientFor = parent;
			base.SetPosition(Gtk.WindowPosition.CenterOnParent);
			base.Decorated = parent.Decorated;
			if (!size.IsEmpty)
				base.SetSizeRequest(size.Width, size.Height);

			Assembly assembly = Assembly.GetEntryAssembly();
			AssemblyName assemblyName = assembly.GetName();

			this.textviewAbout.Buffer.Text = String.Format(
				"{0} v{1}.{2}\n", 
				assemblyName.Name,
				assemblyName.Version.Major,
				assemblyName.Version.Minor);

			this.textviewAbout.Buffer.Text += "\n*Authors*\n";
			foreach (string author in authors)
			{
				this.textviewAbout.Buffer.Text += String.Format("{0}\n", author);
			}
			foreach (string line in lines)
			{
				this.textviewAbout.Buffer.Text += String.Format("{0}\n", line);
			}
		}
	}
}

