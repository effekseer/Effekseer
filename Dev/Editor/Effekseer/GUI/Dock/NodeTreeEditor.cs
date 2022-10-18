using System;
using System.Collections.Generic;
using System.Linq;
using Effekseer.Data;
using Effekseer.swig;

namespace Effekseer.GUI.Dock
{
	class NodeTreeEditor : DockPanel
	{
		internal Utils.DelayedList<NodeTreeEditorNode> Children = new Utils.DelayedList<NodeTreeEditorNode>();

		internal List<IControl> menuItems = new List<IControl>();

		NodeTreeEditorNode SelectedNode = null;

		bool isPopupShown = false;

		internal bool isVisibleChanging = false;
		internal bool changingVisibleMode = false;

		internal string treePyloadName = "NodeTreeNode";
		internal byte[] treePyload = new byte[4];

		internal List<Tuple<int, int, MovingNodeEditorEventType>> exchangeEvents = new List<Tuple<int, int, MovingNodeEditorEventType>>();

		/// <summary>
		/// For maintain parameters when removing and adding immediately
		/// </summary>
		internal Dictionary<EffectAsset.Node, NodeTreeEditorNode> temporalChangingNodeTreeViews = new Dictionary<EffectAsset.Node, NodeTreeEditorNode>();

		public NodeTreeEditor()
		{
			Label = Icons.PanelNodeTree + Resources.GetString("NodeTreeEditor") + "###NodeTreeEditor";

			Core.OnAfterNew += OnRenew;
			Core.OnAfterLoad += OnRenew;
			Core.OnAfterSelectNode += OnAfterSelect;

			Menu.MenuItem create_menu_item_from_commands(Func<bool> a)
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

			// TODO fix it

			Func<bool> addNode = () =>
			{
				CoreContext.SelectedEffect.Context.CommandManager.AddNode(
				CoreContext.SelectedEffect.Asset.NodeTreeAsset,
				CoreContext.SelectedEffect.Context.NodeTree,
				CoreContext.SelectedEffectNode.InstanceID,
				typeof(EffectAsset.ParticleNode),
				CoreContext.Environment);
				return true;
			};

			menuItems.Add(create_menu_item_from_commands(addNode));
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
			float showHideButtonOffset = Manager.NativeManager.GetTextLineHeight() * 3
										 + Manager.NativeManager.GetStyleVar2(ImGuiStyleVarFlags.ItemSpacing).X;

			isPopupShown = false;

			var windowSize = Manager.NativeManager.GetContentRegionAvail();

			Manager.NativeManager.BeginTable("NodeTreeView", 2);
			Manager.NativeManager.TableSetupColumn("Nodes", TableColumnFlags.NoResize | TableColumnFlags.WidthFixed, Math.Max(0, windowSize.X - showHideButtonOffset));
			Manager.NativeManager.TableSetupColumn("Options");

			// Assign tree node index
			Action<Utils.DelayedList<NodeTreeEditorNode>> assignIndex = null;
			int nodeIndex = 0;

			assignIndex = (objs) =>
			{
				foreach (var child in objs.Internal)
				{
					child.TreeNodeIndex = nodeIndex;
					nodeIndex++;

					if (child.IsExpanding)
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

			Manager.NativeManager.EndTable();

			// Run exchange events
			foreach (var pair in exchangeEvents)
			{
				var n1 = FindNode(Children[0], pair.Item1);
				var n2 = FindNode(Children[0], pair.Item2);

				var isNode = n1.Node is EffectAsset.ParticleNode;

				if (isNode)
				{
					var movedNode = n1.Node as EffectAsset.ParticleNode;

					if (pair.Item3 == MovingNodeEditorEventType.AddLast)
					{
						var parent = FindParentNode(Children[0], n2.UniqueID);
						CoreContext.SelectedEffect.Context.CommandManager.MoveNode(
							CoreContext.SelectedEffect.Asset.NodeTreeAsset,
							CoreContext.SelectedEffect.Context.NodeTree,
							movedNode.InstanceID,
							parent.Node.InstanceID,
							int.MaxValue,
							CoreContext.Environment);
					}
					else if (pair.Item3 == MovingNodeEditorEventType.Insert)
					{
						// to avoid root node
						if (n2.Node is EffectAsset.ParticleNode)
						{
							var parent = FindParentNode(Children[0], n2.UniqueID);
							CoreContext.SelectedEffect.Context.CommandManager.MoveNode(
								CoreContext.SelectedEffect.Asset.NodeTreeAsset,
								CoreContext.SelectedEffect.Context.NodeTree,
								movedNode.InstanceID,
								parent.Node.InstanceID,
								parent.Children.Internal.IndexOf(n2),
								CoreContext.Environment);
						}
					}
					else if (pair.Item3 == MovingNodeEditorEventType.AddAsChild)
					{
						var parent = FindParentNode(Children[0], n2.UniqueID);
						CoreContext.SelectedEffect.Context.CommandManager.MoveNode(
							CoreContext.SelectedEffect.Asset.NodeTreeAsset,
							CoreContext.SelectedEffect.Context.NodeTree,
							movedNode.InstanceID,
							n2.Node.InstanceID,
							int.MaxValue,
							CoreContext.Environment);
					}
					else
					{
						throw new Exception();
					}
				}
			}
			exchangeEvents.Clear();

			// reset
			if (!Manager.NativeManager.IsMouseDown(0))
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
			if (Children.Count != 1 || Children[0].Node != CoreContext.SelectedEffect.Context.NodeTree.Root)
			{
				Children.Lock();
				foreach (var child in Children.Internal)
				{
					child.RemoveEvent(true);
				}
				Children.Unlock();
				Children.Clear();

				Children.Add(new NodeTreeEditorNode(this, CoreContext.SelectedEffect.Context.NodeTree.Root as EffectAsset.Node));
			}

			Action<NodeTreeEditorNode, EffectAsset.Node> set_nodes = null;
			set_nodes = (treenode, node) =>
			{
				var tns = treenode.Children;
				var ns = node.Children;

				for (int ind = 0; ind < ns.Count;)
				{
					// not need to change
					if (ind < tns.Count && ((NodeTreeEditorNode)tns[ind]).Node == ns[ind])
					{
						ind++;
						continue;
					}

					// need to change
					while (tns.Count > ind)
					{
						((NodeTreeEditorNode)tns[ind]).RemoveEvent(true);
						tns.RemoveAt(ind);
					}

					while (ns.Count > ind)
					{
						tns.Add(new NodeTreeEditorNode(this, ns[ind]));
						ind++;
					}
				}

				for (int i = 0; i < ns.Count; i++)
				{
					set_nodes(tns[i], ns[i]);
				}
			};

			set_nodes(Children[0], CoreContext.SelectedEffect.Context.NodeTree.Root as EffectAsset.Node);
		}

		internal void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem("##Popup"))
			{
				foreach (var item in menuItems)
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
			Func<EffectAsset.Node, Utils.DelayedList<NodeTreeEditorNode>, NodeTreeEditorNode> search_node = null;
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

			var node = search_node(CoreContext.SelectedEffectNode, Children);

			SelectedNode = node;
		}

		void OnAfterSelect(object sender, EventArgs e)
		{
			ReadSelect();
		}


		NodeTreeEditorNode FindNode(NodeTreeEditorNode node, int uniqueID)
		{
			if (node.UniqueID == uniqueID)
			{
				return node;
			}

			foreach (var child in node.Children.Internal)
			{
				var found = FindNode(child, uniqueID);
				if (found != null)
				{
					return found;
				}
			}

			return null;
		}

		NodeTreeEditorNode FindParentNode(NodeTreeEditorNode node, int childUniqueID)
		{
			foreach (var child in node.Children.Internal)
			{
				if (child.UniqueID == childUniqueID)
				{
					return node;
				}

				var found = FindParentNode(child, childUniqueID);
				if (found != null)
				{
					return found;
				}
			}

			return null;
		}

	}

