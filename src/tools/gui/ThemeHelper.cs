/**
\author		Shestakov Mikhail aka MIKE
\date		14.10.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.IO;
using System.Reflection;
using System.Collections.Generic;
using Mono.Options;

namespace Gui
{
	public static class ThemeHelper
	{
		public static bool ForceDecoration {
			get {
				return forceDecoration;
			}
			private set {
				forceDecoration = value;
			}
		}

		public static string Style {
			get {
				return style;
			}
			private set {
				style = value;
				ApplyCustomTheme ();
			}
		}

		private static readonly string lightId = "Gui.resources.light_gtkrc";
		private static readonly  string darkId = "Gui.resources.dark_gtkrc";

		internal static readonly string minimizeBtnId = "Gui.resources.minimize_button.png";
		internal static readonly string maximizeBtnId = "Gui.resources.maximize_button.png";
		internal static readonly string closeBtnId = "Gui.resources.close_button.png";

		private static string style = "light";
		private static bool forceDecoration = false;

		private static readonly Dictionary<string, string> stylesTypes = GetStylesTypes();

		private static Dictionary<string, string> GetStylesTypes ()
		{
			Dictionary<string, string> styles = new Dictionary<string, string>();
			styles.Add ("light", lightId);
			styles.Add ("dark", darkId);
			
			return styles;
		}

		public static void ApplyCustomTheme (string[] args)
		{
			bool applied = false;
			try {
				OptionSet options = new OptionSet ()
				.Add ("s|style=", "theme style", delegate(string value) {
					Style = value;
					applied = true;
				})
				.Add ("d|decorated", "force to decorate with window manager", delegate(string value) {
					ForceDecoration = true;
				});
				options.Parse (args);
			} catch {
			}
			if (!applied)
				ApplyCustomTheme ();
		}

		private static void ApplyCustomTheme ()
		{
			if (style == "default") {
				ForceDecoration = true;
				return;
			}

			string resourceId = GetResourceId (style);

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

		public static bool CheckGtk ()
		{
			if (PlatformUtils.IsWindows) {
				if (Gtk.Rc.FindModuleInPath ("libclearlooks.dll") == null) {
					try {
						File.Copy(@".\libclearlooks.dll", Gtk.Rc.ModuleDir + @"\libclearlooks.dll");
					} catch {
						Gtk.Application.Init();
						new Gtk.MessageDialog (
							null,
							Gtk.DialogFlags.Modal,
							Gtk.MessageType.Warning,
							Gtk.ButtonsType.Close,
							"Please, run {0} as Administrator.",
							Assembly.GetEntryAssembly().GetName().Name
							).Run();
						Gtk.Application.Quit ();
						return false;
					}
				}
			}
			return true;
		}
	}
}

