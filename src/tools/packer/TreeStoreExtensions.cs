/**
\author		Shestakov Mikhail aka MIKE
\date		15.12.2012 (c)Andrey Korotkov

This file is a part of DGLE project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE.h" for more details.
*/
using System;
using System.Collections.Generic;
using System.Linq;
using Gtk;

namespace Packer
{
	public static class TreeStoreExtensions
	{
		public static IEnumerable<TreeIter> GetFullTree(this TreeStore store)
		{
			TreeIter iter;
			if (store.GetIterFirst(out iter))
				do
				{
					foreach(TreeIter child in store.GetTree(iter))
						yield return child;
				}
				while (store.IterNext(ref iter));
		}
		
		public static IEnumerable<TreeIter> GetTree(this TreeStore store, TreeIter parent)
		{
			yield return parent;
			foreach(TreeIter iter in store.GetAllChilds(parent))
				yield return iter;
		}
		
		public static IEnumerable<TreeIter> GetAllChilds(this TreeStore store, TreeIter parent)
		{
			if (store.IterHasChild(parent))
			{
				TreeIter child;
				if (store.IterChildren(out child, parent)) 
					do
					{
						yield return child;
						foreach(TreeIter iter in store.GetAllChilds(child))
							yield return iter;
					}
					while (store.IterNext(ref child));
			}
			yield break;
		}
		
		public static IEnumerable<TreeIter> GetChilds(this TreeStore store, TreeIter parent)
		{
			if (store.IterHasChild(parent))
			{
				TreeIter child;
				if (store.IterChildren(out child, parent))
					do
					{
						yield return child;
					}
					while (store.IterNext(ref child));
			}
			yield break;
		}
		
		public static IEnumerable<TreeIter> GetTopIters(this TreeStore store)
		{
			TreeIter iter;
			if (store.GetIterFirst(out iter))
				do
				{
					yield return iter;
				}
				while (store.IterNext(ref iter));
		}
	}
}

