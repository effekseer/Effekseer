using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class NodeTreeView : DockPanel
    {
        internal Utils.DelayedList<NodeTreeViewNode> Children = new Utils.DelayedList<NodeTreeViewNode>();

		internal List<IControl> menuItems = new List<IControl>();

		NodeTreeViewNode SelectedNode = null;

		bool isPopupShown = false;

		internal bool isVisibleChanging = false;
		internal bool changingVisibleMode = false;

		internal string treePyloadName = "NodeTreeNode";
		internal byte[] treePyload = new byte[4];

		internal List<Tuple<int, int, MovingNodeEventType>> exchangeEvents = new List<Tuple<int, int, MovingNodeEventType>>();

		/// <summary>
		/// For maintain parameters when removing and adding immediately
		/// </summary>
		internal Dictionary<Effekseer.Data.NodeBase, NodeTreeViewNode> temporalChangingNodeTreeViews = new Dictionary<Data.NodeBase, NodeTreeViewNode>();

		public NodeTreeView()
        {
			Label = Icons.PanelNodeTree + Resources.GetString("NodeTree") + "###NodeTree";

			Core.OnAfterNew += OnRenew;
			Core.OnAfterLoad += OnRenew;
			Core.OnAfterSelectNode += OnAfterSelect;

			Func<Func<bool>, Menu.MenuItem> create_menu_item_from_commands = (a) =>
			{
				var item = new Menu.MenuItem();
				var attributes = a.Method.GetCustomAttributes(false);
				var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
				item.Label = NameAttribute.GetName(attributes);
				item.Shortcut = Shortcuts.GetShortcutText(uniquename);
				item.Clicked += () =>
				{
					a();
				};

				return item;
			};

			menuItems.Add(create_menu_item_from_commands(Commands.AddNode));
			menuItems.Add(create_menu_item_from_commands(Commands.InsertNode));
			menuItems.Add(create_menu_item_from_commands(Commands.RemoveNode));
			menuItems.Add(create_menu_item_from_commands(Commands.RenameNode));
			menuItems.Add(new Menu.MenuSeparator());

			menuItems.Add(create_menu_item_from_commands(Commands.Copy));
			menuItems.Add(create_menu_item_from_commands(Commands.Paste));
			menuItems.Add(create_menu_item_from_commands(Commands.PasteInfo));

			menuItems.Add(new Menu.MenuSeparator());

			menuItems.Add(create_menu_item_from_commands(Commands.Undo));
			menuItems.Add(create_menu_item_from_commands(Commands.Redo));

			Renew();

			TabToolTip = Resources.GetString("NodeTree");
		}

		override protected void UpdateInternal()
		{
			float showHideButtonOffset = Manager.NativeManager.GetTextLineHeight();

			isPopupShown = false;

			var windowSize = Manager.NativeManager.GetContentRegionAvail();
			Manager.NativeManager.Columns(2);
			Manager.NativeManager.SetColumnOffset(1, Math.Max(0, windowSize.X - showHideButtonOffset));

			// Assign tree node index
			Action<Utils.DelayedList<NodeTreeViewNode>> assignIndex = null;
			int nodeIndex = 0;

			assignIndex = (objs) =>
			{
				foreach (var child in objs.Internal)
				{
					child.TreeNodeIndex = nodeIndex;
					nodeIndex++;

					if(child.IsExpanding)
					{
						assignIndex(child.Children);
					}	
				}
			};

			assignIndex(Children);

			Children.Lock();
			foreach (var child in Children.Internal)
			{
				child.Update();
			}
			Children.Unlock();

			Manager.NativeManager.Columns(1);

			// Run exchange events
			foreach (var pair in exchangeEvents)
			{
				Func<int, List<NodeTreeViewNode>, NodeTreeViewNode> findNode = null;

				findNode = (int id, List<NodeTreeViewNode> ns) =>
				{
					foreach (var n in ns)
					{
						if (n.UniqueID == id) return n;
						var ret = findNode(id, n.Children.Internal);
						if (ret != null)
						{
							return ret;
						}
					}

					return null;
				};

				var n1 = findNode(pair.Item1, Children.Internal);
				var n2 = findNode(pair.Item2, Children.Internal);

				var isNode = n1.Node is Data.Node;

				if(isNode)
				{
					var movedNode = n1.Node as Data.Node;

					if (pair.Item3 == MovingNodeEventType.AddLast)
					{
						Core.MoveNode(movedNode, n2.Node.Parent, int.MaxValue);
					}
					else if (pair.Item3 == MovingNodeEventType.Insert)
					{
						// to avoid root node
						if (n2.Node is Data.Node)
						{
							Core.MoveNode(movedNode, n2.Node.Parent, n2.Node.Parent.Children.Internal.IndexOf(n2.Node as Data.Node));
						}
					}
					else if (pair.Item3 == MovingNodeEventType.AddAsChild)
					{
						Core.MoveNode(movedNode, n2.Node, int.MaxValue);
					}
					else
					{
						throw new Exception();
					}
				}
			}
			exchangeEvents.Clear();

			// reset
			if(!Manager.NativeManager.IsMouseDown(0))
			{
				isVisibleChanging = false;
			}

			temporalChangingNodeTreeViews.Clear();
		}

        /// <summary>
        /// Renew all view
        /// </summary>
        public void Renew()
        {
			// Reset all
			if (Children.Count != 1 || Children[0].Node != Core.Root)
			{
				Children.Lock();
				foreach (var child in Children.Internal)
				{
					child.RemoveEvent(true);
				}
				Children.Unlock();
				Children.Clear();

				Children.Add(new NodeTreeViewNode(this, Core.Root));
			}

            Action<NodeTreeViewNode, Data.NodeBase> set_nodes = null;
            set_nodes = (treenode, node) =>
            {
                var tns = treenode.Children;
                var ns = node.Children;

                for (int ind = 0; ind < ns.Count;)
                {
                    // not need to change
                    if (ind < tns.Count && ((NodeTreeViewNode)tns[ind]).Node == ns[ind])
                    {
                        ind++;
                        continue;
                    }

                    // need to change
                    while (tns.Count > ind)
                    {
                        ((NodeTreeViewNode)tns[ind]).RemoveEvent(true);
                        tns.RemoveAt(ind);
                    }

                    while (ns.Count > ind)
                    {
                        tns.Add(new NodeTreeViewNode(this, ns[ind]));
                        ind++;
                    }
                }

                for (int i = 0; i < ns.Count; i++)
                {
                    set_nodes(tns[i], ns[i]);
                }
            };

            set_nodes(Children[0], Core.Root);
        }

		internal void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem("##Popup"))
			{
				foreach(var item in menuItems)
				{
					item.Update();
				}

				Manager.NativeManager.EndPopup();
				isPopupShown = true;
			}
		}

		/// <summary>
		/// When renew, it is called.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void OnRenew(object sender, EventArgs e)
        {
            Renew();
        }

		void ReadSelect()
		{
			Func<Data.NodeBase, Utils.DelayedList<NodeTreeViewNode>, NodeTreeViewNode> search_node = null;
			search_node = (searched_node, treenodes) =>
			{
				if (search_node == null) return null;

				for (int i = 0; i < treenodes.Count; i++)
				{
					if (treenodes[i].Node == searched_node) return treenodes[i];
					var ret = search_node(searched_node, treenodes[i].Children);
					if (ret != null) return ret;
				}
				return null;
			};

			var node = search_node(Core.SelectedNode, Children);

			SelectedNode = node;
		}

		void OnAfterSelect(object sender, EventArgs e)
		{
			ReadSelect();
		}
	}

	enum MovingNodeEventType
	{
		Insert,
		AddLast,
		AddAsChild,
	}

    class NodeTreeViewNode : IControl
    {
        string id = "";
		public int UniqueID { get; private set; }

        public Data.NodeBase Node { get; private set; } = null;

        internal Utils.DelayedList<NodeTreeViewNode> Children = new Utils.DelayedList<NodeTreeViewNode>();

		NodeTreeView treeView = null;

		bool requiredToExpand = false;

		public bool IsExpanding = false;

		public int TreeNodeIndex = 0;

		public NodeTreeViewNode(NodeTreeView treeView, Data.NodeBase node, bool createChildren = false)
        {
			UniqueID = Manager.GetUniqueID();
			id = "###" + UniqueID.ToString();

			this.treeView = treeView;
            this.Node = node;

            if (createChildren)
            {
                for(int i = 0; i < node.Children.Count; i++)
                {
                    var newNode = new NodeTreeViewNode(treeView, node.Children[i], true);
                    Children.Add(newNode);
                }
            }

			AddEvent(false);
		}

		public void Expand()
		{
			requiredToExpand = true;
		}

		public void ChangeVisible(bool recursion, bool value)
		{
			Command.CommandManager.StartCollection();

			Action<Data.NodeBase.ChildrenCollection> recurse = null;

			Node.IsRendered.SetValue(value);

			if(recursion)
			{
				recurse = (t) =>
				{
					for (int i = 0; i < t.Count; i++)
					{
						t[i].IsRendered.SetValue(value);
					}

					for (int i = 0; i < t.Count; i++)
					{
						recurse(t[i].Children);
					}
				};

				recurse(Node.Children);
			}


			Command.CommandManager.EndCollection();

		}

		public void AddEvent(bool recursion)
		{
			if (Node is Data.Node)
			{
				var realNode = (Data.Node)Node;
			}

			Node.OnAfterAddNode += OnAfterAddNode;
			Node.OnAfterRemoveNode += OnAfterRemoveNode;
			Node.OnAfterExchangeNodes += OnAfterExchangeNodes;
			if (recursion)
			{
				for (int i = 0; i < Children.Count; i++)
				{
					(Children[i]).AddEvent(true);
				}
			}
		}
		
		public void RemoveEvent(bool recursion)
        {
            if (Node is Data.Node)
            {
                var realNode = (Data.Node)Node;
            }

            Node.OnAfterAddNode -= OnAfterAddNode;
            Node.OnAfterRemoveNode -= OnAfterRemoveNode;
            Node.OnAfterExchangeNodes -= OnAfterExchangeNodes;
            if (recursion)
            {
                for (int i = 0; i < Children.Count; i++)
                {
                    (Children[i]).RemoveEvent(true);
                }
            }
        }

		void UpdateVisibleButton()
		{
			var visible = Node.IsRendered;

			float buttonSize = Manager.NativeManager.GetTextLineHeight();
			if (Manager.NativeManager.ImageButton(Images.GetIcon(visible ? "VisibleShow" : "VisibleHide"), buttonSize, buttonSize))
			{
				int LEFT_SHIFT = 340;
				int RIGHT_SHIFT = 344;

				if (Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || 
					Manager.NativeManager.IsKeyDown(RIGHT_SHIFT) ||
					((Node is Effekseer.Data.Node) && (Node as Effekseer.Data.Node).DrawingValues.Type.Value == Data.RendererValues.ParamaterType.None))
				{
					ChangeVisible(true, !visible);
				}
				else
				{
					ChangeVisible(false, !visible);
				}

				treeView.isVisibleChanging = true;
				treeView.changingVisibleMode = Node.IsRendered;
			}

			// Change value continuously
			if (Manager.NativeManager.IsItemHovered() && treeView.isVisibleChanging)
			{
				ChangeVisible(false, treeView.changingVisibleMode);
			}
		}

        public void Update()
        {
			var flag = swig.TreeNodeFlags.OpenOnArrow | swig.TreeNodeFlags.OpenOnDoubleClick | swig.TreeNodeFlags.DefaultOpen | swig.TreeNodeFlags.SpanFullWidth;

			if(Core.SelectedNode == this.Node)
			{
				flag = flag | swig.TreeNodeFlags.Selected;
			}

			if(this.Node.Children.Count == 0)
			{
				flag = flag | swig.TreeNodeFlags.Leaf;
			}

			UpdateDDTargetSeparator(false);

			if(requiredToExpand)
			{
				Manager.NativeManager.SetNextTreeNodeOpen(true);
				requiredToExpand = false;
			}

			var icon = Icons.NodeTypeEmpty;
			var node = Node as Data.Node;
			if(node != null)
			{
				if(node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Sprite) icon = Icons.NodeTypeSprite;
				if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ring) icon = Icons.NodeTypeRing;
				if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Ribbon) icon = Icons.NodeTypeRibbon;
				if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Model) icon = Icons.NodeTypeModel;
				if (node.DrawingValues.Type.Value == Data.RendererValues.ParamaterType.Track) icon = Icons.NodeTypeTrack;
			}

			// Change background color
			if(TreeNodeIndex % 2 == 1)
			{
				Manager.NativeManager.DrawLineBackground(Manager.NativeManager.GetTextLineHeight(), 0x0cffffff);
			}

			// Extend clickable space
			var label = icon + " " + Node.Name + id;
			IsExpanding = Manager.NativeManager.TreeNodeEx(label, flag);

			SelectNodeIfClicked();

			treeView.Popup();

			// D&D Source
			if (Manager.NativeManager.BeginDragDropSource())
			{
				byte[] idBuf = BitConverter.GetBytes(UniqueID);
				if (Manager.NativeManager.SetDragDropPayload(treeView.treePyloadName, idBuf, idBuf.Length))
				{
				}
				Manager.NativeManager.Text(this.Node.Name);

				Manager.NativeManager.EndDragDropSource();
			}

			UpdateDDTargetNode();

			Manager.NativeManager.NextColumn();

			UpdateVisibleButton();

			Manager.NativeManager.NextColumn();

			if (IsExpanding)
			{
				Children.Lock();

				foreach (var child in Children.Internal)
				{
					child.Update();
				}

				Children.Unlock();

				if (Children.Count != 0)
				{
					Children.Internal.Last().UpdateDDTargetSeparator(true);
				}

				// pair with TreeNodeEx
				Manager.NativeManager.TreePop();
			}
		}

		private void SelectNodeIfClicked()
		{
			if (Manager.NativeManager.IsItemClicked(0) ||
				Manager.NativeManager.IsItemClicked(1) ||
				Manager.NativeManager.IsItemFocused())
			{
				Core.SelectedNode = this.Node;
			}
		}

		/// <summary>
		/// Update D&D Target between nodes
		/// </summary>
		/// <param name="isEnd"></param>
		void UpdateDDTargetSeparator(bool isEnd)
		{
			// Hidden separator is a target to be dropped
			// adjust a position
			Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() - 6);
			Manager.NativeManager.HiddenSeparator(12, 6);

			if (Manager.NativeManager.BeginDragDropTarget())
			{
				int size = 0;
				if (Manager.NativeManager.AcceptDragDropPayload(treeView.treePyloadName, treeView.treePyload, treeView.treePyload.Length, ref size))
				{
					var sourceID = BitConverter.ToInt32(treeView.treePyload, 0);
					treeView.exchangeEvents.Add(
						Tuple.Create(sourceID, UniqueID, (isEnd ? MovingNodeEventType.AddLast : MovingNodeEventType.Insert)));
				}

				Manager.NativeManager.EndDragDropTarget();
			}
		}

		/// <summary>
		/// Update nodes as D&D target
		/// </summary>
		void UpdateDDTargetNode()
		{
			if (Manager.NativeManager.BeginDragDropTarget())
			{
				int size = 0;
				if (Manager.NativeManager.AcceptDragDropPayload(treeView.treePyloadName, treeView.treePyload, treeView.treePyload.Length, ref size))
				{
					var sourceID = BitConverter.ToInt32(treeView.treePyload, 0);
					treeView.exchangeEvents.Add(
						Tuple.Create(sourceID, UniqueID, MovingNodeEventType.AddAsChild));
				}

				Manager.NativeManager.EndDragDropTarget();
			}
		}

		void OnAfterAddNode(object sender, ChangedValueEventArgs e)
        {
            var node = e.Value as Data.NodeBase;

			Console.WriteLine(string.Format("OnAfterAddNode({0})", node.Name.Value));

			int ind = 0;
            for (; ind < Node.Children.Count; ind++)
            {
                if (node == Node.Children[ind]) break;
            }

			NodeTreeViewNode treeViewNode = null;
			if(treeView.temporalChangingNodeTreeViews.ContainsKey(node))
			{
				treeViewNode = treeView.temporalChangingNodeTreeViews[node];
				treeViewNode.AddEvent(true);
			}
			else
			{
				treeViewNode = new NodeTreeViewNode(treeView, node, true);
			}

			if (ind == Children.Count)
            {
                Children.Add(treeViewNode);
            }
            else
            {
                Children.Insert(ind, treeViewNode);
            }
        }

        void OnAfterRemoveNode(object sender, ChangedValueEventArgs e)
        {
            var node = e.Value as Data.NodeBase;

			Console.WriteLine(string.Format("OnAfterRemoveNode({0})", node.Name.Value));

			for (int i = 0; i < Children.Count; i++)
            {
                var treenode = Children[i];
                if (treenode.Node == node)
                {
                    treenode.RemoveEvent(true);
                    Children.Remove(treenode);

					if (!treeView.temporalChangingNodeTreeViews.ContainsKey(node))
					{
						treeView.temporalChangingNodeTreeViews.Add(treenode.Node, treenode);
					}
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

            for (int i = 0; i < Children.Count; i++)
            {
                var treenode = Children[i];
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

            Children.Insert(ind2, new NodeTreeViewNode(treeView, node1, true));
            Children.RemoveAt(ind2 + 1);

            Children.Insert(ind1, new NodeTreeViewNode(treeView, node2, true));
            Children.RemoveAt(ind1 + 1);
        }
    }
}
