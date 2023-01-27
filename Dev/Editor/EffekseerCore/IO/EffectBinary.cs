using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Effekseer.IO
{
	public class EffectBinary
	{
		public enum Version
		{
			Ver18 = 1800,
			Latest = Ver18,
		}

		public byte[] Buffer;

		public bool Init(Asset.EffectAsset effectAsset, Asset.EffectAssetEnvironment env, float magnification)
		{
			var nodeTree = PartsTreeSystem.Utility.CreateNodeFromNodeTreeGroup(effectAsset.NodeTreeAsset, env);

			var dependencies = CollectDependencies(nodeTree, env);

			List<FB.Effect.TexturePropertyT> textureProps = new();
			foreach (var texture in dependencies.Textures)
			{
				var textureProp = new FB.Effect.TexturePropertyT();
				textureProp.Path = texture;
				textureProps.Add(textureProp);
			}


			FB.Effect.NodeT exportNode(Asset.Node node)
			{
				var ret = new FB.Effect.NodeT();
				ret.Children = new List<FB.Effect.NodeT>();

				foreach (var child in node.Children)
				{
					ret.Children.Add(exportNode(child));
				}

				if (node is Asset.ParticleNode pn)
				{
					ret.BasicParameter = GenerateDummyBasicSettings();
					ret.PositionParameter = ToFB(effectAsset, pn.PositionParam);
					if (pn.DrawingValues.Type == Data.RendererValues.ParamaterType.Sprite)
					{
						ret.Type = FB.Effect.EffectNodeType.EffectNodeType_Sprite;
					}
				}
				else if (node is Asset.RootNode rn)
				{
					ret.Type = FB.Effect.EffectNodeType.EffectNodeType_Root;
				}

				return ret;
			}

			var effectT = new FB.Effect.EffectAssetT();
			effectT.Textures = textureProps;
			effectT.RootNode = exportNode(nodeTree.Root as Asset.Node);

			List<byte[]> data = new List<byte[]>();
			data.Add(Encoding.UTF8.GetBytes("SKFE"));
			data.Add(BitConverter.GetBytes((int)Version.Latest));
			data.Add(effectT.SerializeToBinary());

			Buffer = data.SelectMany(_ => _).ToArray();

			return true;
		}


		Effekseer.FB.Effect.BasicParameterT GenerateDummyBasicSettings()
		{
			var ret = new FB.Effect.BasicParameterT();
			ret.MaxGeneration.RefEq = -1;
			ret.MaxGeneration.Value = 1;
			ret.Life = new FB.IntRangeT { RefEq = new FB.RefMinMaxT { Min = -1, Max = -1 }, Max = 100, Min = 100 };
			ret.GenerationTime = new FB.FloatRangeT { RefEq = new FB.RefMinMaxT { Min = -1, Max = -1 }, Min = 10, Max = 100 };
			ret.GenerationTimeOffset = new FB.FloatRangeT { RefEq = new FB.RefMinMaxT { Min = -1, Max = -1 }, Min = 10, Max = 100 };
			ret.TranslationBindType = FB.Effect.TranslationParentBindType.TranslationParentBindType_Always;
			ret.RotationBindType = FB.Effect.BindType.BindType_Always;
			ret.ScalingBindType = FB.Effect.BindType.BindType_Always;

			return ret;
		}

		Effekseer.FB.Effect.PositionParameterT ToFB(Asset.EffectAsset effectAsset, Asset.Effect.PositionParameter positionParam)
		{
			var ret = new Effekseer.FB.Effect.PositionParameterT();
			ret.Type = (FB.Effect.PositionType)positionParam.Type;
			if (positionParam.Type == Asset.Effect.PositionParameter.ParamaterType.Fixed)
			{
				var param = positionParam.Fixed;
				var dst = new FB.Effect.PositionParameter_FixedT();
				dst.RefEq = effectAsset.DynamicEquations.IndexOf(param.Location.DynamicEquation);
				dst.Value = new FB.Vec3FT { X = param.Location.Value.X, Y = param.Location.Value.Y, Z = param.Location.Value.Z };
				ret.Fixed = dst;
			}
			else if (positionParam.Type == Asset.Effect.PositionParameter.ParamaterType.PVA)
			{
				var param = positionParam.PVA;
				var dst = new FB.Effect.PositionParameter_PVAT();
				dst.Pos = ToFB(effectAsset, param.Location);
				dst.Vel = ToFB(effectAsset, param.Velocity);
				dst.Acc = ToFB(effectAsset, param.Acceleration);
				ret.Pva = dst;
			}
			else if (positionParam.Type == Asset.Effect.PositionParameter.ParamaterType.Easing)
			{
				var param = positionParam.Easing;
				var dst = new FB.Effect.PositionParameter_EasingT();

				var easing = new FB.Vec3FEasingT();

				easing.Start = ToFB(effectAsset, param.Start);
				easing.End = ToFB(effectAsset, param.End);
				easing.Middle = ToFB(effectAsset, param.Middle);

				float[] easingParams = null;
				if (param.Type == Asset.EasingType.LeftRightSpeed)
				{
					easingParams = Utils.MathUtl.Easing((float)param.StartSpeed, (float)param.EndSpeed);
				}

				int channel = 0;
				if (param.IsRandomGroupEnabled)
				{
					Dictionary<int, int> id2ind = new Dictionary<int, int>();

					var ids = new[] { param.RandomGroupX, param.RandomGroupY, param.RandomGroupZ };

					foreach (var id in ids)
					{
						if (!id2ind.ContainsKey(id))
						{
							id2ind.Add(id, id2ind.Count);
						}
					}

					channel += (byte)id2ind[param.RandomGroupX] << 0;
					channel += (byte)id2ind[param.RandomGroupY] << 8;
					channel += (byte)id2ind[param.RandomGroupZ] << 16;

				}
				else
				{
					channel += (byte)0 << 0;
					channel += (byte)1 << 8;
					channel += (byte)2 << 16;
				}

				List<FB.Easing3Type> types = new List<FB.Easing3Type>();
				if (param.IsIndividualTypeEnabled)
				{
					types.Add((FB.Easing3Type)param.TypeX);
					types.Add((FB.Easing3Type)param.TypeY);
					types.Add((FB.Easing3Type)param.TypeZ);
				}

				easing.Params = easingParams.ToList();
				easing.Type = (FB.Easing3Type)param.Type;
				easing.Channel = channel;
				easing.IsMiddleEnabled = param.IsMiddleEnabled;
				easing.IsIndividualEnabled = param.IsIndividualTypeEnabled;
				easing.Types = types;
				dst.Location = easing;
			}
			if (positionParam.Type == Asset.Effect.PositionParameter.ParamaterType.LocationFCurve)
			{
				var param = positionParam.LocationFCurve;
				var dst = new FB.Effect.PositionParameter_FCurveT();
				FB.FCurveGroupT fcurve = new FB.FCurveGroupT();

				fcurve.Timeline = (FB.FCurveTimelineType)param.Timeline;
				fcurve.Curves.Add(ToFB(param.X));
				fcurve.Curves.Add(ToFB(param.Y));
				fcurve.Curves.Add(ToFB(param.Z));

				dst.Fcurve = fcurve;
				ret.Fcurve = dst;
			}
			else if (positionParam.Type == Asset.Effect.PositionParameter.ParamaterType.ViewOffset)
			{
				var param = positionParam.ViewOffset;
				var dst = new FB.Effect.PositionParameter_ViewOffsetT();

				dst.Distance = ToFB(effectAsset, param.Distance);
				ret.ViewOffset = dst;
			}
			else if (positionParam.Type == Asset.Effect.PositionParameter.ParamaterType.NurbsCurve)
			{
				var param = positionParam.NurbsCurve;
				var dst = new FB.Effect.PositionParameter_NurbsCurveT();

				dst.Index = -1;
				dst.LoopType = (int)param.LoopType;
				dst.Scale = param.Scale;
				dst.MoveSpeed = param.MoveSpeed;
				ret.NurbsCurve = dst;
			}

			return ret;
		}

		Effekseer.FB.FCurveT ToFB(Asset.FCurve fcurve)
		{
			FB.FCurveT curve = new FB.FCurveT();

			var len = fcurve.Keys.Last().Key.X - fcurve.Keys.First().Key.X;

			curve.Start = (Effekseer.FB.FCurveEdgeType)fcurve.StartType;
			curve.End = (Effekseer.FB.FCurveEdgeType)fcurve.EndType;
			curve.Freq = fcurve.Sampling;
			curve.Len = (int)(fcurve.Keys.Last().Key.X - fcurve.Keys.First().Key.X);
			curve.OffsetMin = fcurve.OffsetMin;
			curve.OffsetMax = fcurve.OffsetMax;

			//??? TODO check
			curve.Offset = (int)fcurve.DefaultValue;

			if (fcurve.Keys.Count > 0)
			{
				for (var f = fcurve.Keys.First().Key.X; f < fcurve.Keys.Last().Key.X; f += curve.Freq)
				{
					curve.Keys.Add(fcurve.GetValue(f));
				}

				{
					curve.Keys.Add(fcurve.GetValue(fcurve.GetValue(fcurve.Keys.Last().Key.X)));
				}
			}

			return curve;
		}

		Effekseer.FB.Vec3FRangeT ToFB(Asset.EffectAsset effectAsset, Asset.Vector3WithRange param)
		{
			int refMin = -1;
			int refMax = -1;
			if (param.IsDynamicEquationEnabled)
			{
				refMin = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMin);
				refMax = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMax);
			}

			var ret = new FB.Vec3FRangeT();
			ret.RefEq.Min = refMin;
			ret.RefEq.Max = refMax;

			ret.Min.X = param.X.Min;
			ret.Max.X = param.X.Max;
			ret.Min.Y = param.Y.Min;
			ret.Max.Y = param.Y.Max;
			ret.Min.Z = param.Z.Min;
			ret.Max.Z = param.Z.Max;

			return ret;
		}

		Effekseer.FB.FloatRangeT ToFB(Asset.EffectAsset effectAsset, Asset.FloatWithRange param)
		{
			int refMin = -1;
			int refMax = -1;
			if (param.IsDynamicEquationEnabled)
			{
				refMin = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMin);
				refMax = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMax);
			}

			var ret = new Effekseer.FB.FloatRangeT();
			ret.RefEq.Min = refMin;
			ret.RefEq.Max = refMax;
			ret.Min = param.Min;
			ret.Max = param.Max;
			return ret;
		}

		class DependencyProperty
		{
			public List<string> Textures = new List<string>();
			public List<string> Curves = new List<string>();
		}

		DependencyProperty CollectDependencies(PartsTreeSystem.NodeTree nodeTree, Asset.EffectAssetEnvironment env)
		{
			var textures = new SortedSet<string>();
			var curves = new SortedSet<string>();

			void collect(Asset.Node node)
			{
				if (node is Asset.ParticleNode pn)
				{
					if (pn.TextureTest != null)
					{
						var path = env.GetAssetPath(pn.TextureTest);
						textures.Add(path);
					}

					if (pn.PositionParam.Type == Asset.Effect.PositionParameter.ParamaterType.NurbsCurve)
					{
						var path = env.GetAssetPath(pn.PositionParam.NurbsCurve.FilePath);
						curves.Add(path);
					}
				}

				foreach (var child in node.Children)
				{
					collect(child);
				}
			}

			collect(nodeTree.Root as Asset.Node);

			var ret = new DependencyProperty();
			ret.Textures = textures.OrderBy(_ => _).ToList();

			return ret;
		}
	}
}