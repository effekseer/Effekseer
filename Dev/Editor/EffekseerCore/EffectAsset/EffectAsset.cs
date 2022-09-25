using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Effekseer.EffectAsset
{
	public class EffectAssetEnvironment : PartsTreeSystem.Environment
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

	public class EffectAssetEditorContext
	{
		public PartsTreeSystem.NodeTree NodeTree { get; private set; }

		public PartsTreeSystem.CommandManager CommandManager { get; private set; }

		public PartsTreeSystem.NodeTreeGroupEditorProperty EditorProperty { get; private set; }

		EffectAssetEnvironment env;

		public EffectAssetEditorContext(PartsTreeSystem.NodeTree nodeTree, PartsTreeSystem.NodeTreeGroupEditorProperty editorProperty, EffectAssetEnvironment env)
		{
			NodeTree = nodeTree;
			EditorProperty = editorProperty;
			this.env = env;
			CommandManager = new PartsTreeSystem.CommandManager();
		}

		public void Undo()
		{
			CommandManager.Undo(env);
			EditorProperty.Rebuild();
		}

		public void Redo()
		{
			CommandManager.Redo(env);
			EditorProperty.Rebuild();
		}
	}

	public class EffectAsset
	{
		public PartsTreeSystem.NodeTreeGroup NodeTreeGroup { get; private set; }

		public EffectAssetEditorContext CreateEditorContext(EffectAssetEnvironment env)
		{
			var nodeTree = PartsTreeSystem.Utility.CreateNodeFromNodeTreeGroup(NodeTreeGroup, env);
			var editorProperty = new PartsTreeSystem.NodeTreeGroupEditorProperty(NodeTreeGroup, env);
			var context = new EffectAssetEditorContext(nodeTree, editorProperty, env);
			return context;
		}

		public void New(EffectAssetEnvironment env)
		{
			NodeTreeGroup = new PartsTreeSystem.NodeTreeGroup();
			var rootNodeId = NodeTreeGroup.Init(typeof(RootNode), env);
			NodeTreeGroup.AddNode(rootNodeId, typeof(Node), env);
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

	public class RootNode : Node
	{

	}

	public class ParticleNode : Node
	{
		public string Name = string.Empty;

		public PositionParameter PositionParam = new PositionParameter();

		public RotationParameter RotationParam = new RotationParameter();
	}

	public class PositionParameter
	{
		public float X;
		public float Y;
		public float Z;
	}

	public class RotationParameter
	{
		public float X;
		public float Y;
		public float Z;
	}
}