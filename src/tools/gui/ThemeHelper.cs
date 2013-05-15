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
using System.Text.RegularExpressions;
using System.Linq;

namespace Gui
{
	public static class ThemeHelper
	{
		private static readonly string lightId = "Gui.resources.light_gtkrc";
		private static readonly  string darkId = "Gui.resources.dark_gtkrc";
		internal static readonly string minimizeBtnId = "Gui.resources.minimize_button.png";
		internal static readonly string maximizeBtnId = "Gui.resources.maximize_button.png";
		internal static readonly string closeBtnId = "Gui.resources.close_button.png";

		public static bool ForceDecoration { get; private set; }

		public static string Style
		{
			get { return style; }
			private set
			{
				style = value;
				ApplyCustomTheme();
			}
		}

		private static string style = "light";
		private static readonly Dictionary<string, string> stylesTypes = new Dictionary<string, string>()
		{
			{ "light", lightId },
			{ "dark", darkId }
		};

		public static void ApplyCustomTheme(string[] args)
		{
			const string argsPattern = "(?<=[-{1,2}|/])(?<name>[a-zA-Z0-9]*)[ |:|=|\"]*(?<value>[\\w|.|?|=|&|+| |:|/|\\\\]*)([ |\"]|$)";
			var pairs = from Match match in Regex.Matches(String.Join(" ", args), argsPattern)
				where match.Groups.Count > 0
				select new
				{
					Key = match.Groups["name"].Value,
					Value = match.Groups["value"].Value
				};

			Style = (
				from pair in pairs
				where Regex.IsMatch(pair.Key, "s|style")
				select pair).DefaultIfEmpty(new { Key = "style", Value = "light" }).First().Value;

			ForceDecoration = (
				from pair in pairs
				where Regex.IsMatch(pair.Key, "d|decorated")
				select pair).Any();
		}

		private static void ApplyCustomTheme()
		{
			if (style == "default")
			{
				ForceDecoration = true;
				return;
			}

			string resourceId = GetResourceId(style);
			Assembly assembly = Assembly.GetExecutingAssembly();
			Stream gtkrcStream = null;
			
			try
			{
				gtkrcStream = assembly.GetManifestResourceStream(resourceId);
				TextReader reader = new StreamReader(gtkrcStream);
				string gtkrcTheme = reader.ReadToEnd();
				Gtk.Rc.DefaultFiles = new string[] { };
				Gtk.Rc.ParseString(gtkrcTheme);
			}
			finally
			{
				if (gtkrcStream != null)
					gtkrcStream.Close();
			}
		}

		private static string GetResourceId(string styleType)
		{
			string resourceId;
			stylesTypes.TryGetValue(styleType, out resourceId);
			return resourceId ?? lightId;
		}

		public static bool CheckGtk()
		{
			if (PlatformUtils.IsWindows)
			{
				if (Gtk.Rc.FindModuleInPath("libclearlooks.dll") == null)
				{
					try
					{
						File.Copy(@".\libclearlooks.dll", Gtk.Rc.ModuleDir + @"\libclearlooks.dll");
					}
					catch (UnauthorizedAccessException)
					{
						Gtk.Application.Init();
						new Gtk.MessageDialog(
							null,
							Gtk.DialogFlags.Modal,
							Gtk.MessageType.Warning,
							Gtk.ButtonsType.Close,
							"Please, run {0} as Administrator.",
							Assembly.GetEntryAssembly().GetName().Name).Run();
						Gtk.Application.Quit();
						return false;
					}
					catch (IOException)
					{
						Gtk.Application.Init();
						new Gtk.MessageDialog(
							null,
							Gtk.DialogFlags.Modal,
							Gtk.MessageType.Warning,
							Gtk.ButtonsType.Close,
							"Please, reinstall gtk-sharp correctly.").Run();
						Gtk.Application.Quit();
						return false;
					}
				}
			}

			return true;
		}
	}
}

