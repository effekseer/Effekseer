using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class NodeBase
	{
		List<Node> children = new List<Node>();

		[Key(key = "Node_IsRendered")]
		public Value.Boolean IsRendered
		{
			get;
			private set;
		}

		[Key(key = "Node_Name")]
		public Value.String Name
		{
			get;
			private set;
		}

		[IO(Export=false)]
		public NodeBase Parent
		{
			get;
			private set;
		}

		public ChildrenCollection Children
		{
			get;
			private set;
		}

		/// <summary>
		/// Identifier to use when referring to a node from the editor.
		/// </summary>
		[IO(Export = false)]
		public int EditorNodeId
		{
			get;
			set;
		}

		public event ChangedValueEventHandler OnAfterAddNode;

		public event ChangedValueEventHandler OnAfterRemoveNode;

		public event ChangedValueEventHandler OnAfterExchangeNodes;

		/// <summary>
		/// コンストラクタ
		/// </summary>
		internal NodeBase(NodeBase parent)
		{
			Parent = parent;
			Name = new Value.String("Node");
			IsRendered = new Value.Boolean(true);
			Children = new ChildrenCollection(this);
		}

		/// <summary>
		/// Get root node
		/// </summary>
		/// <returns>Root node</returns>
		public NodeRoot GetRoot()
		{
			var node = Parent;
			while (node != null)
			{
				if (node is NodeRoot)
					return node as NodeRoot;
				node = node.Parent;
			}
			return null;
		}

		/// <summary>
		/// Add child
		/// </summary>
		/// <param name="node">Added node (if null, generated automatically)</param>
		/// <param name="index">inserted position</param>
		/// <returns></returns>
		public Node AddChild(Node node = null, int index = int.MaxValue)
		{
			if(node == null)
			{
				node = new Node(this);
			}

			var old_parent = node.Parent;
			var new_parent = this;
			var old_value = children;
			var new_value = new List<Node>(children);

			if(index == int.MaxValue)
			{
				new_value.Add(node);
			}
			else
			{
				if(index >= children.Count)
				{
					new_value.Add(node);
				}
				else
				{
					new_value.Insert(index, node);
				}
			}

			var cmd = new Command.DelegateCommand(
				() =>
				{
					children = new_value;
					node.Parent = new_parent;

					if (OnAfterAddNode != null)
					{
						OnAfterAddNode(this, new ChangedValueEventArgs(node, ChangedValueType.Execute));
					}
				},
				() =>
				{
					children = old_value;
					node.Parent = old_parent;

					if (OnAfterRemoveNode != null)
					{
						OnAfterRemoveNode(this, new ChangedValueEventArgs(node, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);

			return node;
		}

		/// <summary>
		/// 子ノードの破棄
		/// </summary>
		/// <param name="node"></param>
		/// <returns></returns>
		public bool RemoveChild(Node node)
		{
			if (!children.Contains(node)) return false;

			var old_value = children;
			var new_value = new List<Node>(children);
			new_value.Remove(node);

			var cmd = new Command.DelegateCommand(
				() =>
				{
					children = new_value;

					if (OnAfterRemoveNode != null)
					{
						OnAfterRemoveNode(this, new ChangedValueEventArgs(node, ChangedValueType.Execute));
					}
				},
				() =>
				{
					children = old_value;

					if (OnAfterAddNode != null)
					{
						OnAfterAddNode(this, new ChangedValueEventArgs(node, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);

			if (node == Core.SelectedNode)
			{
				Core.SelectedNode = null;
			}

			return true;
		}

		/// <summary>
		/// 現在のノードの親とノードの間にノードを挿入
		/// </summary>
		public Node InsertParent()
		{
			if (Parent == null) return null;

			var node = new Node(Parent);
			node.children.Add((Node)this);

			var old_value = Parent.children;
			var new_value = new List<Node>(Parent.children);
			for (int i = 0; i < new_value.Count; i++)
			{
				if (new_value[i] == this)
				{
					new_value[i] = node;
					break;
				}
			}

			var parent = Parent;
			var _this = this;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					parent.children = new_value;
					_this.Parent = node;
					if (parent.OnAfterRemoveNode != null)
					{
						parent.OnAfterRemoveNode(parent, new ChangedValueEventArgs(_this, ChangedValueType.Execute));
					}

					if (parent.OnAfterAddNode != null)
					{
						parent.OnAfterAddNode(parent, new ChangedValueEventArgs(node, ChangedValueType.Execute));
					}

				},
				() =>
				{
					parent.children = old_value;
					_this.Parent = parent;
					if (parent.OnAfterAddNode != null)
					{
						parent.OnAfterAddNode(this, new ChangedValueEventArgs(_this, ChangedValueType.Unexecute));
					}

					if (parent.OnAfterRemoveNode != null)
					{
						parent.OnAfterRemoveNode(parent, new ChangedValueEventArgs(node, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);

			return node;
		}

		/// <summary>
		/// 子ノードの全破棄
		/// </summary>
		public void ClearChildren()
		{
			Command.CommandManager.StartCollection();

			while (children.Count > 0)
			{
				RemoveChild(children[0]);
			}

			Command.CommandManager.EndCollection();
		}

		/// <summary>
		/// Exchange children each other
		/// </summary>
		/// <param name="node1"></param>
		/// <param name="node2"></param>
		/// <returns></returns>
		public bool ExchangeChildren(Node node1, Node node2)
		{
			if (node1 == node2) return false;
			if (!children.Contains(node1)) return false;
			if (!children.Contains(node2)) return false;

			var old_value = children;
			var new_value = new List<Node>(children);
			var ind1 = new_value.FindIndex((n) => { return n == node1; });
			var ind2 = new_value.FindIndex((n) => { return n == node2; });
			new_value[ind1] = node2;
			new_value[ind2] = node1;

			

			var cmd = new Command.DelegateCommand(
				() =>
				{
					Tuple35<Node, Node> nodes = new Tuple35<Node, Node>(node1, node2);
					children = new_value;

					if (OnAfterExchangeNodes != null)
					{
						OnAfterExchangeNodes(this, new ChangedValueEventArgs(nodes, ChangedValueType.Execute));
					}
				},
				() =>
				{
					Tuple35<Node, Node> nodes = new Tuple35<Node, Node>(node2, node1);
					children = old_value;

					if (OnAfterExchangeNodes != null)
					{
						OnAfterExchangeNodes(this, new ChangedValueEventArgs(nodes, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);

			return true;
		}

		/// <summary>
		/// ノードを子として保有しているか取得
		/// </summary>
		/// <param name="node">ノード</param>
		/// <param name="recursion">再帰的に検索するか</param>
		/// <returns></returns>
		public bool HasNode(NodeBase node, bool recursion)
		{
			for (int i = 0; i < Children.Count; i++)
			{
				if (Children[i] == node) return true;

				if (recursion)
				{
					if (Children[i].HasNode(node, true)) return true;
				}
			}

			return false;
		}

		/// <summary>
		/// Get the layer number in the hierarchy
		/// </summary>
		/// <returns>the layer number</returns>
		public int GetLayerNumber()
		{
			if (Parent != null)
			{
				return 1 + Parent.GetLayerNumber();
			}
			else
			{
				return 1;
			}
		}

		/// <summary>
		/// Get the number of the deepest layer in chidren 
		/// </summary>
		/// <returns></returns>
		public int GetDeepestLayerNumberInChildren()
		{
			int maxGen = 0;
			foreach (var child in children)
			{
				int gen = child.GetDeepestLayerNumberInChildren();
				if (gen > maxGen) maxGen = gen;
			}
			return maxGen + 1;
		}

		/// <summary>
		/// Children
		/// </summary>
		public class ChildrenCollection
		{
			NodeBase _node = null;

			public NodeBase Node
			{
				get
				{
					return _node;
				}
			}

			internal ChildrenCollection(NodeBase node)
			{
				_node = node; 
			}

			internal void SetChildren(List<Node> children)
			{
				_node.children = children;
				foreach (var child in _node.children)
				{
					child.Parent = _node;
				}
			}

			public int Count
			{
				get
				{
					return _node.children.Count;
				}
			}

			public Node this[int index]
			{
				get
				{
					if (Count > index)
					{
						return _node.children[index];
					}
					return null;
				}
			}

			public List<Node> Internal
			{
				get
				{
					return _node.children;
				}
			}
		}
	}

	public class NodeBaseValues : Data.IEditableValueCollection
	{
		Data.NodeBase node;

		public NodeBaseValues(Data.NodeBase node)
		{
			this.node = node;
		}
		public Data.EditableValue[] GetValues()
		{
			List<Data.EditableValue> values = new List<Data.EditableValue>();

			var valueIsRendered = Data.EditableValue.Create(node.IsRendered, node.GetType().GetProperty("IsRendered"));
			var valueName = Data.EditableValue.Create(node.Name, node.GetType().GetProperty("Name"));

			return new[]
			{
				valueIsRendered,
				valueName,
			};
		}

		public event ChangedValueEventHandler OnChanged;
	}

}
