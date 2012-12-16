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
using Gtk;
using Gui;

namespace DCPPacker {

	public partial class MainWindow: Gui.CustomWindow
	{
		private DCP dcp;
		private TreeStore packStore;
		private TreeIter currentFolder = TreeIter.Zero;
		private ListStore folderStore;

		private readonly Gdk.Pixbuf folderIcon = 
			Gdk.PixbufLoader.LoadFromResource("DCPPacker.resources.folder.png").Pixbuf;
		private readonly Gdk.Pixbuf fileIcon = 
			Gdk.PixbufLoader.LoadFromResource("DCPPacker.resources.page_white.png").Pixbuf;

		public MainWindow(): base (WindowType.Toplevel)
		{
			Build();

			if (InitDCP()) {
				InitPackTreeView();
				InitActions();
				InitKeyControl();
				InitDnD();
				
				ChangePackActionSensitive(false);
				NewAction.Activate();
			}
		}

		private bool InitDCP()
		{
			// init dcp file system from dgle
			try {
				dcp = new DCP();

				return true;
			} catch {
				CustomMessageDialog dlg = new CustomMessageDialog(
					this, MessageType.Error, ButtonsType.Ok, 
					"Failed to load DCP file system from DGLE!");
				dlg.Ok += (sender, e) => Program.Stop();
				dlg.Show();

				return false;
			}
		}

		private void InitPackTreeView()
		{
			packStore = new TreeStore(typeof(DCP.Item));

			folderStore = new ListStore(typeof(Gdk.Pixbuf), typeof(string), typeof(DCP.Item));
			packTreeView.Model = folderStore;
			packTreeView.HeadersVisible = false;
			packTreeView.Selection.Mode = SelectionMode.Multiple;
			packTreeView.EnableSearch = true;
			packTreeView.SearchColumn = 1;
			
			packTreeView.AppendColumn("Icon", new CellRendererPixbuf(), "pixbuf", 0);
			packTreeView.AppendColumn("Name", new CellRendererText(), "text", 1);

			folderStore.SetSortFunc(0, (model, a, b) => {
				DCP.Item itemA = model.GetValue(a, 2) as DCP.Item;
				DCP.Item itemB = model.GetValue(b, 2) as DCP.Item;

				var ItemCost = new Func<DCP.Item, int>((item) => {
					return (null == item ? 0 : (item.IsFolder ? (item.IsRoot ? 3 : 2) : 1));
				});

				if (ItemCost(itemA) > ItemCost(itemB))
					return -1;
				else if (ItemCost(itemA) < ItemCost(itemB))
					return 1;
				else if (null == itemA && null == itemB)
					return 0;
				else {
					return itemA.Name.CompareTo(itemB.Name);
				}
			});
			folderStore.SetSortColumnId(0, SortType.Ascending);
		}