	enum MovingNodeEditorEventType
	{
		Insert,
		AddLast,
		AddAsChild,
	}

	class NodeTreeEditorNode
	{
		string id = "";
		public int UniqueID { get; private set; }

		public EffectAsset.Node Node { get; private set; } = null;

		internal Utils.DelayedList<NodeTreeEditorNode> Children = new Utils.DelayedList<NodeTreeEditorNode>();

		private BindableComponent.Enum lodBehaviourEnumControl;

		NodeTreeEditor treeView = null;

		bool requiredToExpand = false;

		public bool IsExpanding = false;

		public int TreeNodeIndex = 0;

		public NodeTreeEditorNode(NodeTreeEditor treeView, EffectAsset.Node node, bool createChildren = false)
		{
			UniqueID = Manager.GetUniqueID();
			id = "###" + UniqueID.ToString();

			this.treeView = treeView;
			this.Node = node;

			if (createChildren)
			{
				for (int i = 0; i < node.Children.Count; i++)
				{
					var newNode = new NodeTreeEditorNode(treeView, node.Children[i], true);
					Children.Add(newNode);
				}
			}

			var effectNode = node as EffectAsset.ParticleNode;
			if (effectNode != null)
			{
				lodBehaviourEnumControl = new BindableComponent.Enum();
				lodBehaviourEnumControl.Initialize(typeof(LODBehaviourType));
				lodBehaviourEnumControl.SetBinding(effectNode.CommonValues.LodParameter.LodBehaviour);
				lodBehaviourEnumControl.EnableUndo = true;
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

			Action<List<EffectAsset.Node>> recurse = null;

			Node.IsRendered = value;

			if (recursion)
			{
				recurse = (t) =>
				{
					for (int i = 0; i < t.Count; i++)
					{
						t[i].IsRendered = value;
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
			Node.AddedNode += Node_AddedNode;
			Node.RemovedNode += Node_RemovedNode;
			Node.InsertedNode += Node_InsertedNode;

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
			Node.AddedNode -= Node_AddedNode;
			Node.RemovedNode -= Node_RemovedNode;
			Node.InsertedNode -= Node_InsertedNode;

			if (recursion)
			{
				for (int i = 0; i < Children.Count; i++)
				{
					(Children[i]).RemoveEvent(true);
				}
			}
		}

		void UpdateLODButton()
		{
			var node = Node as EffectAsset.ParticleNode;
			float lodButtonSize = Manager.NativeManager.GetTextLineHeight();
			if (node != null)
			{
				int enabledLevels = Core.LodValues.GetEnabledLevelsBits();
				int enabledLevelsCount = 0;
				for (int i = 0; i < LOD.LevelCount; i++)
				{
					enabledLevelsCount += ((enabledLevels & (1 << i)) > 0) ? 1 : 0;
				}

				int levels = node.CommonValues.LodParameter.MatchingLODs & enabledLevels;

				Manager.NativeManager.Button(id, lodButtonSize, lodButtonSize);


				for (int i = 0; i < LOD.LevelCount; i++)
				{
					bool isEnabled = (levels & (1 << i)) > 0;
					if (!isEnabled) continue;

					float boxW = Manager.NativeManager.GetItemRectSizeX();
					float boxH = Manager.NativeManager.GetItemRectSizeY();
					float pad = 2F;
					float spacing = 2F;
					float entryW = boxW - 2F * pad;
					float entryH = (boxH - 2F * pad - enabledLevelsCount * spacing) / enabledLevelsCount;
					float x = Manager.NativeManager.GetItemRectMinX() + pad;
					float y = Manager.NativeManager.GetItemRectMinY() + (entryH + spacing) * i + pad + 1F;

					Manager.NativeManager.AddRectFilled(x, y, x + entryW, y + entryH,
						LOD.LevelColors[i], 5, 0);
				}

				if (Manager.NativeManager.IsItemClicked(0))
				{
					Manager.NativeManager.OpenPopup("LodEditor " + id);
				}

				if (Manager.NativeManager.BeginPopup("LodEditor " + id))
				{
					if (Core.LodValues.GetEnabledLevelsBits() == 1)
					{
						Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("LOD_NotConfigured"));
						if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("LOD_Configure")))
						{
							var state = Manager.MainWindow.GetState();
							Manager.SelectOrShowWindow(typeof(Dock.LOD),
								new swig.Vec2(state.Width * 0.25f, state.Height * 0.5f), true, false);
						}
					}
					else
					{
						Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("LOD_SelectMatching"));
						for (int i = 0; i < LOD.LevelCount; i++)
						{
							if ((enabledLevels & (1 << i)) == 0) break;

							bool[] level0Match = { (levels & (1 << i)) > 0 };

							Manager.NativeManager.AlignTextToFramePadding();
							Manager.NativeManager.PushStyleColor(ImGuiColFlags.Text, LOD.LevelColors[i]);
							Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("LOD_Level") + " " + i);
							Manager.NativeManager.PopStyleColor();
							Manager.NativeManager.SameLine();
							if (Manager.NativeManager.Checkbox("##level" + i, level0Match))
							{
								node.CommonValues.LodParameter.MatchingLODs = (
									(level0Match[0] ? 1 << i : 0) | (levels & ~(1 << i)));
								ApplyLODSettingsToChildren();
							}
						}

						Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("LOD_Behaviour"));
						var prevBehaviour = node.CommonValues.LodParameter.LodBehaviour;
						lodBehaviourEnumControl.Update();
						// detecting value change without listener
						if (prevBehaviour != node.CommonValues.LodParameter.LodBehaviour)
						{
							ApplyLODSettingsToChildren();
						}
						Manager.NativeManager.Separator();
						if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("LOD_ApplyToChildren")))
						{
							ApplyLODSettingsToChildren();
						}
					}

