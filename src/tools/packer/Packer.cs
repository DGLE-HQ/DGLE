/**
\author		Shestakov Mikhail aka MIKE
\date		02.02.2013 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using DGLE;

namespace Packer
{
	public static class Packer
	{
		public class Item
		{
			public string Name { get; set; }
			public string Directory { get; set; }
			public string FullName
			{
				get { return Directory + Name + (IsFolder ? @"\" : ""); }
			}
			public bool IsFolder { get; set; }
			public bool IsRoot { get; private set;}
			public string HDDPath { get; set; }
			
			public Item()
			{
				this.IsFolder = false;
				this.IsRoot = false;
			}
			
			public Item(bool isRoot)
			{
				this.Name = "..";
				this.IsFolder = true;
				this.IsRoot = isRoot;
			}
			
			public Item(string hddPath)
			{
				this.HDDPath = hddPath;
				this.Name = Path.GetFileName(this.HDDPath);
				this.Directory = @"";
				this.IsFolder = System.IO.Directory.Exists(this.HDDPath);
				this.IsRoot = false;
			}
		}

		public class VirtualFileSystem
		{
			private IFileSystem fileSystem;
			public string Extension { get; private set; }
			public string Description { get; private set; }
			public string LastError { get; private set; }

			public VirtualFileSystem(IFileSystem fileSystem, string extension, string description)
			{
				this.fileSystem = fileSystem;
				this.Extension = extension;
				this.Description = description;
			}

			public void New()
			{
				Console.WriteLine("Create");
				TVariant arg = TVariant.Zero;
				fileSystem.ExecuteTextCommand("create", ref arg);
				GetLastError(arg);
			}
			
			public bool Open(string filename)
			{
				Console.WriteLine("Open " + filename);
				TVariant arg = TVariant.Zero;
				fileSystem.ExecuteTextCommand(
					String.Format("{0} {1}", "open", filename),
					ref arg);
				GetLastError(arg);
				return (bool) arg;
			}
			
			public string[] ListFiles()
			{
				Console.WriteLine("List");
				
				string filenames = MarshalUtils.MarshalString((pnt, length) => { 
					fileSystem.ExecuteTextCommandEx("list", pnt, out length);
					return length;
				});
				
				Console.WriteLine(filenames);
				return filenames.Split(';');
			}
			
			public bool Save(string filename)
			{
				if (Path.HasExtension(filename))
					filename = Path.ChangeExtension(filename, "." + Extension.ToLower());
				else
					filename += "." + Extension.ToLower();

				Console.WriteLine("Save as " + filename);
				TVariant arg = TVariant.Zero;
				fileSystem.ExecuteTextCommand(
					String.Format("{0} {1}", "save", filename),
					ref arg);
				GetLastError(arg);
				return (bool) arg;
			}
			
			public void Close()
			{
				Console.WriteLine("Close");
				TVariant arg = TVariant.Zero;
				fileSystem.ExecuteTextCommand("close", ref arg);
				GetLastError(arg);
			}
			
			public bool Add(Item item)
			{
				Console.WriteLine("Add " + item.HDDPath);
				Console.WriteLine("As " + item.FullName);
				TVariant arg = TVariant.Zero;
				fileSystem.ExecuteTextCommand(
					String.Format("{0} {1};{2}", "add", item.HDDPath, item.Directory),
					ref arg);
				GetLastError(arg);
				return (bool) arg;
			}
			
			public bool Remove(Item item)
			{
				Console.WriteLine("Remove " + item.FullName);
				TVariant arg = TVariant.Zero;
				fileSystem.ExecuteTextCommand(
					String.Format("{0} {1}", "remove", item.FullName),
					ref arg);
				GetLastError(arg);
				return (bool) arg;
			}
			
			public bool Extract(Item item, string newFilename)
			{
				Console.WriteLine("Extract " + item.FullName);
				Console.WriteLine("To " + newFilename);
				TVariant arg = TVariant.Zero;
				fileSystem.ExecuteTextCommand(
					String.Format("{0} {1};{2}", "extract", item.FullName, newFilename),
					ref arg);
				GetLastError(arg);
				return (bool) arg;
			}
			
			private void GetLastError(TVariant arg)
			{
				if ((bool) arg)
				{
					LastError = "";
					return;
				}
				LastError = MarshalUtils.MarshalString((pnt, length) => { 
					fileSystem.ExecuteTextCommandEx("last_error", pnt, out length);
					return length;
				});
				Console.WriteLine(LastError);
			}
		}

		public static Dictionary<String, VirtualFileSystem> SupportedFileSystems { get; private set; }
		public static string[] SupportedExtensions { get { return SupportedFileSystems.Keys.ToArray(); } }

		public static void Init()
		{
			IEngineSubSystem subSystem;
			Program.Core.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_FILE_SYSTEM, out subSystem);
			IMainFileSystem mainFileSystem = subSystem as DGLE.IMainFileSystem;

			String[] registredFileSystems = MarshalUtils.MarshalString((pnt, length) => {
				mainFileSystem.GetRegisteredVirtualFileSystems(pnt, out length);
				return length;
			}).Split(';').ToList().Where(name => name.Trim().Length > 0).ToArray();

			SupportedFileSystems = new Dictionary<string, VirtualFileSystem>(registredFileSystems.Length);
			registredFileSystems.ToList().ForEach(ext => {
				IFileSystem fileSystem;
				mainFileSystem.GetVirtualFileSystem(ext, out fileSystem);

				string desc = MarshalUtils.MarshalString((pnt, length) => {
					mainFileSystem.GetVirtualFileSystemDescription(ext, pnt, out length);
					return length;
				});

				SupportedFileSystems.Add(ext, new VirtualFileSystem(fileSystem, ext, desc));
			});
		}

		public static bool Create(string extension, out VirtualFileSystem fileSystem)
		{
			return SupportedFileSystems.TryGetValue(
				extension.Replace(".", "").ToUpper(), out fileSystem);
		}
	}
}

