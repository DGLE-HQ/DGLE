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
			GuiWidgets.ThemingHelper.LoadTheme();

			Application.Init ();
			MainWindow win = new MainWindow ();
			win.Show ();
			Application.Run ();

			GuiWidgets.ThemingHelper.UnloadTheme();
		}
	}
}
