// \author		Макс
// \date		18.12.2012 (c)Макс
//
// This file is a part of DGLE project and is distributed
// under the terms of the GNU Lesser General Public License.
// See "DGLE.h" for more details.
using System;
using Gtk;

namespace color_picker
{
	class MainClass
	{
		public static void Main(string[] args)
		{
			if (!Gui.ThemeHelper.CheckGtk ())
				return;
			Gui.ThemeHelper.ApplyCustomTheme (args);

			Application.Init("ColorPicker", ref args);
			MainWindow win = new MainWindow();
			win.Show();
			Application.Run();
		}
	}
}
