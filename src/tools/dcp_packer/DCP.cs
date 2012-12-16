/**
\author		Shestakov Mikhail aka MIKE
\date		8.12.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.IO;
using System.Runtime.InteropServices;
using DGLE;

namespace DCPPacker
{
	public class DCP
	{

		public class Item
		{
			public string Name { get; set; }
			public string Directory { get; set; }
			public string FullName {
				get {
					return Directory + Name + (IsFolder ? @"\" : "");
				}
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
				this.Directory = @"\";
				this.IsFolder = System.IO.Directory.Exists(this.HDDPath);
				this.IsRoot = false;
			}
		}

		public const string FILE_SYSTEM_NAME = "dcp";

		public IFileSystem FileSystem { get; private set; }
		public string LastError { get; private set; }

		public DCP()
		{
			IEngineSubSystem subSystem;
			Program.Core.GetSubSystem(E_ENGINE_SUB_SYSTEM.ESS_FILE_SYSTEM, out subSystem);
			IMainFileSystem mainFileSystem = subSystem as DGLE.IMainFileSystem;
			
			IFileSystem dcpFileSystem;
			mainFileSystem.GetVirtualFileSystem(FILE_SYSTEM_NAME, out dcpFileSystem);
			FileSystem = dcpFileSystem;
		}

		public void New()
		{
			TVariant arg = new TVariant();
			FileSystem.ExecCmdVar("create", ref arg);
			GetLastError(arg);
		}

		public bool Open(string fileName)
		{
			Console.WriteLine("Open " + fileName);
			TVariant arg = new TVariant();
			FileSystem.ExecCmdVar(
				String.Format("{0} {1}", "open", fileName),
				ref arg);
			GetLastError(arg);
			return arg.AsBool();
		}

		public string[] ListFiles()
		{
			Console.WriteLine("List");

			string filenames = MarshalUtils.MarshalString((pnt, length) => { 
				FileSystem.ExecCmdStr("list", pnt, out length);
				return length;
			}, 8192);// dgle doesn't return buffer length with zero pointer

			Console.WriteLine(filenames);
			return filenames.Split(';');
		}

		public bool Save(string fileName)
		{
			if (Path.HasExtension(fileName))
				fileName = Path.ChangeExtension(fileName, ".dcp");
			else
				fileName += ".dcp";
			Console.WriteLine("Save as " + fileName);
			TVariant arg = new TVariant();
			FileSystem.ExecCmdVar(
				String.Format("{0} {1}", "save", fileName),
				ref arg);
			GetLastError(arg);
			return arg.AsBool();
		}

		public void Close()
		{
			TVariant arg = new TVariant();
			FileSystem.ExecCmdVar("close", ref arg);
			GetLastError(arg);
		}

		public bool Add(Item item)
		{
			Console.WriteLine("Add " + item.HDDPath);
			Console.WriteLine("As " + item.FullName);
			TVariant arg = new TVariant();
			FileSystem.ExecCmdVar(
				String.Format("{0} {1};{2}", "add", item.HDDPath, item.Directory),
				ref arg);
			GetLastError(arg);
			return arg.AsBool();
		}

		public bool Remove(Item item)
		{
			Console.WriteLine("Remove " + item.FullName);
			TVariant arg = new TVariant();
			FileSystem.ExecCmdVar(
				String.Format("{0} {1}", "remove", item.FullName),
				ref arg);
			GetLastError(arg);
			return arg.AsBool();
		}

		public bool Extract(Item item, string newFileName)
		{
			Console.WriteLine("Extract " + item.FullName);
			Console.WriteLine("To " + newFileName);
			TVariant arg = new TVariant();
			FileSystem.ExecCmdVar(
				String.Format("{0} {1};{2}", "extract", item.FullName, newFileName),
				ref arg);
			GetLastError(arg);
			return arg.AsBool();
		}

		private void GetLastError(TVariant arg)
		{
			if (arg.AsBool()) {
				LastError = "";
				return;
			}
			LastError = MarshalUtils.MarshalString((pnt, length) => { 
				FileSystem.ExecCmdStr("last_error", pnt, out length);
				return length;
			});
			Console.WriteLine(LastError);
		}
	}
}

