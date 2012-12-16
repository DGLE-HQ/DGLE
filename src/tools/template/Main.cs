/**
\author		Shestakov Mikhail aka MIKE
\date		20.9.2012 (c)Korotkov Andrey

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using Gtk;
using System.Collections.Generic;

namespace Template
{
	internal static class About
	{
		internal static string[] Authors = new string[]
		{
			"Shestakov Mikhail aka MIKE"
		};
	}


	class MainClass
	{
		public static void Main(string[] args)
		{
			if (!Gui.ThemeHelper.CheckGtk ())
				return;
			Gui.ThemeHelper.ApplyCustomTheme (args);

			Application.Init ("Template", ref args);
			TemplateWindow win = new TemplateWindow();
			win.Show();
			Application.Run ();
		}
	}
}