		private void InitActions()
		{
			// show about
			AboutAction.Activated += (sender, e) => 
				new AboutWindow (this, About.Authors, About.Lines, About.Size).Show();

			// create new pack
			NewAction.Activated += (sender, e) => {
				dcp.New();
				ChangePackActionSensitive(true);
			};

			// open pack action
			OpenAction.Activated += (sender, e) => HandleOpenAction();

			// save current pack
			SaveAction.Activated += (sender, e) => HandleSaveAction();

			// close pack and exit
			CloseAction.Activated += (sender, e) => {
				dcp.Close();
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
			var UpFolderAction = new Action<TreeIter>((iter) => {
				if (TreeIter.Zero.Equals(iter))
					return;

				TreeIter parent;
				packStore.IterParent(out parent, iter);
				RefreshFolderView(parent);
				currentFolder = parent;
				
				DCP.Item item = packStore.GetValue(iter, 0) as DCP.Item;
				TreeIter selectedInFolder = FindInCurrentFolder(item);
				packTreeView.SelectAndFocus(selectedInFolder);
			});

			var DownFolderAction = new Action<TreeIter>((iter) => {
				if (TreeIter.Zero.Equals(iter))
					return;

				RefreshFolderView(iter);
				currentFolder = iter;

				TreeIter selectedInFolder;
				folderStore.GetIterFirst(out selectedInFolder);
				packTreeView.SelectAndFocus(selectedInFolder);
			});

			packTreeView.RowActivated += (o, args) => {
				
				TreeIter selectedInFolder;
				folderStore.GetIter(out selectedInFolder, args.Path);
				DCP.Item item = folderStore.GetValue(selectedInFolder, 2) as DCP.Item;
				if (!item.IsFolder)
					return;
				
				if (item.IsRoot) {
					UpFolderAction(currentFolder);
				} else {
					DownFolderAction(FindInPack(item));
				}
			};

			packTreeView.AddEvents((int) (Gdk.EventMask.KeyPressMask));

			packTreeView.KeyReleaseEvent += (o, args) => {
				if (args.Event.Key == Gdk.Key.BackSpace)
					UpFolderAction(currentFolder);

				if (args.Event.Key == Gdk.Key.Delete)
					RemoveAction.Activate();
			};
		}

		private void InitDnD()
		{
			Drag.DestSet(
				packTreeView, 
				DestDefaults.All,
				new TargetEntry[] {
					new TargetEntry("text/uri-list", TargetFlags.OtherApp, 0)
				},
				Gdk.DragAction.Copy
			);

			packTreeView.DragDataReceived += (o, args) => {
				string data = Encoding.UTF8.GetString(args.SelectionData.Data);
				Drag.Finish(args.Context, true, false, args.Time);

				data.Split("\r\n".ToCharArray())
					.Where((path) => null != path && path.Length > 0).ToList()
						.Select((path) => path.StartsWith("file:///") ? path.Substring(8) : path).ToList()
						.ForEach((path) => {
							if (Directory.Exists(path)) {
								DCP.Item folderItem = NewItem(currentFolder, path);
								TreeIter folderIter = AppendItem(currentFolder, folderItem);
								AppendFolders(folderIter, path);
							}
							else if (File.Exists(path)) {
								DCP.Item fileItem = NewItem(currentFolder, path);
								if (dcp.Add(fileItem))
									AppendItem(currentFolder, fileItem);
							}
						});
			};
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
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter()) {
				filter.Name = "DCP";
				filter.AddPattern("*.dcp");
				dlg.FileChooser.AddFilter(filter);
			}
			
			dlg.Ok += (sender, e) => {
				if (dcp.Open(dlg.FileChooser.Filename)) {
					RebuildPackTree(dcp.ListFiles());
					ChangePackActionSensitive(true);
				}
				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}

		private void RebuildPackTree(string[] filenames)
		{
			packStore.Clear();
			folderStore.Clear();
			currentFolder = TreeIter.Zero;

			filenames
				.Where((filename) => filename != null && filename.Length > 0).ToList()
				.ForEach((filename) => {
						DCP.Item fileItem = new DCP.Item();
						fileItem.Name = System.IO.Path.GetFileName(filename);
						fileItem.Directory = filename.Replace(fileItem.Name, "");

						List<string> folders = fileItem.Directory.Split('\\')
							.Where((folder) => folder != null && folder.Length > 0).ToList();

						TreeIter lastFolderIter = TreeIter.Zero;
						DCP.Item lastFolderItem = new DCP.Item();
						lastFolderItem.IsFolder = true;
						folders.ForEach((folder) => {
							DCP.Item folderItem = new DCP.Item();
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
			CustomFileChooserDialog dlg = 
				new CustomFileChooserDialog(this, "Save pack", FileChooserAction.Save);
			dlg.FileChooser.SelectMultiple = false;
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter()) {
				filter.Name = "DCP";
				filter.AddPattern("*.dcp");
				dlg.FileChooser.AddFilter(filter);
			}
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter()) {
				filter.Name = "All";
				filter.AddPattern("*.*");
				dlg.FileChooser.AddFilter(filter);
			}
			
			dlg.Ok += (sender, e) => {
				dcp.Save(dlg.FileChooser.Filename);
				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}
		#endregion Save pack action

		#region Add actions
		private void HandleAddFilesAction()
		{
			CustomFileChooserDialog dlg = 
				new CustomFileChooserDialog(this, "Add files to pack", FileChooserAction.Open);
			dlg.FileChooser.SelectMultiple = true;
			
			using (Gtk.FileFilter filter = new Gtk.FileFilter()) {
				filter.Name = "All";
				filter.AddPattern("*.*");
				dlg.FileChooser.AddFilter(filter);
			}
			
			dlg.Ok += (sender, e) => {
				dlg.FileChooser.Filenames.ToList().ForEach(file => {
					DCP.Item fileItem = NewItem(currentFolder, file);
					if (dcp.Add(fileItem))
						AppendItem(currentFolder, fileItem);
				});

				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}

		private void HandleAddFolderAction()
		{
			CustomFileChooserDialog dlg = 
				new CustomFileChooserDialog(this, "Add folder to pack", FileChooserAction.SelectFolder);

			dlg.Ok += (sender, e) => {
				string path = dlg.FileChooser.Filename;

				DCP.Item folderItem = NewItem(currentFolder, path);
				TreeIter folderIter = AppendItem(currentFolder, folderItem);
				if (!TreeIter.Zero.Equals(folderIter))
					AppendFolders(folderIter, folderItem.HDDPath);

				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}
		#endregion Add actions

		#region Append item
		private void AppendFolders(TreeIter destIter, string path)
		{
			Directory.EnumerateDirectories(path, "*", SearchOption.TopDirectoryOnly).ToList()
				.ForEach(folder => {
					DCP.Item folderItem = NewItem(destIter, folder);
					TreeIter folderIter = AppendItem(destIter, folderItem);
					AppendFolders(folderIter, folder);
				});
			
			AppendFiles(destIter, path);
		}
		
		private void AppendFiles(TreeIter destIter, string path) {
			Directory.EnumerateFiles(path, "*.*", SearchOption.TopDirectoryOnly).ToList()
				.ForEach(file => {
					DCP.Item fileItem = NewItem(destIter, file);					
					if (dcp.Add(fileItem))
						AppendItem(destIter, fileItem);
				});
		}
		
		private TreeIter AppendItem(TreeIter parent, DCP.Item item)
		{
			TreeIter existIter = FindInPack(item);
			if (!TreeIter.Zero.Equals(existIter))
				return existIter;
			
			// add item to list store of current folder
			if (parent.Equals(currentFolder)) {
				packTreeView.SelectAndFocus(
					folderStore.AppendValues(item.IsFolder ? folderIcon : fileIcon, item.Name, item)
				);
			}
			
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
			List<TreeIter> selectedIters = packTreeView.GetSelected().ToList();

			if (selectedIters.Count == 0)
				return;

			TreePath path = folderStore.GetPath(selectedIters.Last());
			for(int i = 0; i < selectedIters.Count - 1; i++)
				path.Prev();

			selectedIters.ForEach(selectedInFolder => {
				if (TreeIter.Zero.Equals(selectedInFolder))
					return;

				DCP.Item selectedItem = folderStore.GetValue(selectedInFolder, 2) as DCP.Item;
				if (selectedItem.IsRoot)
					return;
				
				TreeIter selectedInPack = FindInPack(selectedItem);
				if (!TreeIter.Zero.Equals(selectedInPack)) {
					List<TreeIter> iterInPack = packStore.GetTree(selectedInPack).ToList();
					iterInPack.Select(iter => packStore.GetValue(iter, 0) as DCP.Item).ToList()
						.ForEach(item => {
							if (!item.IsFolder)
								dcp.Remove(item);
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
			int selectedCount = packTreeView.Selection.CountSelectedRows();

			if (selectedCount == 0)
				return;

			List<TreeIter> selectedInFolder = packTreeView.GetSelected().ToList();
			DCP.Item firstItem = folderStore.GetValue(selectedInFolder.First(), 2) as DCP.Item;

			if (selectedCount == 1 && !firstItem.IsFolder) {

				TreeIter iterInPack = FindInPack(firstItem);

				CustomFileChooserDialog dlg = new CustomFileChooserDialog(this, "Extract file from pack", FileChooserAction.Save);
				dlg.FileChooser.SetFilename(firstItem.Name);

				using (Gtk.FileFilter filter = new Gtk.FileFilter()) {
					filter.Name = "All";
					filter.AddPattern("*.*");
					dlg.FileChooser.AddFilter(filter);
				}

				dlg.Ok += (sender, e) => {
					ExtractFile(iterInPack, dlg.FileChooser.Filename);
					dlg.Destroy();
				};
				dlg.Cancel += (sender, e) => dlg.Destroy();
				dlg.Show();

			} else {

				CustomFileChooserDialog dlg = new CustomFileChooserDialog(	
					this, "Extract from pack", FileChooserAction.SelectFolder);

				dlg.Ok += (sender, e) => {

					selectedInFolder.ForEach((iter) => {
						DCP.Item item = folderStore.GetValue(iter, 2) as DCP.Item;
						TreeIter iterInPack = FindInPack(item);

						if (item.IsFolder) {
							ExtractFolder(iterInPack, dlg.FileChooser.Filename);
						} else {
							ExtractFile(iterInPack, dlg.FileChooser.Filename + 
							            System.IO.Path.DirectorySeparatorChar + item.Name);
						}
					});

					dlg.Destroy();
				};
				dlg.Cancel += (sender, e) => dlg.Destroy();
				dlg.Show();
			}
		}

		private void HandleExtractAllAction()
		{
			CustomFileChooserDialog dlg = new CustomFileChooserDialog(
				this, "Extract pack", FileChooserAction.SelectFolder);
			
			dlg.Ok += (sender, e) => {

				packStore.GetTopIters().ToList().ForEach((iterInPack) => {
					DCP.Item item = packStore.GetValue(iterInPack, 0) as DCP.Item;
					
					if (item.IsFolder) {
						ExtractFolder(iterInPack, dlg.FileChooser.Filename);
					} else {
						ExtractFile(iterInPack, dlg.FileChooser.Filename + 
						            System.IO.Path.DirectorySeparatorChar + item.Name);
					}
				});
				
				dlg.Destroy();
			};
			dlg.Cancel += (sender, e) => dlg.Destroy();
			dlg.Show();
		}

		private void ExtractFile(TreeIter iter, string filename)
		{
			DCP.Item item = packStore.GetValue(iter, 0) as DCP.Item;
			dcp.Extract(item, filename);
		}
		
		private void ExtractFolder(TreeIter iter, string path)
		{
 			DCP.Item item = packStore.GetValue(iter, 0) as DCP.Item;
			
			if (item.IsRoot)
				return;
			
			else if (item.IsFolder) {
				try {
					DirectoryInfo info = Directory.CreateDirectory(
						path + System.IO.Path.DirectorySeparatorChar + item.Name);
					packStore.GetChilds(iter).ToList().ForEach((child) => {
						ExtractFolder(child, info.FullName);
					});
				} catch {
				}
			} else {
				ExtractFile(iter, path + System.IO.Path.DirectorySeparatorChar + item.Name);
			}
		}
		#endregion Extract action


		private void RefreshFolderView(TreeIter parent)
		{
			folderStore.Clear();

			TreeIter child;
			if (TreeIter.Zero.Equals(parent))
				packStore.GetIterFirst(out child);
			else {
				packStore.IterChildren(out child, parent);

				DCP.Item root = new DCP.Item(true);
				folderStore.AppendValues(root.IsFolder ? folderIcon : fileIcon, root.Name, root);
			}

			if (!TreeIter.Zero.Equals(child)) {
				do {
					DCP.Item item = packStore.GetValue(child, 0) as DCP.Item;

					folderStore.AppendValues(item.IsFolder ? folderIcon : fileIcon, item.Name, item);
				} while(packStore.IterNext(ref child));
			}


		}

		private TreeIter FindInCurrentFolder(DCP.Item item)
		{
			// find iter in list store of current folder
			TreeIter foundIter = TreeIter.Zero;
			folderStore.Foreach((model, path, iter) => {
				DCP.Item foundItem = folderStore.GetValue(iter, 2) as DCP.Item;
				if (foundItem.FullName.CompareTo(item.FullName) == 0) {
					foundIter = iter;
					return true;
				}
				return false;
			});

			return foundIter;
		}

		private TreeIter FindInPack(DCP.Item item)
		{
			// find iter in tree store of current pack
			TreeIter foundIter = TreeIter.Zero;
			packStore.Foreach((model, path, iter) => {
				DCP.Item foundItem = packStore.GetValue(iter, 0) as DCP.Item;
				if (item.FullName.CompareTo(foundItem.FullName) == 0) {
					foundIter = iter;
					return true;
				}
				return false;
			});
			return foundIter;
		}

		private DCP.Item NewItem(TreeIter parent, string path)
		{
			DCP.Item item = new DCP.Item(path);
			if (!TreeIter.Zero.Equals(parent)) {
				DCP.Item parentItem = packStore.GetValue(parent, 0) as DCP.Item;
				item.Directory = parentItem.FullName;
			}
			return item;
		}
	}
}