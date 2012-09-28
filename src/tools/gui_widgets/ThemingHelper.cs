/**
\author		Shestakov Mikhail aka MIKE
\date		22.9.2012 (c)Korotkov Andrey

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;
using System.Reflection;
using System.IO;

namespace GuiWidgets
{
	public static class ThemingHelper
	{
		private static Assembly _s_exe_assembly = Assembly.GetExecutingAssembly ();
		public static string default_theme_path = @"./../../../resources/tools/style/gtkrc";
		public static string default_min_btn_path = @".\..\..\..\resources\tools\style\Title\minimize_button_normal.png";
		public static string default_max_btn_path = @".\..\..\..\resources\tools\style\Title\maximize_button_normal.png";
		public static string default_close_btn_path = @".\..\..\..\resources\tools\style\Title\close_button_normal.png";

		public static void LoadTheme ()
		{
			LoadTheme(_s_exe_assembly);
		}

		public static void LoadTheme(Assembly assembly)
		{
			string style_path = @".\style";
			string rc_file_path = "";

			try {
				Directory.Delete(style_path, true);
			} catch {
			}

			assembly = assembly ?? _s_exe_assembly;
			string[] names;
			try {
				names = assembly.GetManifestResourceNames();
			} catch {
				names = new string[0];
			}

			string rel_path = Path.Combine(style_path, @"gtk-2.0");
			Type theType = typeof(ThemingHelper);
			string resource_start_name = 
				String.Format("{0}.style.gtk-2.0.", theType.Namespace);

			foreach(string name in names) {
				if (!name.StartsWith(resource_start_name))
					continue;

				string path = "";
				path = name.Replace(resource_start_name, "");
				if (name.EndsWith("gtkrc")) {
					path = Path.Combine(rel_path, path.Replace(".", @"\"));
					rc_file_path = path;
				} else {
					int last_dot = path.LastIndexOf(".");
					path = Path.Combine(rel_path, path.Substring(0, last_dot).Replace(".", @"\") + path.Substring(last_dot));
				}

				Stream file_stream = null;
				Stream rc_stream = null;
				try {
					Directory.CreateDirectory(Path.GetDirectoryName(path));
					file_stream = System.IO.File.Create(path);
					rc_stream = assembly.GetManifestResourceStream(name);
					CopyStream(rc_stream, file_stream);
				} finally {
					try {
						if (null != rc_stream)
							rc_stream.Close();
					} catch { 
					}
					try {
						if (null != file_stream)
							file_stream.Close();
					} catch { 
					}
				}
			}

			InitTheme (rc_file_path);
		}

		public static void UnloadTheme()
		{
			string style_path = @".\style";


			try {
				Directory.Delete(style_path, true);
			} catch {
			}
		}


		private static void CopyStream(Stream input, Stream output)
		{
			byte[] buffer = new byte[4 * 1024];
			int len;
			while ( (len = input.Read(buffer, 0, buffer.Length)) > 0) {
				output.Write(buffer, 0, len);
			}    
		}

		public static void InitTheme(string rc_file_path)
		{
			if (File.Exists(rc_file_path)) {
				Gtk.Rc.AddDefaultFile(rc_file_path);
				Gtk.Rc.Parse(rc_file_path);
				//Gtk.Settings.Default.ThemeName = rc_file_path;
			} else {
				Console.WriteLine("Faile to load style from " + rc_file_path);
			}
		}

		public static void InitTheme ()
		{
			string wd = Directory.GetCurrentDirectory();
			wd = Path.GetFullPath(wd + default_theme_path);
			InitTheme(wd);
		}

		public static Gtk.Image LoadImageFromFile(string fileName)
		{
			Stream file_stream = null;
			try {
				file_stream = System.IO.File.OpenRead(fileName);
				Gtk.Image img = new Gtk.Image(file_stream);
				img.SetSizeRequest(13, 16);
				return img;
			} catch {
				return null;
			} finally {
				try {
					if (null != file_stream)
						file_stream.Close();
				} catch { 
				}
			}
		}

		public static Gtk.Image LoadImageMinBtn()
		{
			return LoadImageFromFile(default_min_btn_path);
		}

		public static Gtk.Image LoadImageMaxBtn()
		{
			return LoadImageFromFile(default_max_btn_path);
		}

		public static Gtk.Image LoadImageCloseBtn()
		{
			return LoadImageFromFile(default_close_btn_path);
		}
	}
}

