// \author		Shestakov Mikhail aka MIKE
// \date		20.9.2012 (c) Shestakov Mikhail
//
// This file is a part of DGLE2 project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE2.h" for more details.
using System;
using Gtk;
using System.IO;

namespace ToolTemplate
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			const string rcFile = @"..\..\..\..\resources\tools\style\Elegant Brit\gtk-2.0\gtkrc";
			InitTheme(rcFile);

			Application.Init ();
			MainWindow win = new MainWindow ();
			win.Show ();
			Application.Run ();
		}

		public static void InitTheme (string rcFile)
		{
			if (File.Exists (rcFile))
			{
				Gtk.Rc.AddDefaultFile (rcFile);
				Gtk.Rc.Parse (rcFile);
				Console.WriteLine(rcFile + " is loaded!");
			} else {
				Console.WriteLine(rcFile + " is NOT loaded!");
			}
		}
	}
}