					Manager.NativeManager.EndPopup();
				}
			}
			else
			{
				Manager.NativeManager.Button("##dummy", lodButtonSize, lodButtonSize);
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
					((Node is EffectAsset.ParticleNode) && (Node as EffectAsset.ParticleNode).DrawingValues.Type == Data.RendererValues.ParamaterType.None))
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
			Manager.NativeManager.TableNextRow();
			Manager.NativeManager.TableSetColumnIndex(0);

			var flag = swig.TreeNodeFlags.OpenOnArrow | swig.TreeNodeFlags.OpenOnDoubleClick | swig.TreeNodeFlags.DefaultOpen | swig.TreeNodeFlags.SpanFullWidth;

			if (CoreContext.SelectedEffectNode == this.Node)
			{
				flag = flag | swig.TreeNodeFlags.Selected;
			}

			if (this.Node.Children.Count == 0)
			{
				flag = flag | swig.TreeNodeFlags.Leaf;
			}

			UpdateDDTargetSeparator(false);

			if (requiredToExpand)
			{
				Manager.NativeManager.SetNextItemOpen(true);
				requiredToExpand = false;
			}

			var icon = Icons.NodeTypeEmpty;
			var node = Node as EffectAsset.ParticleNode;
			if (node != null)
			{
				if (node.DrawingValues.Type == Data.RendererValues.ParamaterType.Sprite) icon = Icons.NodeTypeSprite;
				if (node.DrawingValues.Type == Data.RendererValues.ParamaterType.Ring) icon = Icons.NodeTypeRing;
				if (node.DrawingValues.Type == Data.RendererValues.ParamaterType.Ribbon) icon = Icons.NodeTypeRibbon;
				if (node.DrawingValues.Type == Data.RendererValues.ParamaterType.Model) icon = Icons.NodeTypeModel;
				if (node.DrawingValues.Type == Data.RendererValues.ParamaterType.Track) icon = Icons.NodeTypeTrack;
			}

			// Change background color
			if (TreeNodeIndex % 2 == 1)
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

			Manager.NativeManager.TableSetColumnIndex(1);

			UpdateLODButton();

			Manager.NativeManager.SameLine();

			UpdateVisibleButton();

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

		private void ApplyLODSettingsToChildren()
		{
			var effectNode = Node as EffectAsset.ParticleNode;
			if (effectNode == null) return;

			int matchingLods = effectNode.CommonValues.LodParameter.MatchingLODs;
			LODBehaviourType lodBehaviour = effectNode.CommonValues.LodParameter.LodBehaviour;
			for (var i = 0; i < Children.Count; i++)
			{
				var child = Children[i];
				var childNode = child.Node as EffectAsset.ParticleNode;
				if (childNode == null) continue;

				childNode.CommonValues.LodParameter.MatchingLODs = matchingLods;
				childNode.CommonValues.LodParameter.LodBehaviour = lodBehaviour;
				child.ApplyLODSettingsToChildren();
			}
		}

		private void SelectNodeIfClicked()
		{
			int KEY_ENTER = 257;

			if ((Manager.NativeManager.IsItemFocused() && Manager.NativeManager.IsKeyDown(KEY_ENTER)) ||
				Manager.NativeManager.IsItemClicked(0) ||
				Manager.NativeManager.IsItemClicked(1))
			{
				CoreContext.SelectedEffectNode = this.Node;
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

			var windowSize = Manager.NativeManager.GetContentRegionAvail();

			Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() - 6);
			Manager.NativeManager.Dummy(new swig.Vector2I((int)windowSize.X, 12));
			Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() - 6);

			if (Manager.NativeManager.BeginDragDropTarget())
			{
				int size = 0;
				if (Manager.NativeManager.AcceptDragDropPayload(treeView.treePyloadName, treeView.treePyload, treeView.treePyload.Length, ref size))
				{
					var sourceID = BitConverter.ToInt32(treeView.treePyload, 0);
					treeView.exchangeEvents.Add(
						Tuple.Create(sourceID, UniqueID, (isEnd ? MovingNodeEditorEventType.AddLast : MovingNodeEditorEventType.Insert)));
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
						Tuple.Create(sourceID, UniqueID, MovingNodeEditorEventType.AddAsChild));
				}

				Manager.NativeManager.EndDragDropTarget();
			}
		}

		private void Node_AddedNode(EffectAsset.Node self, EffectAsset.Node added)
		{
			var node = added as EffectAsset.Node;

			Console.WriteLine(string.Format("OnAfterAddNode({0})", node.Name));

			int ind = 0;
			for (; ind < Node.Children.Count; ind++)
			{
				if (node == Node.Children[ind]) break;
			}

			NodeTreeEditorNode treeViewNode = null;
			if (treeView.temporalChangingNodeTreeViews.ContainsKey(node))
			{
				treeViewNode = treeView.temporalChangingNodeTreeViews[node];
				treeViewNode.AddEvent(true);
			}
			else
			{
				treeViewNode = new NodeTreeEditorNode(treeView, node, true);
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

		private void Node_RemovedNode(EffectAsset.Node self, EffectAsset.Node removed)
		{
			var node = removed as EffectAsset.Node;

			Console.WriteLine(string.Format("OnAfterRemoveNode({0})", node.Name));

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

		private void Node_InsertedNode(EffectAsset.Node self, int index, EffectAsset.Node inserted)
		{
			Children.Insert(index, new NodeTreeEditorNode(treeView, inserted, true));

			throw new NotImplementedException();
		}
	}
}