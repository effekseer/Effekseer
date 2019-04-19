using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	public partial class DockNodeTreeView : DockContent
	{
		public DockNodeTreeView()
		{
			InitializeComponent();

			nodeTreeView.Renew();

			// Add icon for each nodes
			imageList1.Images.Add(Properties.Resources.NodeEmpty);
			imageList1.Images.Add(Properties.Resources.NodeEmpty);	// Dummy
			imageList1.Images.Add(Properties.Resources.NodeSprite);
			imageList1.Images.Add(Properties.Resources.NodeRibbon);
			imageList1.Images.Add(Properties.Resources.NodeRing);
			imageList1.Images.Add(Properties.Resources.NodeModel);
			imageList1.Images.Add(Properties.Resources.NodeTrack);

            Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconNodeTree).GetHicon());
        }

		internal Component.NodeTreeView NodeTreeView
		{
			get
			{
				return nodeTreeView;
			}
		}

		private void DockNodeTreeView_Load(object sender, EventArgs e)
		{
			Func<Func<bool>, ToolStripMenuItem> create_menu_item_from_commands = (a) =>
			{
				var item = new ToolStripMenuItem();
				var attributes = a.Method.GetCustomAttributes(false);
				var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
				item.Text = NameAttribute.GetName(attributes);
				item.ShowShortcutKeys = true;
				item.ShortcutKeyDisplayString = Shortcuts.GetShortcutText(uniquename);
				item.Click += (object _sender, EventArgs _e) =>
				{
					a();
				};

				return item;
			};

			contextMenuStrip.SuspendLayout();
			var items = contextMenuStrip.Items;

			items.Add(create_menu_item_from_commands(Commands.AddNode));
			items.Add(create_menu_item_from_commands(Commands.InsertNode));
			items.Add(create_menu_item_from_commands(Commands.RemoveNode));

			items.Add(new ToolStripSeparator());

			items.Add(create_menu_item_from_commands(Commands.Copy));
			items.Add(create_menu_item_from_commands(Commands.Paste));
			items.Add(create_menu_item_from_commands(Commands.PasteInfo));

			items.Add(new ToolStripSeparator());

			items.Add(create_menu_item_from_commands(Commands.Undo));
			items.Add(create_menu_item_from_commands(Commands.Redo));
			
			contextMenuStrip.ResumeLayout();
			contextMenuStrip.PerformLayout();
		}

		private void btn_up_Click(object sender, EventArgs e)
		{
			if (Core.SelectedNode == null) return;
			if (Core.SelectedNode.Parent == null) return;
			var s_node = Core.SelectedNode;
			
			int ind = -1;
			for (int i = 0; i < s_node.Parent.Children.Count; i++)
			{
				if (s_node.Parent.Children[i] == s_node)
				{
					ind = i;
					break;
				}
			}
			if (ind != -1 && ind > 0)
			{
				if (s_node.Parent.ExchangeChildren(s_node.Parent.Children[ind - 1], s_node.Parent.Children[ind]))
				{
					Core.SelectedNode = s_node;
				}
			}
		}

		private void btn_down_Click(object sender, EventArgs e)
		{
			if (Core.SelectedNode == null) return;
			if (Core.SelectedNode.Parent == null) return;
			var s_node = Core.SelectedNode;
			
			int ind = -1;
			for (int i = 0; i < s_node.Parent.Children.Count; i++)
			{
				if (s_node.Parent.Children[i] == s_node)
				{
					ind = i;
					break;
				}
			}
			if (ind != -1 && ind -1 < s_node.Parent.Children.Count)
			{
				if (s_node.Parent.ExchangeChildren(s_node.Parent.Children[ind], s_node.Parent.Children[ind + 1]))
				{
					Core.SelectedNode = s_node;
				}
			}
		}

		private void DockNodeTreeView_FormClosed(object sender, FormClosedEventArgs e)
		{

		}
	}
}
