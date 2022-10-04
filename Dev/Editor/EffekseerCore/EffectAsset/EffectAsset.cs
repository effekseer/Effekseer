using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Effekseer.EffectAsset
{
	/// <summary>
	/// A value for a selector
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = false,
	Inherited = false)]
	public class VisiblityControllerAttribute : Attribute
	{
		public int ID
		{
			get;
			set;
		}

		public VisiblityControllerAttribute()
		{
			ID = -1;
		}
	}

	/// <summary>
	/// A values which is shown or hidden with a selector
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = true,
	Inherited = false)]
	public class VisiblityControlledAttribute : Attribute
	{
		public int ID
		{
			get;
			set;
		}

		public int Value
		{
			get;
			set;
		}

		public VisiblityControlledAttribute()
		{
			ID = -1;
			Value = -1;
		}
	}

	public class EffectAssetEnvironment : PartsTreeSystem.Environment
	{
		Dictionary<EffectAsset, string> pathes = new Dictionary<EffectAsset, string>();
		public override PartsTreeSystem.Asset GetAsset(string path)
		{
			var pathElms = path.Split(":");
			if (pathElms.Length != 2)
			{
				return null;
			}

			if (pathes.ContainsValue(pathElms[0]))
			{
				return pathes.Where(_ => _.Value == pathElms[0]).FirstOrDefault().Key.NodeTreeAsset;
			}

			var effectAsset = new EffectAsset();
			effectAsset.Load(pathElms[0], this);

			pathes.Add(effectAsset, pathElms[0]);
			return effectAsset.NodeTreeAsset;
		}

		public override string GetAssetPath(PartsTreeSystem.Asset asset)
		{
			foreach (var path in pathes)
			{
				if (path.Key.NodeTreeAsset == asset)
				{
					return System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), path.Key + ":Nodes");
				}

				for (int i = 0; i < path.Key.ProceduralModels.Count; i++)
				{
					if (path.Key.ProceduralModels[i] == asset)
					{
						return System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), path.Key + ":PM/" + i);
					}
				}
			}

			return System.IO.Directory.GetCurrentDirectory();
		}
	}

	public class EffectAssetEditorContext
	{
		public PartsTreeSystem.NodeTree NodeTree { get; private set; }

		public PartsTreeSystem.CommandManager CommandManager { get; private set; }

		public PartsTreeSystem.NodeTreeAssetEditorProperty EditorProperty { get; private set; }

		EffectAssetEnvironment env;

		public EffectAssetEditorContext(PartsTreeSystem.NodeTree nodeTree, PartsTreeSystem.NodeTreeAssetEditorProperty editorProperty, EffectAssetEnvironment env)
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
		public PartsTreeSystem.NodeTreeAsset NodeTreeAsset { get; private set; }

		public List<ProceduralModelAsset> ProceduralModels { get; private set; } = new List<ProceduralModelAsset>();

		public EffectAssetEditorContext CreateEditorContext(EffectAssetEnvironment env)
		{
			var nodeTree = PartsTreeSystem.Utility.CreateNodeFromNodeTreeGroup(NodeTreeAsset, env);
			var editorProperty = new PartsTreeSystem.NodeTreeAssetEditorProperty(NodeTreeAsset, env);
			var context = new EffectAssetEditorContext(nodeTree, editorProperty, env);
			return context;
		}

		public void New(EffectAssetEnvironment env)
		{
			NodeTreeAsset = new PartsTreeSystem.NodeTreeAsset();
			var rootNodeId = NodeTreeAsset.Init(typeof(RootNode), env);
			NodeTreeAsset.AddNode(rootNodeId, typeof(Node), env);

			ProceduralModels.Clear();
		}

		public void Load(string path, PartsTreeSystem.Environment env)
		{
			var text = System.IO.File.ReadAllText(path);
			var nodeTreeAsset = PartsTreeSystem.NodeTreeAsset.Deserialize(text, env);
			NodeTreeAsset = nodeTreeAsset;

			ProceduralModels.Clear();
		}

		public void Save(string path, PartsTreeSystem.Environment env)
		{
			// Temp implementation
			var text = NodeTreeAsset.Serialize(env);
			System.IO.File.WriteAllText(path, text);
		}
	}

	public class ProceduralModelAsset : PartsTreeSystem.Asset
	{

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
		public class FixedParamater
		{
			[Key(key = "Position_FixedParamater_Location")]
			public Vector3F Location;
		}

		public class PVAParamater
		{
			[Key(key = "Position_FixedParamater_Location")]
			public FloatWithRange Location;
		}

		[VisiblityController(ID = 100)]
		public ParamaterType Type = ParamaterType.Fixed;

		[VisiblityControlled(ID = 100, Value = 0)]
		public FixedParamater Fixed = new FixedParamater();

		[VisiblityControlled(ID = 100, Value = 1)]
		public PVAParamater PVA = new PVAParamater();

		public enum ParamaterType : int
		{
			[Key(key = "Position_ParamaterType_Fixed")]
			Fixed = 0,
			[Key(key = "Position_ParamaterType_PVA")]
			PVA = 1,
			[Key(key = "Position_ParamaterType_Easing")]
			Easing = 2,
			[Key(key = "Position_ParamaterType_LocationFCurve")]
			LocationFCurve = 3,
			[Key(key = "Position_ParameterType_NurbsCurve")]
			NurbsCurve = 4,
			[Key(key = "Position_ParameterType_ViewOffset")]
			ViewOffset = 5,
		}
	}

	public class RotationParameter
	{
		public float X;
		public float Y;
		public float Z;

		public FloatWithRange Range = new FloatWithRange();
	}

	public class FloatWithRange
	{
		public float Max = float.MaxValue;
		public float Min = float.MinValue;
		public Data.DrawnAs DrawnAs = Data.DrawnAs.CenterAndAmplitude;

		public float Center
		{
			get
			{
				return (Max + Min) / 2;
			}
			set
			{
				var amplitude = Amplitude;
				Max = value + amplitude;
				Min = value - amplitude;
			}
		}

		public float Amplitude
		{
			get
			{
				return Max - Center;
			}
			set
			{
				var center = Center;
				Max = center + value;
				Min = center - value;
			}
		}
	}
}