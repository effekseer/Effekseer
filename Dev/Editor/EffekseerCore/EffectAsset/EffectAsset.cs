using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Effekseer.EffectAsset
{
	class EffectAsset
	{
		public Node Root = new RootNode();

		public void New()
		{
			Root = new RootNode();
			Root.AddChild(new ParticleNode());
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