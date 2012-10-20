/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using System.IO;
using System.Reflection;

namespace Gui
{
	public static class ThemeHelper
	{
		public static string lightId = "Gui.light_gtkrc";
		public static string darkId = "Gui.dark_gtkrc";

		public static string minimizeBtnId = "Gui.minimize_button.png";
		public static string maximizeBtnId = "Gui.maximize_button.png";
		public static string closeBtnId = "Gui.close_button.png";

		static ThemeHelper ()
		{
			ApplyCustomTheme (lightId);
		}

		private static void ApplyCustomTheme(string resourceId)
		{
			Assembly assembly = Assembly.GetExecutingAssembly();
			Stream gtkrcStream = null;
			
			try {
				gtkrcStream = assembly.GetManifestResourceStream (resourceId);
				TextReader reader = new StreamReader(gtkrcStream);
				string gtkrc_theme = reader.ReadToEnd();
				Gtk.Rc.DefaultFiles = new string[] {};
				Gtk.Rc.ParseString (gtkrc_theme);
			} catch { 
			} finally {
				if (null != gtkrcStream)
					gtkrcStream.Close ();
			}
		}
	}
}

