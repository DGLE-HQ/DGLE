/**
\author		Shestakov Mikhail aka MIKE
\date		26.11.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Threading;
using System.Reflection;
using Gtk;
using Gui;
using DGLE;

namespace Packer
{
	internal static class About
	{
		internal static string[] Authors = new string[]
		{
			"Engine File Subsystems:",
			"Andrey Korotkov aka DRON",
			"Interface:",
			"Shestakov Mikhail aka MIKE"
		};

		internal static string[] Lines = new string[] {
			"\n*External resources*",
			"Some icons are taken from Mark James Silk library http://www.famfamfam.com."
		};

		internal static Gdk.Size Size = new Gdk.Size(400, 300);
	}

	class Program
	{
		internal static string DLL_NAME = "DGLE.dll";
		internal static string DLL_PATH = @"..\" + DLL_NAME;

		public static IEngineCore Core { get; private set; }

		public static bool Init(string[] args)
		{
			string appName = Assembly.GetExecutingAssembly().GetName().Name;

			// check gtk
			if (!ThemeHelper.CheckGtk())
				return false;
			ThemeHelper.ApplyCustomTheme(args);
			
			// init gtk app
			Application.Init(appName, ref args);

			// check windows platform
			if (!Gui.PlatformUtils.IsWindows) {
				CustomMessageDialog dlg = new CustomMessageDialog(
					null, MessageType.Error,
					"Your OS is not supported on this version!");
				dlg.Ok += (sender, e) => Application.Quit();
				dlg.Show();
				Application.Run();
				return false;
			}

			// load dgle
			IEngineCore pEngineCore = null;
			if (!Engine.GetEngine(DLL_PATH, out pEngineCore)) {
				CustomMessageDialog dlg = new CustomMessageDialog(
					null, MessageType.Error,
					"Couldn't load \"{0}\"!", DLL_NAME);
				dlg.Ok += (sender, e) => Application.Quit();
				dlg.Show();
				Application.Run();
				return false;
			}

			Core = pEngineCore;

			// init dgle with force no window
			TEngineWindow eng_win = new TEngineWindow();
			Core.InitializeEngine(
				IntPtr.Zero, appName, ref eng_win, 33, 
				E_ENGINE_INIT_FLAGS.EIF_FORCE_NO_WINDOW | E_ENGINE_INIT_FLAGS.EIF_NO_SPLASH);
			//Core.ConsoleVisible(false);

			// init virtual file systems from dgle
			try {
				Packer.Init();
			} catch {
				CustomMessageDialog dlg = new CustomMessageDialog(
					null, MessageType.Error, ButtonsType.Ok, 
					"Failed to load file systems from DGLE!");
				dlg.Ok += (sender, e) => Program.Stop();
				dlg.Show();
				
				return false;
			}

			// init main win
			MainWindow win = new MainWindow();
			win.Show();
			win.DeleteEvent += (o, a) => {
				Stop();
				a.RetVal = true;
			};

			return true;
		}

		public static void Start()
		{
			// start dgle
			if (Core != null)
				Core.StartEngine();

			// start gtk
			Application.Run();
		}

		public static void Stop()
		{
			// stop dgle
			if (Core != null)
				Core.QuitEngine();
			Engine.FreeEngine();
			
			// stop gtk
			Application.Quit();
		}

		public static void Main(string[] args) {
			if (Init(args))
				Start();
		}
	}
}
