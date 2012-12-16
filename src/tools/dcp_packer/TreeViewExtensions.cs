/**
\author		Shestakov Mikhail aka MIKE
\date		12.12.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Collections.Generic;
using System.Linq;
using Gtk;

namespace DCPPacker
{
	public static class TreeViewExtensions
	{
		public static IEnumerable<TreeIter> GetSelected(this TreeView tree)
		{
			return tree.Selection.GetSelectedRows().ToList()
				.Select(path => {
					TreeIter selectedIter;
					if (tree.Model.GetIter(out selectedIter, path))
						return selectedIter;
					return TreeIter.Zero;
				});
		}

		public static void SelectAndFocus(this TreeView tree, TreeIter iter)
		{
			tree.Selection.SelectIter(iter);
			tree.GrabFocus();
		}

		public static void SelectAndFocus(this TreeView tree, TreePath path)
		{
			tree.Selection.SelectPath(path);
			tree.GrabFocus();
		}
	}
}

