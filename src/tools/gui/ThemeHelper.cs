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
using System.Collections.Generic;

namespace Gui
{
	public static class ThemeHelper
	{

		private static string lightId = "Gui.resources.light_gtkrc";
		private static string darkId = "Gui.resources.dark_gtkrc";

		internal static string minimizeBtnId = "Gui.resources.minimize_button.png";
		internal static string maximizeBtnId = "Gui.resources.maximize_button.png";
		internal static string closeBtnId = "Gui.resources.close_button.png";

		private static Dictionary<string, string> stylesTypes = GetStylesTypes ();
		// static initialization doesn't work on .net 2.0
		private static Dictionary<string, string> GetStylesTypes ()
		{
			Dictionary<string, string> styles = new Dictionary<string, string>();
			styles.Add ("light", lightId);
			styles.Add ("dark", darkId);
			
			return styles;
		}

		static ThemeHelper ()
		{
			CopyEngineForWindows ();
		}

		public static void ApplyCustomTheme (string[] args)
		{
			if (args.Length > 0) {
				Gui.ThemeHelper.ApplyCustomTheme (args[0]);
			} else {
				Gui.ThemeHelper.ApplyCustomTheme (Gui.ThemeHelper.lightId);
			}
		}

		public static void ApplyCustomTheme(string styleType)
		{
			string resourceId = GetResourceId (styleType);

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

		private static string GetResourceId (string styleType)
		{
			string resourceId;
			stylesTypes.TryGetValue (styleType, out resourceId);
			return resourceId ?? lightId;
		}

		private static void CopyEngineForWindows ()
		{
			if (PlatformUtils.IsWindows) {
				if (Gtk.Rc.FindModuleInPath ("libclearlooks.dll") == null) {
					File.Copy(@".\libclearlooks.dll", Gtk.Rc.ModuleDir + @"\libclearlooks.dll");
				}
			}
		}
	}
}

