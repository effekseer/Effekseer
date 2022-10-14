using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Effekseer.EffectAsset
{
	class VirtualPathUtility
	{
		public static string[] SplitPath(string path)
		{
			var elms = path.Split(":");
			return elms;
		}

		public static string CombinePath(string filePath, string virtualPath)
		{
			return filePath + ":" + virtualPath;
		}
	}

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
		Dictionary<EffectAsset, string> effectAssets = new Dictionary<EffectAsset, string>();

		Dictionary<TextureAsset, string> textureAssets = new Dictionary<TextureAsset, string>();

		public override PartsTreeSystem.Asset GetAsset(string path)
		{
			if (textureAssets.ContainsValue(path))
			{
				return textureAssets.Where(_ => _.Value == path).FirstOrDefault().Key;
			}

			var pathElms = VirtualPathUtility.SplitPath(path);
			if (pathElms.Length == 2)
			{
				if (effectAssets.ContainsValue(pathElms[0]))
				{
					return effectAssets.Where(_ => _.Value == pathElms[0]).FirstOrDefault().Key.NodeTreeAsset;
				}
			}

			if (pathElms.Length == 2)
			{
				var effectAsset = new EffectAsset();
				effectAsset.Load(pathElms[0], this);

				effectAssets.Add(effectAsset, pathElms[0]);
				return effectAsset.NodeTreeAsset;
			}

			{
				Utils.TextureInformation texInfo = new Utils.TextureInformation();
				if (texInfo.Load(path))
				{
					var asset = new TextureAsset();
					textureAssets.Add(asset, path);
					return asset;
				}
			}

			return null;
		}

		public override string GetAssetPath(PartsTreeSystem.Asset asset)
		{
			foreach (var path in textureAssets)
			{
				if (path.Key == asset)
				{
					var assetPath = System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), path.Value);
					return assetPath;
				}
			}

			foreach (var path in effectAssets)
			{
				if (path.Key.NodeTreeAsset == asset)
				{
					var assetPath = System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), path.Value);
					return VirtualPathUtility.CombinePath(assetPath, "Nodes");
				}

				for (int i = 0; i < path.Key.ProceduralModels.Count; i++)
				{
					if (path.Key.ProceduralModels[i] == asset)
					{
						var assetPath = System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), path.Value);
						return VirtualPathUtility.CombinePath(assetPath, "PM/" + i);
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

		public void Load(string text, PartsTreeSystem.Environment env)
		{
			var dict = PartsTreeSystem.JsonSerializer.Deserialize<Dictionary<string, string>>(text, env);
			RestoreFromDict(dict, env);
		}

		public string Save(PartsTreeSystem.Environment env)
		{
			var dict = StoreToDict(env);
			return PartsTreeSystem.JsonSerializer.Serialize(dict, env);
		}

		Dictionary<string, string> StoreToDict(PartsTreeSystem.Environment env)
		{
			Dictionary<string, string> dict = new();
			dict.Add("Nodes", NodeTreeAsset.Serialize(env));

			for (int i = 0; i < ProceduralModels.Count; i++)
			{
				dict.Add("PM/" + i, PartsTreeSystem.JsonSerializer.Serialize(ProceduralModels[i], env));
			}

			return dict;
		}

		void RestoreFromDict(Dictionary<string, string> dict, PartsTreeSystem.Environment env)
		{
			{
				ProceduralModels.Clear();

				int pmIndex = 0;
				while (true)
				{
					var key = "PM/" + pmIndex;

					if (dict.TryGetValue(key, out var text))
					{
						var ret = PartsTreeSystem.JsonSerializer.Deserialize<ProceduralModelAsset>(text, env);
						ProceduralModels.Add(ret);
					}
					else
					{
						break;
					}

					pmIndex++;
				}
			}

			var nodeTreeAsset = PartsTreeSystem.NodeTreeAsset.Deserialize(dict["Nodes"], env);
			NodeTreeAsset = nodeTreeAsset;
		}
	}

	public class TextureAsset : PartsTreeSystem.Asset
	{

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

		public Gradient GradientTest = new Gradient();

		public Vector3WithRange Vector3WithRangeTest = new Vector3WithRange();

		public TextureAsset TextureTest = null;
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

	public class Vector3WithRange
	{
		public FloatWithRange X = new FloatWithRange();
		public FloatWithRange Y = new FloatWithRange();
		public FloatWithRange Z = new FloatWithRange();

		public Data.DrawnAs DrawnAs = Data.DrawnAs.CenterAndAmplitude;
	}

	public class Gradient
	{
		public unsafe struct ColorMarker
		{
			public float Position;
			public float ColorR;
			public float ColorG;
			public float ColorB;
			public float Intensity;
		}

		public struct AlphaMarker
		{
			public float Position;
			public float Alpha;
		}

		public ColorMarker[] ColorMarkers;

		public AlphaMarker[] AlphaMarkers;

		public unsafe override bool Equals(object obj)
		{
			var o = (Gradient)obj;
			if (o == null)
			{
				return false;
			}

			if (ColorMarkers.Count() != o.ColorMarkers.Count() || AlphaMarkers.Count() != o.AlphaMarkers.Count())
			{
				return false;
			}

			for (int i = 0; i < ColorMarkers.Count(); i++)
			{
				if (ColorMarkers[i].ColorR != o.ColorMarkers[i].ColorR ||
					ColorMarkers[i].ColorG != o.ColorMarkers[i].ColorG ||
					ColorMarkers[i].ColorB != o.ColorMarkers[i].ColorB ||
					ColorMarkers[i].Intensity != o.ColorMarkers[i].Intensity ||
					ColorMarkers[i].Position != o.ColorMarkers[i].Position)
				{
					return false;
				}
			}

			for (int i = 0; i < AlphaMarkers.Count(); i++)
			{
				if (
					AlphaMarkers[i].Alpha != o.AlphaMarkers[i].Alpha ||
					AlphaMarkers[i].Position != o.AlphaMarkers[i].Position)
				{
					return false;
				}
			}

			return true;
		}

		public unsafe void New()
		{
			ColorMarkers = new ColorMarker[2];
			ColorMarkers[0].Position = 0;
			ColorMarkers[0].Intensity = 1;
			ColorMarkers[0].ColorR = 1.0f;
			ColorMarkers[0].ColorG = 1.0f;
			ColorMarkers[0].ColorB = 1.0f;

			ColorMarkers[1].Position = 1;
			ColorMarkers[1].Intensity = 1;
			ColorMarkers[1].ColorR = 1.0f;
			ColorMarkers[1].ColorG = 1.0f;
			ColorMarkers[1].ColorB = 1.0f;

			AlphaMarkers = new AlphaMarker[2];
			AlphaMarkers[0].Position = 0.0f;
			AlphaMarkers[0].Alpha = 1.0f;
			AlphaMarkers[1].Position = 1.0f;
			AlphaMarkers[1].Alpha = 1.0f;
		}

		public unsafe Gradient()
		{
			New();
		}
	}

}