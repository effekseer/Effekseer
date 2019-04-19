using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI.Component
{
	partial class NodeTreeView : System.Windows.Forms.TreeView
	{
		public NodeTreeView()
		{
			InitializeComponent();

			Reading = false;
			Writing = false;

			HandleCreated += new EventHandler(NodeTreeView_HandleCreated);
			HandleDestroyed += new EventHandler(NodeTreeView_HandleDestroyed);
			timer.Start();
		}

		List<SetValueEvent> setValueEvents = new List<SetValueEvent>();

		/// <summary>
		/// 他のクラスからデータ読み込み中
		/// </summary>
		public bool Reading
		{
			get;
			private set;
		}

		/// <summary>
		/// 他のクラスにデータ書き込み中
		/// </summary>
		public bool Writing
		{
			get;
			private set;
		}

		/// <summary>
		/// 全再表示
		/// </summary>
		public void Renew()
		{
			Reading = true;

			ReadNodes();
			ReadSelect();

			Reading = false;
		}

		/// <summary>
		/// 他クラスからノードを取得し設定
		/// </summary>
		/// <remarks>
		/// Readingがtrueである必要あり
		/// </remarks>
		void ReadNodes()
		{
			if (!Reading) throw new Exception();

			// Root再設定
			if (Nodes.Count != 1 || ((NodeTreeNode)Nodes[0]).Node != Core.Root)
			{
				Nodes.Clear();

				Nodes.Add(new NodeTreeNode(Core.Root));
			}

			Action<TreeNode, Data.NodeBase> set_nodes = null;
			set_nodes = (treenode, node) =>
				{
					var tns = treenode.Nodes;
					var ns = node.Children;

					for (int ind = 0; ind < ns.Count;)
					{
						// 一致
						if (ind < tns.Count && ((NodeTreeNode)tns[ind]).Node == ns[ind])
						{
							ind++;
							continue;
						}
						
						// 最終手段
						while (tns.Count > ind)
						{
							((NodeTreeNode)tns[ind]).RemoveEvent(true);
							tns.RemoveAt(ind);
						}

						while (ns.Count > ind)
						{
							tns.Add(new NodeTreeNode(ns[ind]));
							ind++;
						}
					}

					for (int i = 0; i < ns.Count; i++)
					{
						set_nodes(tns[i], ns[i]);
					}
				};

			set_nodes(Nodes[0], Core.Root);

			ExpandAll();
		}

		/// <summary>
		/// 他クラスから選択中のノードを取得し設定
		/// </summary>
		/// <remarks>
		/// Readingがtrueである必要あり
		/// </remarks>
		void ReadSelect()
		{
			if (!Reading) throw new Exception();

			Func<Data.NodeBase, TreeNodeCollection, TreeNode> search_node = null;
			search_node = (searched_node, treenodes) =>
			{
				if (search_node == null) return null;

				for (int i = 0; i < treenodes.Count; i++)
				{
					if (((NodeTreeNode)treenodes[i]).Node == searched_node) return treenodes[i];
					var ret = search_node(searched_node, treenodes[i].Nodes);
					if (ret != null) return ret;
				}
				return null;
			};

			var node = search_node(Core.SelectedNode, Nodes);

			SelectedNode = node;
			ApplyColor();
		}

		/// <summary>
		/// 全再表示イベント
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void OnRenew(object sender, EventArgs e)
		{
			Renew();
		}

		/// <summary>
		/// Coreの選択が変更された時のイベント
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void OnAfterSelect(object sender, EventArgs e)
		{
			if (Writing) return;

			Reading = true;

			ReadSelect();

			Reading = false;
		}

		void NodeTreeView_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterNew += OnRenew;
			Core.OnAfterLoad += OnRenew;
			Core.OnAfterSelectNode += OnAfterSelect;
		}

		void NodeTreeView_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterNew -= OnRenew;
			Core.OnAfterLoad -= OnRenew;
			Core.OnAfterSelectNode -= OnAfterSelect;

			((NodeTreeNode)Nodes[0]).RemoveEvent(true);
		}

		private void NodeTreeView_BeforeSelect(object sender, TreeViewCancelEventArgs e)
		{
			if (SelectedNode != null)
			{
				SelectedNode.BackColor = Color.Transparent;
			}
		}

		private void NodeTreeView_AfterSelect(object sender, TreeViewEventArgs e)
		{
			if (Reading) return;

			Writing = true;

			var node = e.Node as NodeTreeNode;

			if (node != null)
			{
				Core.SelectedNode = node.Node;
			}
			else
			{
				Core.SelectedNode = null;
			}

			ApplyColor();

			Writing = false;
		}

		private void ApplyColor()
		{
			if (SelectedNode != null)
			{
				SelectedNode.BackColor = Color.Gray;
			}
		}

		private void NodeTreeView_AfterCheck(object sender, TreeViewEventArgs e)
		{
			var node = e.Node as NodeTreeNode;

			if (Reading) return;
			
			if (node.Reading) return;

			node.Node.IsRendered.SetValue(node.Checked);

			setValueEvents.RemoveAll(_ => _.Node == node);
			setValueEvents.Add(new SetValueEvent(node));
		}

		private void timer_Tick(object sender, EventArgs e)
		{
			for (int i = 0; i < setValueEvents.Count(); i++)
			{
				setValueEvents[i].Time--;
				if (setValueEvents[i].Time == 0)
				{
					setValueEvents[i].Execute();
				}
			}

			setValueEvents.RemoveAll(_ => _.Time == 0);
		}

		class SetValueEvent
		{
			public int Time;
			public NodeTreeNode Node;

			public SetValueEvent(NodeTreeNode node)
			{
				Node = node;
				Time = 30;
			}

			public void Execute()
			{
				Node.ChangedIsRendered();
			}
		}

		/// <summary>
		/// 専用ノード
		/// </summary>
		class NodeTreeNode : System.Windows.Forms.TreeNode
		{
			public Data.NodeBase Node
			{
				get;
				private set;
			}

			/// <summary>
			/// 他のクラスからデータ読み込み中
			/// </summary>
			public bool Reading
			{
				get;
				private set;
			}

			/// <summary>
			/// 他のクラスにデータ書き込み中
			/// </summary>
			public bool Writing
			{
				get;
				private set;
			}

			public NodeTreeNode(Data.NodeBase node, bool create_children = false)
			{
				if (node == null) throw new Exception();

				Reading = false;
				Writing = false;

				Node = node;
				Text = node.Name;
				ImageIndex = 0;
				SelectedImageIndex = 0;
				Checked = node.IsRendered;
				Node.IsRendered.OnChanged += OnChangedIsRendered;
				Node.Name.OnChanged += OnChangedName;
				Node.OnAfterAddNode += OnAfterAddNode;
				Node.OnAfterRemoveNode += OnAfterRemoveNode;
				Node.OnAfterExchangeNodes += OnAfterExchangeNodes;
				
				if (Node is Data.Node)
				{
					var realNode = (Data.Node)Node;
					int renderType = (int)realNode.DrawingValues.Type.Value;
					ImageIndex = renderType;
					SelectedImageIndex = renderType;

					realNode.DrawingValues.Type.OnChanged += OnChangedRenderType;
				}
				
				if (create_children)
				{
					for (int i = 0; i < Node.Children.Count; i++)
					{
						Nodes.Add(new NodeTreeNode(Node.Children[i], true));
					}
				}
			}

			public void RemoveEvent(bool recursion)
			{
				if (Node is Data.Node)
				{
					var realNode = (Data.Node)Node;
					realNode.DrawingValues.Type.OnChanged -= OnChangedRenderType;
				}

				Node.IsRendered.OnChanged -= OnChangedIsRendered;
				Node.Name.OnChanged -= OnChangedName;
				Node.OnAfterAddNode -= OnAfterAddNode;
				Node.OnAfterRemoveNode -= OnAfterRemoveNode;
				Node.OnAfterExchangeNodes -= OnAfterExchangeNodes;
				if (recursion)
				{
					for (int i = 0; i < Nodes.Count; i++)
					{
						((NodeTreeNode)Nodes[i]).RemoveEvent(true);
					}
				}
			}

			public void ChangedIsRendered()
			{
				Reading = true;

				Checked = Node.IsRendered;

				Reading = false;
			}

			void OnChangedIsRendered(object sender, ChangedValueEventArgs e)
			{
				Reading = true;
				
				Checked = Node.IsRendered;

				Reading = false;
			}

			void OnChangedName(object sender, ChangedValueEventArgs e)
			{
				Reading = true;

				Text = Node.Name;

				Reading = false;
			}
			
			void OnChangedRenderType(object sender, ChangedValueEventArgs e)
			{
				Reading = true;

				if (Node is Data.Node)
				{
					var realNode = (Data.Node)Node;
					int renderType = (int)realNode.DrawingValues.Type.Value;
					ImageIndex = renderType;
					SelectedImageIndex = renderType;
				}

				Reading = false;
			}

			void OnAfterAddNode(object sender, ChangedValueEventArgs e)
			{
				var node = e.Value as Data.NodeBase;

				int ind = 0;
				for (; ind < Node.Children.Count; ind++)
				{
					if (node == Node.Children[ind]) break;
				}

				if (ind == Node.Children.Count)
				{
					Nodes.Add(new NodeTreeNode(node, true));
				}
				else
				{
					Nodes.Insert(ind, new NodeTreeNode(node, true));
				}

				ExpandAll();
			}

			void OnAfterRemoveNode(object sender, ChangedValueEventArgs e)
			{
				var node = e.Value as Data.NodeBase;

				for (int i = 0; i < Nodes.Count; i++)
				{
					var treenode = Nodes[i] as NodeTreeNode;
					if (treenode.Node == node)
					{
						treenode.RemoveEvent(true);
						Nodes.Remove(treenode);
						return;
					}
				}

				throw new Exception();
			}

			void OnAfterExchangeNodes(object sender, ChangedValueEventArgs e)
			{
				var node1 = (e.Value as Tuple<Data.Node, Data.Node>).Item1;
				var node2 = (e.Value as Tuple<Data.Node, Data.Node>).Item2;

				int ind1 = 0;
				int ind2 = 0;

				for (int i = 0; i < Nodes.Count; i++)
				{
					var treenode = Nodes[i] as NodeTreeNode;
					if (treenode.Node == node1)
					{
						ind1 = i;
					}
					if (treenode.Node == node2)
					{
						ind2 = i;
					}
				}

				if (ind1 > ind2)
				{
					var ind_temp = ind1;
					var node_temp = node1;
					ind1 = ind2;
					node1 = node2;
					ind2 = ind_temp;
					node2 = node_temp;
				}

				Nodes.Insert(ind2, new NodeTreeNode(node1, true));
				Nodes.RemoveAt(ind2 + 1);

				Nodes.Insert(ind1, new NodeTreeNode(node2, true));
				Nodes.RemoveAt(ind1 + 1);

				ExpandAll();
			}
		}
	}
}
