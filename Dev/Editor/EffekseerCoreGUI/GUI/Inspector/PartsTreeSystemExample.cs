using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using System.Linq;

/**
 The file should be removed
 */

namespace Effekseer.GUI.Inspector
{

	class NodeTreeGroupContext
	{
		public PartsTreeSystem.NodeTreeGroup NodeTreeGroup;
		public PartsTreeSystem.NodeTree NodeTree;
		public PartsTreeSystem.NodeTreeGroupEditorProperty EditorProperty;
		public PartsTreeSystem.CommandManager CommandManager;

		public void New(Type type, EditorState state)
		{
			NodeTreeGroup = new PartsTreeSystem.NodeTreeGroup();
			NodeTreeGroup.Init(type, state.Env);
			EditorProperty = new PartsTreeSystem.NodeTreeGroupEditorProperty(NodeTreeGroup, state.Env);
			NodeTree = PartsTreeSystem.Utility.CreateNodeFromNodeTreeGroup(NodeTreeGroup, state.Env);
			CommandManager = new PartsTreeSystem.CommandManager();
		}

		public void Load(string path, EditorState state)
		{
			var text = System.IO.File.ReadAllText(path);
			NodeTreeGroup = PartsTreeSystem.NodeTreeGroup.Deserialize(text);
			EditorProperty = new PartsTreeSystem.NodeTreeGroupEditorProperty(NodeTreeGroup, state.Env);
			NodeTree = PartsTreeSystem.Utility.CreateNodeFromNodeTreeGroup(NodeTreeGroup, state.Env);
			CommandManager = new PartsTreeSystem.CommandManager();
		}
	}

	class EditorState
	{
		public PartsTreeSystem.Environment Env;
		public PartsList PartsList;
		public Node SelectedNode = null;
		public Node PopupedNode = null;
		public string NextLoadPath = string.Empty;

		public void Unselect()
		{
			SelectedNode = null;
			PopupedNode = null;
		}
	}

	public class Node : PartsTreeSystem.INode
	{
		public int InstanceID { get; set; }

		[System.NonSerialized]
		public List<Node> Children = new List<Node>();

		public void AddChild(PartsTreeSystem.INode node)
		{
			Children.Add(node as Node);
		}

		public void RemoveChild(int instanceID)
		{
			Children.RemoveAll(_ => _.InstanceID == instanceID);
		}

		public void InsertChild(int index, PartsTreeSystem.INode node)
		{
			Children.Insert(index, node as Node);
		}

		public IReadOnlyCollection<PartsTreeSystem.INode> GetChildren()
		{
			return Children;
		}
	}

	class Environment : PartsTreeSystem.Environment
	{
		Dictionary<PartsTreeSystem.Asset, string> pathes = new Dictionary<PartsTreeSystem.Asset, string>();
		public override PartsTreeSystem.Asset GetAsset(string path)
		{
			if (pathes.ContainsValue(path))
			{
				return pathes.Where(_ => _.Value == path).FirstOrDefault().Key;
			}
			var text = System.IO.File.ReadAllText(path);
			var nodeTreeGroup = PartsTreeSystem.NodeTreeGroup.Deserialize(text);

			pathes.Add(nodeTreeGroup, path);
			return nodeTreeGroup;
		}

		public override string GetAssetPath(PartsTreeSystem.Asset asset)
		{
			if (pathes.TryGetValue(asset, out var path))
			{
				return System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), path);
			}
			return System.IO.Directory.GetCurrentDirectory();
		}
	}

	class PartsList
	{
		public IReadOnlyCollection<string> Pathes { get { return pathes; } }

		string[] pathes = new string[0];

		public void Renew()
		{
			pathes = System.IO.Directory.GetFiles("./", "*.nodes");
		}
	}

	class FieldGetterSetter
	{
		object parent;
		System.Reflection.FieldInfo fieldInfo;
		int? index;

		public void Reset(object o, System.Reflection.FieldInfo fieldInfo)
		{
			parent = o;
			this.fieldInfo = fieldInfo;
			index = null;
		}

		public void Reset(object o, int index)
		{
			parent = o;
			fieldInfo = null;
			this.index = index;
		}

		public string GetName()
		{
			if (fieldInfo != null)
			{
				return fieldInfo.Name;
			}
			else if (index.HasValue)
			{
				return index.Value.ToString();
			}

			return string.Empty;
		}

		public object GetValue()
		{
			if (fieldInfo != null)
			{
				return fieldInfo.GetValue(parent);
			}
			else if (index.HasValue)
			{
				return Helper.GetValueWithIndex(parent, index.Value);
			}

			return null;
		}

		public void SetValue(object value)
		{
			if (fieldInfo != null)
			{
				fieldInfo.SetValue(parent, value);
			}
			else if (index.HasValue)
			{
				Helper.SetValueToIndex(parent, value, index.Value);
			}
		}
	}

	class Helper
	{
		public static void ResizeList(IList list, int count)
		{
			while (list.Count < count)
			{
				list.Add(CreateDefaultValue(list.GetType().GetGenericArguments()[0]));
			}

			while (list.Count > count)
			{
				list.RemoveAt(list.Count - 1);
			}
		}

		public static object GetValueWithIndex(object target, int index)
		{
			foreach (var pi in target.GetType().GetProperties())
			{
				if (pi.GetIndexParameters().Length != 1)
				{
					continue;
				}

				return pi.GetValue(target, new object[] { index });
			}
			return null;
		}

		public static bool SetValueToIndex(object target, object value, int index)
		{
			foreach (var pi in target.GetType().GetProperties())
			{
				if (pi.GetIndexParameters().Length != 1)
				{
					continue;
				}

				pi.SetValue(target, value, new object[] { index });
				return true;
			}
			return false;
		}

		public static object CreateDefaultValue(Type type)
		{
			if (type.IsValueType)
			{
				return Activator.CreateInstance(type);
			}
			else
			{
				var constructor = type.GetConstructor(new Type[] { });
				if (constructor == null)
				{
					return null;
				}

				return constructor.Invoke(null);
			}
		}
	}
}
