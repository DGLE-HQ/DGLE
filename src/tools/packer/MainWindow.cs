/**
\author		Shestakov Mikhail aka MIKE
\date		26.11.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Linq;
using System.Threading;
using Gtk;
using Gui;

namespace Packer
{
	public partial class MainWindow: Gui.CustomWindow
	{
		private Packer.VirtualFileSystem fileSystem;
		private TreeStore packStore;
		private TreeIter currentFolder = TreeIter.Zero;
		private ListStore folderStore;
		private readonly Gdk.Pixbuf folderIcon = 
			Gdk.PixbufLoader.LoadFromResource("Packer.resources.folder.png").Pixbuf;
		private readonly Gdk.Pixbuf fileIcon = 
			Gdk.PixbufLoader.LoadFromResource("Packer.resources.page_white.png").Pixbuf;

		private WaitWindow waitWindow;

		public MainWindow(): base (WindowType.Toplevel)
		{
			Build();

			InitPackTreeView();
			InitActions();
			InitKeyControl();
			SetDestDnD();
			
			ChangePackActionSensitive(false);
		}

		private void InitPackTreeView()
		{
			packStore = new TreeStore(typeof(Packer.Item));

			folderStore = new ListStore(typeof(Gdk.Pixbuf), typeof(string), typeof(Packer.Item));
			packTreeView.Model = folderStore;
			packTreeView.HeadersVisible = false;
			packTreeView.Selection.Mode = SelectionMode.Multiple;
			packTreeView.EnableSearch = true;
			packTreeView.SearchColumn = 1;
			
			packTreeView.AppendColumn("Icon", new CellRendererPixbuf(), "pixbuf", 0);
			packTreeView.AppendColumn("Name", new CellRendererText(), "text", 1);

			folderStore.SetSortFunc(0, (model, a, b) =>
			{
				Packer.Item itemA = model.GetValue(a, 2) as Packer.Item;
				Packer.Item itemB = model.GetValue(b, 2) as Packer.Item;

				var ItemCost = new Func<Packer.Item, int>(item =>
				{
					return (null == item ? 0 : (item.IsFolder ? (item.IsRoot ? 3 : 2) : 1));
				});

				if (ItemCost(itemA) > ItemCost(itemB))
					return -1;
				else if (ItemCost(itemA) < ItemCost(itemB))
					return 1;
				else if (null == itemA && null == itemB)
					return 0;
				else
					return itemA.Name.CompareTo(itemB.Name);
			});
			folderStore.SetSortColumnId(0, SortType.Ascending);
		}

		private void InitActions()
		{
			// show about
			AboutAction.Activated += (sender, e) => 
				new AboutWindow(this, About.Authors, About.Lines, About.Size).Show();

			// show supported extensions
			SupportedFormatsAction.Activated += (sender, e) =>
				new SupportedFormatsWindow(this).Show();

			// create new pack
			MenuItem newMenuItem = this.UIManager.GetWidget(@"/mainMenuBar/FileAction/NewAction") as MenuItem;
			Menu newSubMenu = new Menu();
			newMenuItem.Submenu = newSubMenu;

			Packer.SupportedExtensions.ToList().ForEach(ext =>
			{
				Gtk.Action action = new Gtk.Action(String.Format("New{0}Action", ext), ext);
				action.Activated+= (sender, e) =>
				{
					if (fileSystem != null)
						fileSystem.Close();
					fileSystem = null;

					if (Packer.Create(ext, out fileSystem))
					{
						fileSystem.New();
						ChangePackActionSensitive(true);
					}
				};
				MenuItem menuItem = action.CreateMenuItem() as MenuItem;
				menuItem.AddAccelerator(
					"activate", 
					this.UIManager.AccelGroup, 
					new AccelKey((Gdk.Key) Gdk.Key.Parse(typeof(Gdk.Key), ext[0].ToString().ToLower()),
							 Gdk.ModifierType.ControlMask, AccelFlags.Visible));

				newSubMenu.Append(menuItem);
			});

			// open pack action
			OpenAction.Activated += (sender, e) => HandleOpenAction();

			// save current pack
			SaveAction.Activated += (sender, e) => HandleSaveAction();

			// close pack and exit
			CloseAction.Activated += (sender, e) =>
			{
				if (fileSystem != null)
					fileSystem.Close();
				fileSystem = null;
				ChangePackActionSensitive(false);
				currentFolder = TreeIter.Zero;
				folderStore.Clear();
				packStore.Clear();
			};

			// exit app
			ExitAction.Activated += (sender, e) => Program.Stop();

			// add files to current pack
			AddFilesAction.Activated += (sender, e) => HandleAddFilesAction();

			// add folder to current pack
			AddFolderAction.Activated += (sender, e) => HandleAddFolderAction();

			// remove items from current pack
			RemoveAction.Activated += (sender, e) => HandleRemoveAction();

			// extract items from current pack
			ExtractAction.Activated += (sender, e) => HandleExtractAction();

			// extract all items from current pack
			ExtractAllAction.Activated += (sender, e) => HandleExtractAllAction();
		}

		private void InitKeyControl()
		{
			var UpFolderAction = new Action<TreeIter>(iter =>
			{
				if (TreeIter.Zero.Equals(iter))
					return;

				TreeIter parent;
				packStore.IterParent(out parent, iter);
				RefreshFolderView(parent);
				currentFolder = parent;
				
				Packer.Item item = packStore.GetValue(iter, 0) as Packer.Item;
				TreeIter selectedInFolder = FindInCurrentFolder(item);
				packTreeView.SelectAndFocus(selectedInFolder);
			});

			var DownFolderAction = new Action<TreeIter>(iter =>
			{
				if (TreeIter.Zero.Equals(iter))
					return;

				RefreshFolderView(iter);
				currentFolder = iter;

				TreeIter selectedInFolder;
				folderStore.GetIterFirst(out selectedInFolder);
				packTreeView.SelectAndFocus(selectedInFolder);
			});

			packTreeView.RowActivated += (o, args) =>
			{	
				TreeIter selectedInFolder;
				folderStore.GetIter(out selectedInFolder, args.Path);
				Packer.Item item = folderStore.GetValue(selectedInFolder, 2) as Packer.Item;
				if (!item.IsFolder)
					return;
				
				if (item.IsRoot)
					UpFolderAction(currentFolder);
				else
					DownFolderAction(FindInPack(item));
			};

			packTreeView.AddEvents((int) (Gdk.EventMask.KeyPressMask));

			packTreeView.KeyReleaseEvent += (o, args) =>
			{
				if (args.Event.Key == Gdk.Key.BackSpace)
					UpFolderAction(currentFolder);

				if (args.Event.Key == Gdk.Key.Delete)
					RemoveAction.Activate();
			};
		}

		private void SetDestDnD()
		{
			Drag.DestSet(
				packTreeView, 
				DestDefaults.All,
				new TargetEntry[] { new TargetEntry("text/uri-list", TargetFlags.OtherApp, 0) },
				Gdk.DragAction.Copy);

			packTreeView.DragDataReceived += (o, args) =>
			{
				string data = Encoding.UTF8.GetString(args.SelectionData.Data, 0, args.SelectionData.Length - 1);
				data = Uri.UnescapeDataString(data);
				Drag.Finish(args.Context, true, false, args.Time);

				string[] files = data.Split("\r\n".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);

				// try to open pack
				if (fileSystem == null)
				{
					if (files.Length == 1)
						OpenPack(files.Select(
							path => path.StartsWith("file:///") ? path.Substring(8) : path).First());

					return;
				}

				packTreeView.Selection.UnselectAll();

				files.Select(path => path.StartsWith("file:///") ? path.Substring(8) : path).ToList()
					.ForEach(path =>
					{
						if (Directory.Exists(path))
						{
							Packer.Item folderItem = NewItem(currentFolder, path);
							TreeIter folderIter = AppendItem(currentFolder, folderItem);
							AppendFolders(folderIter, path);
						}
						else
							if (File.Exists(path))
								AppendFile(currentFolder, path);
					});
			};
		}

		private void UnsetDestDnd()
		{
			Drag.DestUnset(packTreeView);
		}

		private void ChangePackActionSensitive(bool isPackOpen)
		{
			SaveAction.Sensitive = isPackOpen;
			CloseAction.Sensitive = isPackOpen;
			
			AddFilesAction.Sensitive = isPackOpen;
			AddFolderAction.Sensitive = isPackOpen;
			RemoveAction.Sensitive = isPackOpen;
			ExtractAction.Sensitive = isPackOpen;
			ExtractAllAction.Sensitive = isPackOpen;
		}

		#region Open pack action
		private void HandleOpenAction()
		{
			CustomFileChooserDialog dlg = 
				new CustomFileChooserDialog(this, "Open pack", FileChooserAction.Open);
			dlg.FileChooser.SelectMultiple = false;

			Packer.SupportedExtensions.ToList().ForEach(ext =>
			{
				using (Gtk.FileFilter filter = new Gtk.FileFilter())
				{
					filter.Name = ext.ToUpper();
					filter.AddPattern("*." + ext);
					dlg.FileChooser.AddFilter(filter);
				}
			});
			
			dlg.Ok += (sender, e) =>
			{
				if (fileSystem != null)
					fileSystem.Close();
				fileSystem = null;

				OpenPack(dlg.FileChooser.Filename);
				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}

		private void OpenPack(string filename)
		{
			if (Packer.Create(System.IO.Path.GetExtension(filename), out fileSystem))
			{
				
				waitWindow = new WaitWindow(this, "Opening", filename);
				waitWindow.Show();
				
				ThreadNotify done = new ThreadNotify(new ReadyEvent(() =>
				{
					waitWindow.Destroy();
					waitWindow = null;
				}));
				
				ThreadNotify error = new ThreadNotify(new ReadyEvent(() =>
				{
					waitWindow.Destroy();
					waitWindow = null;
					
					CustomMessageDialog msg = 
						new CustomMessageDialog(this, MessageType.Error, fileSystem.LastError);
					msg.Run();
					msg.Destroy();
				}));
				
				new Thread(new ThreadStart(() =>
				{
					if (fileSystem.Open(filename))
					{
						RebuildPackTree(fileSystem.ListFiles());
						ChangePackActionSensitive(true);
						done.WakeupMain();
					}
					else
						error.WakeupMain();
				})).Start();
			}
		}

		private void RebuildPackTree(string[] filenames)
		{
			packStore.Clear();
			folderStore.Clear();
			currentFolder = TreeIter.Zero;

			filenames.Where(filename => filename != null && filename.Length > 0).ToList()
				.ForEach(filename =>
				{
					Packer.Item fileItem = new Packer.Item();
					fileItem.Name = System.IO.Path.GetFileName(filename);
					fileItem.Directory = System.IO.Path.GetDirectoryName(filename) + "\\";

					List<string> folders = fileItem.Directory.Split('\\')
						.Where(folder => folder != null && folder.Length > 0).ToList();

					TreeIter lastFolderIter = TreeIter.Zero;
					Packer.Item lastFolderItem = new Packer.Item();
					lastFolderItem.IsFolder = true;
					folders.ForEach(folder =>
					{
						Packer.Item folderItem = new Packer.Item();
						folderItem.Name = folder;
						folderItem.Directory = lastFolderItem.FullName;
						folderItem.IsFolder = true;

						lastFolderIter = AppendItem(lastFolderIter, folderItem);
						lastFolderItem = folderItem;
					});

					AppendItem(lastFolderIter, fileItem);
				});

			RefreshFolderView(currentFolder);
			TreeIter selected;
			if (folderStore.GetIterFirst(out selected))
				packTreeView.SelectAndFocus(selected);
		}
		#endregion

		#region Save pack action
		private void HandleSaveAction()
		{
			if (fileSystem == null)
				return;

			CustomFileChooserDialog dlg = 
				new CustomFileChooserDialog(this, "Save pack", FileChooserAction.Save);
			dlg.FileChooser.SelectMultiple = false;
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter())
			{
				filter.Name = fileSystem.Extension.ToUpper();
				filter.AddPattern("*." + fileSystem.Extension.ToLower());
				dlg.FileChooser.AddFilter(filter);
			}
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter())
			{
				filter.Name = "All";
				filter.AddPattern("*.*");
				dlg.FileChooser.AddFilter(filter);
			}
			
			dlg.Ok += (sender, e) =>
			{
				string filename = dlg.FileChooser.Filename;

				waitWindow = new WaitWindow(this, "Saving", filename);
				waitWindow.Show();
				
				ThreadNotify done = new ThreadNotify(new ReadyEvent(() =>
				{
					waitWindow.Destroy();
					waitWindow = null;
				}));
				
				ThreadNotify error = new ThreadNotify(new ReadyEvent(() =>
				{
					waitWindow.Destroy();
					waitWindow = null;
					
					CustomMessageDialog msg = 
						new CustomMessageDialog(this, MessageType.Error, fileSystem.LastError);
					msg.Run();
					msg.Destroy();
				}));
				
				new Thread(new ThreadStart(() =>
				{
					if (fileSystem.Save(filename))
						done.WakeupMain();
					else
						error.WakeupMain();
				})).Start();

				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}
		#endregion Save pack action

		#region Add actions
		private void HandleAddFilesAction()
		{
			if (fileSystem == null)
				return;

			CustomFileChooserDialog dlg = 
				new CustomFileChooserDialog(this, "Add files to pack", FileChooserAction.Open);
			dlg.FileChooser.SelectMultiple = true;
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter())
			{
				filter.Name = "All";
				filter.AddPattern("*.*");
				dlg.FileChooser.AddFilter(filter);
			}
			
			dlg.Ok += (sender, e) =>
			{
				packTreeView.Selection.UnselectAll();

				waitWindow = new WaitWindow(this, "Adding", "");
				waitWindow.Show();

				ThreadNotify done = new ThreadNotify(new ReadyEvent(() =>
				{
					dlg.Destroy();
					waitWindow.Destroy();
					waitWindow = null;
				}));

				new Thread(new ThreadStart(() =>
				{
					dlg.FileChooser.Filenames.ToList().ForEach(
						file => AppendFile(currentFolder, file));
					done.WakeupMain();
				})).Start();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}

		private void HandleAddFolderAction()
		{
			if (fileSystem == null)
				return;

			CustomFileChooserDialog dlg = 
				new CustomFileChooserDialog(this, "Add folder to pack", FileChooserAction.SelectFolder);

			dlg.Ok += (sender, e) =>
			{
				packTreeView.Selection.UnselectAll();

				waitWindow = new WaitWindow(this, "Adding", "");
				waitWindow.Show();

				ThreadNotify done = new ThreadNotify(new ReadyEvent(() =>
				{
					waitWindow.Destroy();
					waitWindow = null;
				}));

				new Thread(new ThreadStart(() =>
				{
					string path = dlg.FileChooser.Filename;
					dlg.Destroy();

					Packer.Item folderItem = NewItem(currentFolder, path);
					TreeIter folderIter = AppendItem(currentFolder, folderItem);
					if (!TreeIter.Zero.Equals(folderIter))
						AppendFolders(folderIter, folderItem.HDDPath);
					done.WakeupMain();
				})).Start();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}
		#endregion Add actions

		#region Append item
		private void AppendFolders(TreeIter destIter, string path)
		{
			try
			{
				Directory.EnumerateDirectories(path, "*", SearchOption.TopDirectoryOnly).ToList()
					.ForEach(folder =>
					{
						Packer.Item folderItem = NewItem(destIter, folder);
						TreeIter folderIter = AppendItem(destIter, folderItem);
						AppendFolders(folderIter, folder);
					});

				AppendFiles(destIter, path);
			}
			catch(UnauthorizedAccessException)
			{
				CustomMessageDialog msg = 
					new CustomMessageDialog(this, MessageType.Error, String.Format("Access denied to {0}", path));
				msg.Run();
				msg.Destroy();
			}
		}

		private void AppendFiles(TreeIter destIter, string path)
		{
			try
			{
				Directory.EnumerateFiles(path, "*.*", SearchOption.TopDirectoryOnly).ToList()
					.ForEach(file => AppendFile(destIter, file));
			}
			catch(UnauthorizedAccessException)
			{
				CustomMessageDialog msg = 
					new CustomMessageDialog(this, MessageType.Error, String.Format("Access denied to {0}", path));
				msg.Run();
				msg.Destroy();
			}
		}

		private void AppendFile(TreeIter destIter, string filename)
		{
			Gtk.Application.Invoke((sender, e) => waitWindow.FileText = filename);

			Packer.Item fileItem = NewItem(destIter, filename);
			if (fileSystem.Add(fileItem))
				AppendItem(destIter, fileItem);
			else
			{
				CustomMessageDialog msg = 
					new CustomMessageDialog(this, MessageType.Error, fileSystem.LastError);
				msg.Run();
				msg.Destroy();
			}
		}

		private TreeIter AppendItem(TreeIter parent, Packer.Item item)
		{
			TreeIter existIter = FindInPack(parent, item);
			if (!TreeIter.Zero.Equals(existIter))
				return existIter;
			
			// add item to list store of current folder
			if (parent.Equals(currentFolder))
				packTreeView.SelectAndFocus(
					folderStore.AppendValues(item.IsFolder ? folderIcon : fileIcon, item.Name, item));
			
			// add item to tree store of current pack
			if (TreeIter.Zero.Equals(parent))
				return packStore.AppendValues(item);
			else
				return packStore.AppendValues(parent, item);
		}
		#endregion Append item

		#region Remove action
		private void HandleRemoveAction()
		{
			if (fileSystem == null)
				return;

			List<TreeIter> selectedIters = packTreeView.GetSelected().ToList();

			if (selectedIters.Count == 0)
				return;

			TreePath path = folderStore.GetPath(selectedIters.Last());
			for (int i = 0; i < selectedIters.Count - 1; i++)
				path.Prev();

			selectedIters.ForEach(selectedInFolder =>
			{
				if (TreeIter.Zero.Equals(selectedInFolder))
					return;

				Packer.Item selectedItem = folderStore.GetValue(selectedInFolder, 2) as Packer.Item;
				if (selectedItem.IsRoot)
					return;
				
				TreeIter selectedInPack = FindInPack(selectedItem);
				if (!TreeIter.Zero.Equals(selectedInPack))
				{
					List<TreeIter> iterInPack = packStore.GetTree(selectedInPack).ToList();
					iterInPack.Select(iter => packStore.GetValue(iter, 0) as Packer.Item).ToList()
						.ForEach(item =>
						{
							if (!item.IsFolder)
								fileSystem.Remove(item);
						});

					packStore.Remove(ref selectedInPack);
				}
				folderStore.Remove(ref selectedInFolder);
			});

			while (path.Indices.Last() >= folderStore.IterNChildren() && path.Prev());
			packTreeView.SelectAndFocus(path);
		}
		#endregion Remove action

		#region Extract action
		private void HandleExtractAction()
		{
			if (fileSystem == null)
				return;

			int selectedCount = packTreeView.Selection.CountSelectedRows();

			if (selectedCount == 0)
				return;

			List<TreeIter> selectedInFolder = packTreeView.GetSelected().ToList();
			Packer.Item firstItem = folderStore.GetValue(selectedInFolder.First(), 2) as Packer.Item;

			if (selectedCount == 1 && !firstItem.IsFolder)
			{

				TreeIter iterInPack = FindInPack(firstItem);

				CustomFileChooserDialog dlg = new CustomFileChooserDialog(this, "Extract file from pack", FileChooserAction.Save);
				dlg.FileChooser.CurrentName = firstItem.Name;

				using (Gtk.FileFilter filter = new Gtk.FileFilter())
				{
					filter.Name = "All";
					filter.AddPattern("*.*");
					dlg.FileChooser.AddFilter(filter);
				}

				dlg.Ok += (sender, e) =>
				{
					ExtractFile(iterInPack, dlg.FileChooser.Filename);
					dlg.Destroy();
				};
				dlg.Cancel += (sender, e) => dlg.Destroy();
				dlg.Show();

			}
			else
			{
				CustomFileChooserDialog dlg = new CustomFileChooserDialog(
					this, "Extract from pack", FileChooserAction.SelectFolder);

				dlg.Ok += (sender, e) =>
				{

					selectedInFolder.ForEach(iter =>
					{
						Packer.Item item = folderStore.GetValue(iter, 2) as Packer.Item;
						TreeIter iterInPack = FindInPack(item);

						if (item.IsFolder)
							ExtractFolder(iterInPack, dlg.FileChooser.Filename);
						else
							ExtractFile(iterInPack, dlg.FileChooser.Filename + 
										System.IO.Path.DirectorySeparatorChar + item.Name);
					});

					dlg.Destroy();
				};
				dlg.Cancel += (sender, e) => dlg.Destroy();
				dlg.Show();
			}
		}

		private void HandleExtractAllAction()
		{
			if (fileSystem == null)
				return;

			CustomFileChooserDialog dlg = new CustomFileChooserDialog(
				this, "Extract pack", FileChooserAction.SelectFolder);
			
			dlg.Ok += (sender, e) =>
			{

				packStore.GetTopIters().ToList().ForEach(iterInPack =>
				{
					Packer.Item item = packStore.GetValue(iterInPack, 0) as Packer.Item;
					
					if (item.IsFolder)
						ExtractFolder(iterInPack, dlg.FileChooser.Filename);
					else
						ExtractFile(iterInPack, dlg.FileChooser.Filename + 
									System.IO.Path.DirectorySeparatorChar + item.Name);
				});
				
				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}

		private void ExtractFile(TreeIter iter, string filename)
		{
			waitWindow = new WaitWindow(this, "Extracting", filename);
			waitWindow.Show();

			ThreadNotify done = new ThreadNotify(new ReadyEvent(() =>
			{
				waitWindow.Destroy();
				waitWindow = null;
			}));
			
			ThreadNotify error = new ThreadNotify(new ReadyEvent(() =>
			{
				waitWindow.Destroy();
				waitWindow = null;
				
				CustomMessageDialog msg = 
					new CustomMessageDialog(this, MessageType.Error, fileSystem.LastError);
				msg.Run();
				msg.Destroy();
			}));

			Packer.Item item = packStore.GetValue(iter, 0) as Packer.Item;
			new Thread(new ThreadStart(() =>
			{
				if (fileSystem.Extract(item, filename))
					done.WakeupMain();
				else
					error.WakeupMain();
			})).Start();
		}

		private void ExtractFolder(TreeIter iter, string path)
		{
			Packer.Item item = packStore.GetValue(iter, 0) as Packer.Item;
			
			if (item.IsRoot)
				return;
			else if (item.IsFolder)
			{
				try
				{
					DirectoryInfo info = Directory.CreateDirectory(
						path + System.IO.Path.DirectorySeparatorChar + item.Name);
					packStore.GetChilds(iter).ToList().ForEach(child =>
						ExtractFolder(child, info.FullName));
				}
				catch
				{
				}
			}
			else
				ExtractFile(iter, path + System.IO.Path.DirectorySeparatorChar + item.Name);
		}
		#endregion Extract action


		private void RefreshFolderView(TreeIter parent)
		{
			folderStore.Clear();

			TreeIter child;
			if (TreeIter.Zero.Equals(parent))
				packStore.GetIterFirst(out child);
			else
			{
				packStore.IterChildren(out child, parent);

				Packer.Item root = new Packer.Item(true);
				folderStore.AppendValues(root.IsFolder ? folderIcon : fileIcon, root.Name, root);
			}

			if (!TreeIter.Zero.Equals(child))
				do
				{
					Packer.Item item = packStore.GetValue(child, 0) as Packer.Item;
					folderStore.AppendValues(item.IsFolder ? folderIcon : fileIcon, item.Name, item);
				}
				while (packStore.IterNext(ref child));
		}

		private TreeIter FindInCurrentFolder(Packer.Item item)
		{
			// find iter in list store of current folder
			TreeIter foundIter = TreeIter.Zero;
			folderStore.Foreach((model, path, iter) =>
			{
				Packer.Item foundItem = folderStore.GetValue(iter, 2) as Packer.Item;
				if (foundItem.FullName.CompareTo(item.FullName) == 0)
				{
					foundIter = iter;
					return true;
				}
				return false;
			});

			return foundIter;
		}

		private TreeIter FindInPack(Packer.Item item)
		{
			// find iter in tree store of current pack
			TreeIter foundIter = TreeIter.Zero;
			packStore.Foreach((model, path, iter) =>
			{
				Packer.Item foundItem = packStore.GetValue(iter, 0) as Packer.Item;
				if (item.FullName.CompareTo(foundItem.FullName) == 0)
				{
					foundIter = iter;
					return true;
				}
				return false;
			});
			return foundIter;
		}

		private TreeIter FindInPack(TreeIter parent, Packer.Item item)
		{
			List<TreeIter> childIters = null;
			if (TreeIter.Zero.Equals(parent))
				childIters = packStore.GetTopIters().ToList();
			else
				childIters = packStore.GetChilds(parent).ToList();

			var foundIters = childIters.Where(iter =>
			{
				Packer.Item foundItem = packStore.GetValue(iter, 0) as Packer.Item;
				if (item.FullName.CompareTo(foundItem.FullName) == 0)
					return true;
				return false;
			});

			if (foundIters.Any())
				return foundIters.First();
			else
				return TreeIter.Zero;
		}

		private Packer.Item NewItem(TreeIter parent, string path)
		{
			Packer.Item item = new Packer.Item(path);
			if (!TreeIter.Zero.Equals(parent))
			{
				Packer.Item parentItem = packStore.GetValue(parent, 0) as Packer.Item;
				item.Directory = parentItem.FullName;
			}
			return item;
		}
	}
}