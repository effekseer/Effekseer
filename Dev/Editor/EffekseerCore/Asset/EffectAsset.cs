using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Effekseer.Asset
{
	class Conversion
	{
		static public int Convert(Data.Value.Int value)
		{
			var ret = 0;
			ret = value.DefaultValue;
			return ret;
		}
	}

	class VirtualPathUtility
	{
		public static string[] SplitPath(string path)
		{
			var elms = path.Split("?");
			return elms;
		}

		public static string CombinePath(string filePath, string virtualPath)
		{
			return filePath + "?" + virtualPath;
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

	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = false,
	Inherited = false)]
	public class TextAreaAttribute : Attribute
	{
		public TextAreaAttribute()
		{
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
		public PartsTreeSystem.NodeTreeAsset NodeTreeAsset { get; private set; }

		public PartsTreeSystem.NodeTree NodeTree { get; private set; }

		public PartsTreeSystem.CommandManager CommandManager { get; private set; }

		public PartsTreeSystem.NodeTreeAssetEditorProperty EditorProperty { get; private set; }

		EffectAssetEnvironment env;

		public EffectAssetEditorContext(PartsTreeSystem.NodeTreeAsset nodeTreeAsset, PartsTreeSystem.NodeTree nodeTree, PartsTreeSystem.NodeTreeAssetEditorProperty editorProperty, EffectAssetEnvironment env)
		{
			NodeTreeAsset = nodeTreeAsset;
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

		public void StartEditFields(PartsTreeSystem.IInstance editted)
		{
			CommandManager.StartEditFields(
				NodeTreeAsset,
				NodeTree,
				editted,
				env);
		}

		public void NotifyEditFields(PartsTreeSystem.IInstance editted)
		{
			CommandManager.NotifyEditFields(editted);
		}

		public void EndEditFields(PartsTreeSystem.IInstance editted)
		{
			CommandManager.EndEditFields(editted, env);
		}
	}

	public class EffectAsset
	{
		public PartsTreeSystem.NodeTreeAsset NodeTreeAsset { get; private set; }

		public List<ProceduralModelAsset> ProceduralModels { get; private set; } = new List<ProceduralModelAsset>();

		public List<DynamicEquation> DynamicEquations { get; private set; } = new List<DynamicEquation>();

		public EffectAssetEditorContext CreateEditorContext(EffectAssetEnvironment env)
		{
			var nodeTree = PartsTreeSystem.Utility.CreateNodeFromNodeTreeGroup(NodeTreeAsset, env);
			var editorProperty = new PartsTreeSystem.NodeTreeAssetEditorProperty(NodeTreeAsset, env);
			var context = new EffectAssetEditorContext(NodeTreeAsset, nodeTree, editorProperty, env);
			return context;
		}

		public void New(EffectAssetEnvironment env)
		{
			NodeTreeAsset = new PartsTreeSystem.NodeTreeAsset();
			var rootNodeId = NodeTreeAsset.Init(typeof(RootNode), env);
			NodeTreeAsset.AddNode(rootNodeId, typeof(ParticleNode), env);

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

	public class CurveAsset : PartsTreeSystem.Asset
	{

	}

	public class ProceduralModelAsset : PartsTreeSystem.Asset
	{

	}

	public class Node : PartsTreeSystem.INode
	{
		public int InstanceID { get; set; }

		public string Name = "Node";

		public bool IsRendered = true;

		[System.NonSerialized]
		public List<Node> Children = new List<Node>();

		public void AddChild(PartsTreeSystem.INode node)
		{
			Children.Add(node as Node);
			AddedNode?.Invoke(this, node as Node);
		}

		public void RemoveChild(int instanceID)
		{
			var found = Children.FirstOrDefault(_ => _.InstanceID == instanceID);

			if (found != null)
			{
				Children.Remove(found);
				RemovedNode?.Invoke(this, found);
			}
		}

		public void InsertChild(int index, PartsTreeSystem.INode node)
		{
			Children.Insert(index, node as Node);
			InsertedNode?.Invoke(this, index, node as Node);
		}

		public IReadOnlyCollection<PartsTreeSystem.INode> GetChildren()
		{
			return Children;
		}

		public event Action<Node, Node> AddedNode;

		public event Action<Node, Node> RemovedNode;

		public event Action<Node, int, Node> InsertedNode;
	}

	public class RootNode : Node
	{

	}

	public class ParticleNode : Node
	{
		public CommonParameter CommonValues = new CommonParameter();

		public DrawingParameter DrawingValues = new DrawingParameter();

		public Effect.PositionParameter PositionParam = new Effect.PositionParameter();

		public Effect.RotationParameter RotationParam = new Effect.RotationParameter();

		public Gradient GradientTest = new Gradient();

		public Vector3WithRange Vector3WithRangeTest = new Vector3WithRange();

		public TextureAsset TextureTest = new TextureAsset();

		public Color ColorTest = new Color();
	}

	public class CommonParameter
	{
		public LODParameter LodParameter = new LODParameter();
	}

	public class LODParameter
	{
		public int MatchingLODs = 0b1111;
		public Data.LODBehaviourType LodBehaviour = Data.LODBehaviourType.Hide;
	}

	public class DrawingParameter
	{
		/// <summary>
		/// Temp
		/// </summary>
		public Data.RendererValues.ParamaterType Type = Data.RendererValues.ParamaterType.Sprite;
	}

	[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field, AllowMultiple = false, Inherited = false)]
	public class FloatRangeAttribute : Attribute
	{
		public float Min { get; set; }
		public float Max { get; set; }
	}

	[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field, AllowMultiple = false, Inherited = false)]
	public class FloatStepAttribute : Attribute
	{
		public float Step { get; set; }
	}


	public class FloatWithRange
	{
		public bool IsDynamicEquationEnabled = false;
		public DynamicEquation DynamicEquationMin;
		public DynamicEquation DynamicEquationMax;

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

	public class Vector3
	{
		public bool IsDynamicEquationEnabled = false;
		public DynamicEquation DynamicEquation;
		public Vector3F Value;
	}

	public class Vector3WithRange
	{
		public bool IsDynamicEquationEnabled = false;
		public DynamicEquation DynamicEquationMin;
		public DynamicEquation DynamicEquationMax;
		public FloatWithRange X = new FloatWithRange();
		public FloatWithRange Y = new FloatWithRange();
		public FloatWithRange Z = new FloatWithRange();

		public Data.DrawnAs DrawnAs = Data.DrawnAs.CenterAndAmplitude;
	}

	public class DynamicEquation : PartsTreeSystem.Asset
	{
		public string Name = string.Empty;
		[TextArea]
		public string Code = string.Empty;
	}

	public struct Color
	{
		public int V1;
		public int V2;
		public int V3;
		public int A;

		[PartsTreeSystem.SerializeField]
		Effekseer.Data.ColorSpace colorSpace;

		public Effekseer.Data.ColorSpace ColorSpace
		{
			get
			{
				return colorSpace;
			}
			set
			{
				if (colorSpace == value)
				{
					return;
				}

				colorSpace = value;

				Utils.RGBHSVColor color;

				color.RH = V1;
				color.GS = V2;
				color.BV = V3;

				if (colorSpace == Data.ColorSpace.HSVA)
				{
					color = Utils.RGBHSVColor.RGBToHSV(color);
				}
				else
				{
					color = Utils.RGBHSVColor.HSVToRGB(color);
				}

				V1 = color.RH;
				V2 = color.GS;
				V3 = color.BV;
			}
		}

		public static bool operator ==(in Color lhs, in Color rhs)
		{
			return lhs.V1 == rhs.V1 && lhs.V2 == rhs.V2 && lhs.V3 == rhs.V3 && lhs.A == rhs.A && lhs.ColorSpace == rhs.ColorSpace;
		}

		public static bool operator !=(in Color lhs, in Color rhs)
		{
			return !(lhs == rhs);
		}

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


	public enum FCurveTimelineMode : int
	{
		[Key(key = "FcurveTimelineMode_Time")]
		Time = 0,

		[Key(key = "FcurveTimelineMode_Percent")]
		Percent = 1,
	}

	public enum FCurveEdge
	{
		[Key(key = "FcurveEdge_Constant")]
		Constant = 0,
		[Key(key = "FcurveEdge_Loop")]
		Loop = 1,
		[Key(key = "FcurveEdge_LoopInversely")]
		LoopInversely = 2,
	}

	public enum FCurveInterpolation
	{
		[Key(key = "FcurveInterpolation_Bezier")]
		Bezier = 0,
		[Key(key = "FcurveInterpolation_Linear")]
		Linear = 1,
	}


	public struct FCurveKey
	{
		public Vector2F Key;
		public Vector2F Left;
		public Vector2F Right;
		public FCurveInterpolation InterpolationType;
	}

	public class FCurve
	{
		public FCurveEdge StartType = FCurveEdge.Constant;
		public FCurveEdge EndType = FCurveEdge.Constant;
		public float OffsetMax = 0;
		public float OffsetMin = 0;
		public int Sampling = 10;
		public List<FCurveKey> Keys = new List<FCurveKey>();
		public float DefaultValue;

		public float GetValue(float frame)
		{
			var begin = StartType;
			var end = EndType;

			int lInd = 0;
			int rInd = (int)Keys.Count - 1;

			if (Keys.Count == 0)
			{
				return DefaultValue;
			}

			var length = (Keys[rInd].Key.X - Keys[lInd].Key.X);

			if (length == 0)
			{
				return Keys[lInd].Key.Y;
			}

			if (Keys[rInd].Key.X <= frame)
			{
				if (end == FCurveEdge.Constant)
				{
					return Keys[rInd].Key.Y;
				}
				else if (end == FCurveEdge.Loop)
				{
					frame = (frame - Keys[rInd].Key.X) % length + Keys[lInd].Key.X;
				}
				else if (end == FCurveEdge.LoopInversely)
				{
					frame = (length - (frame - Keys[rInd].Key.X) % length) + Keys[lInd].Key.X;
				}
			}

			if (frame <= Keys[lInd].Key.X)
			{
				if (begin == FCurveEdge.Constant)
				{
					return Keys[lInd].Key.Y;
				}
				else if (begin == FCurveEdge.Loop)
				{
					frame = (Keys[lInd].Key.X - frame) % length + Keys[lInd].Key.X;
				}
				else if (begin == FCurveEdge.LoopInversely)
				{
					frame = (length - (Keys[lInd].Key.X - frame) % length) + Keys[lInd].Key.X;
				}
			}

			while (true)
			{
				int mInd = (lInd + rInd) / 2;

				/* lIndとrIndの間 */
				if (mInd == lInd)
				{
					if (Keys[lInd].InterpolationType == FCurveInterpolation.Linear)
					{
						float subF = (float)(Keys[rInd].Key.X - Keys[lInd].Key.X);
						var subV = Keys[rInd].Key.Y - Keys[lInd].Key.Y;

						if (subF == 0) return Keys[lInd].Key.Y;

						return subV / (float)(subF) * (float)(frame - Keys[lInd].Key.X) + Keys[lInd].Key.Y;
					}
					else if (Keys[lInd].InterpolationType == FCurveInterpolation.Bezier)
					{
						if (Keys[lInd].Key.X == frame) return Keys[lInd].Key.Y;

						float[] k1 = new float[2];
						float[] k1rh = new float[2];
						float[] k2lh = new float[2];
						float[] k2 = new float[2];

						k1[0] = Keys[lInd].Key.X;
						k1[1] = Keys[lInd].Key.Y;

						k1rh[0] = Keys[lInd].Right.X;
						k1rh[1] = Keys[lInd].Right.Y;

						k2lh[0] = Keys[rInd].Left.X;
						k2lh[1] = Keys[rInd].Left.Y;

						k2[0] = Keys[rInd].Key.X;
						k2[1] = Keys[rInd].Key.Y;

						NormalizeValues(k1, k1rh, k2lh, k2);
						float t = 0;
						var getT = CalcT(frame, k1[0], k1rh[0], k2lh[0], k2[0], out t);
						if (getT)
						{
							return CalcBezierValue(k1[1], k1rh[1], k2lh[1], k2[1], t);
						}
						else
						{
							return 0;
						}
					}
				}

				if (Keys[mInd].Key.X <= frame)
				{
					lInd = mInd;
				}
				else if (frame <= Keys[mInd].Key.X)
				{
					rInd = mInd;
				}
			}
		}


		/// <summary>
		/// 三乗根を求める。
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		static float Sqrt3(float value)
		{
			if (value == 0)
			{
				return 0;
			}
			else if (value > 0)
			{
				return (float)Math.Pow(value, 1.0 / 3.0);
			}
			else
			{
				return -(float)Math.Pow(-value, 1.0 / 3.0);
			}
		}

		/// <summary>
		/// 平方根を求める。
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		static float Sqrt(float value)
		{
			return (float)Math.Sqrt(value);
		}

		/// <summary>
		/// 正しく値を求めれるように補正を行う。
		/// </summary>
		/// <param name="k1"></param>
		/// <param name="k1rh"></param>
		/// <param name="k2lh"></param>
		/// <param name="k2"></param>
		static void NormalizeValues(float[] k1, float[] k1rh, float[] k2lh, float[] k2)
		{
			var h1 = new float[2];
			var h2 = new float[2];

			// 傾きを求める
			h1[0] = k1[0] - k1rh[0];
			h1[1] = k1[1] - k1rh[1];

			h2[0] = k2[0] - k2lh[0];
			h2[1] = k2[1] - k2lh[1];

			// キーフレーム間の長さ
			var len = k2[0] - k1[0];

			// キー1の右手の長さ
			var lenR = Math.Abs(h1[0]);

			// キー2の左手の長さ
			var lenL = Math.Abs(h2[0]);

			// 長さがない
			if (lenR == 0 && lenL == 0) return;

			// 手が重なっている場合、補正
			if ((lenR + lenL) > len)
			{
				var f = len / (lenR + lenL);

				k1rh[0] = (k1[0] - f * h1[0]);
				k1rh[1] = (k1[1] - f * h1[1]);

				k2lh[0] = (k2[0] - f * h2[0]);
				k2lh[1] = (k2[1] - f * h2[1]);
			}
		}

		/// <summary>
		/// 二次方程式の解を求める。
		/// </summary>
		/// <param name="a"></param>
		/// <param name="b"></param>
		/// <param name="c"></param>
		/// <param name="r1"></param>
		/// <param name="r2"></param>
		static void QuadraticFormula(float a, float b, float c, out float r1, out float r2)
		{
			r1 = float.NaN;
			r2 = float.NaN;

			if (a != 0.0)
			{
				float p = b * b - 4 * a * c;

				if (p > 0)
				{
					p = Sqrt(p);
					r1 = (-b - p) / (2 * a);
					r2 = (-b + p) / (2 * a);
				}
				else if (p == 0)
				{
					r1 = -b / (2 * a);
				}
			}
			else if (b != 0.0)
			{
				r1 = -c / b;
			}
			else if (c == 0.0f)
			{
				r1 = 0.0f;
			}
		}

		/// <summary>
		/// 三次方程式の解を求める。
		/// </summary>
		/// <param name="frame"></param>
		/// <param name="k1X"></param>
		/// <param name="k1rhX"></param>
		/// <param name="k2lhX"></param>
		/// <param name="k2X"></param>
		/// <param name="r"></param>
		/// <returns></returns>
		static bool CalcT(float frame, float k1X, float k1rhX, float k2lhX, float k2X, out float r)
		{
			Func<float, bool> isValid = (v) =>
			{
				if (float.IsNaN(v)) return false;

				const float small = -0.00000001f;
				const float big = 1.000001f;
				return (v >= small) && (v <= big);
			};

			var c3_ = k2X - k1X + 3.0f * (k1rhX - k2lhX);
			var c2_ = 3.0f * (k1X - 2.0f * k1rhX + k2lhX);
			var c1_ = 3.0f * (k1rhX - k1X);
			var c0_ = k1X - frame;

			if (c3_ != 0.0)
			{
				var c2 = c2_ / c3_;
				var c1 = c1_ / c3_;
				var c0 = c0_ / c3_;

				var p = c1 / 3.0f - c2 * c2 / (3.0f * 3.0f);
				var q = (2.0f * c2 * c2 * c2 / (3.0f * 3.0f * 3.0f) - c2 / 3.0f * c1 + c0) / 2.0f;
				var p3q2 = q * q + p * p * p;

				if (p3q2 > 0.0)
				{
					var t = Sqrt(p3q2);
					var u = Sqrt3(-q + t);
					var v = Sqrt3(-q - t);
					r = u + v - c2 / 3.0f;

					if (isValid(r)) return true;
					return false;
				}
				else if (p3q2 == 0.0)
				{
					var u = Sqrt3(-q);
					var v = Sqrt3(-q);
					r = u + v - c2 / 3.0f;
					if (isValid(r)) return true;

					u = -Sqrt3(-q);
					v = -Sqrt3(-q);
					r = u + v - c2 / 3.0f;
					if (isValid(r)) return true;

					return false;
				}
				else
				{
					// ビエタの解
					var phi = (float)Math.Acos(-q / Sqrt(-(p * p * p)));
					var pd3 = (float)Math.Cos(phi / 3);
					var rmp = Sqrt(-p);

					r = 2.0f * rmp * (float)Math.Cos(phi / 3) - c2 / 3.0f;
					if (isValid(r)) return true;

					var q2 = Sqrt(3 - 3 * pd3 * pd3);

					//r = -rmp * (pd3 + q2) - a;
					r = 2.0f * rmp * (float)Math.Cos(phi / 3 + 2.0 * Math.PI / 3.0) - c2 / 3.0f;
					if (isValid(r)) return true;

					//r = -rmp * (pd3 - q2) - a;
					r = 2.0f * rmp * (float)Math.Cos(phi / 3 + 4.0 * Math.PI / 3.0) - c2 / 3.0f;
					if (isValid(r)) return true;
					return false;
				}
			}
			else
			{
				// 2次方程式のケース
				float r1;
				float r2;
				QuadraticFormula(c2_, c1_, c0_, out r1, out r2);

				r = r1;
				if (isValid(r)) return true;

				r = r2;
				if (isValid(r)) return true;

				r = 0.0f;
				return false;
			}
		}

		/// <summary>
		/// 三次方程式の解から値を求める。
		/// </summary>
		/// <param name="k1Y"></param>
		/// <param name="k1rhY"></param>
		/// <param name="k2lhY"></param>
		/// <param name="k2Y"></param>
		/// <param name="t"></param>
		/// <returns></returns>
		static float CalcBezierValue(float k1Y, float k1rhY, float k2lhY, float k2Y, float t)
		{
			var c0 = k1Y;
			var c1 = 3.0f * (k1rhY - k1Y);
			var c2 = 3.0f * (k1Y - 2.0f * k1rhY + k2lhY);
			var c3 = k2Y - k1Y + 3.0f * (k1rhY - k2lhY);

			return c0 + t * c1 + t * t * c2 + t * t * t * c3;
		}
	}

	public class FCurveVector3D
	{
		public FCurveTimelineMode Timeline = FCurveTimelineMode.Percent;
		public FCurve X = new FCurve();
		public FCurve Y = new FCurve();
		public FCurve Z = new FCurve();
	}
}