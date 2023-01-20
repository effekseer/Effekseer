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


		public bool Init(EffectAsset.EffectAsset effectAsset, EffectAsset.EffectAssetEnvironment env, float magnification)
		{
			var nodeTree = PartsTreeSystem.Utility.CreateNodeFromNodeTreeGroup(effectAsset.NodeTreeAsset, env);

			var dependencies = CollectDependencies(nodeTree, env);

			var fbb = new FlatBuffers.FlatBufferBuilder(1);

			List<FlatBuffers.Offset<FB.TextureProperty>> textureProps = new();
			foreach (var texture in dependencies.Textures)
			{
				var texturePath = fbb.CreateString(texture);
				FB.TextureProperty.StartTextureProperty(fbb);
				FB.TextureProperty.AddPath(fbb, texturePath);
				var offset = FB.TextureProperty.EndTextureProperty(fbb);
				FB.TextureProperty.EndTextureProperty(fbb);
				textureProps.Add(offset);
			}

			FlatBuffers.Offset<FB.Node> exportNode(EffectAsset.Node node)
			{
				List<FlatBuffers.Offset<FB.Node>> childrenOffsets = new();

				foreach (var child in node.Children)
				{
					childrenOffsets.Add(exportNode(child));
				}

				var childrenOffset = FB.Node.CreateChildrenVector(fbb, childrenOffsets.ToArray());

				if (node is EffectAsset.ParticleNode pn)
				{
					var bas = ExportBasicSettings(fbb);
					var pos = ExportPositionSettings(fbb, effectAsset, pn.PositionParam);

					FB.Node.StartNode(fbb);
					FB.Node.AddBasicSettings(fbb, bas);
					FB.Node.AddPositionSettings(fbb, pos);
					if (pn.DrawingValues.Type == Data.RendererValues.ParamaterType.Sprite)
					{
						FB.Node.AddType(fbb, FB.EffectNodeType.EffectNodeType_Sprite);
					}

					FB.Node.AddChildren(fbb, childrenOffset);
					return FB.Node.EndNode(fbb);
				}
				else if (node is EffectAsset.RootNode rn)
				{
					FB.Node.StartNode(fbb);
					FB.Node.AddType(fbb, FB.EffectNodeType.EffectNodeType_Root);
					FB.Node.AddChildren(fbb, childrenOffset);
					return FB.Node.EndNode(fbb);
				}

				throw new Exception();
			}


			var texturesOffset = FB.Effect.CreateTexturesVector(fbb, textureProps.ToArray());
			var rootNodeOffset = exportNode(nodeTree.Root as EffectAsset.Node);

			FB.Effect.StartEffect(fbb);
			FB.Effect.AddTextures(fbb, texturesOffset);
			FB.Effect.AddRootNode(fbb, rootNodeOffset);
			var effect = FB.Effect.EndEffect(fbb);
			FB.Effect.FinishEffectBuffer(fbb, effect);

			List<byte[]> data = new List<byte[]>();
			data.Add(Encoding.UTF8.GetBytes("SKFE"));
			data.Add(BitConverter.GetBytes((int)Version.Latest));
			data.Add(fbb.DataBuffer.ToSizedArray());

			Buffer = data.SelectMany(_ => _).ToArray();

			return true;
		}

		FlatBuffers.Offset<Effekseer.FB.BasicSettings> ExportBasicSettings(FlatBuffers.FlatBufferBuilder fbb)
		{
			// TODO
			FB.BasicSettings.StartBasicSettings(fbb);
			FB.BasicSettings.AddMaxGeneration(fbb, 1);
			FB.BasicSettings.AddLife(fbb, FB.IntRange.CreateIntRange(fbb, 100, 100));
			FB.BasicSettings.AddGenerationTime(fbb, FB.FloatRange.CreateFloatRange(fbb, -1, -1, 0, 100));
			FB.BasicSettings.AddGenerationTimeOffset(fbb, FB.FloatRange.CreateFloatRange(fbb, -1, -1, 0, 0));
			return FB.BasicSettings.EndBasicSettings(fbb);
		}

		FlatBuffers.Offset<Effekseer.FB.PositionSettings> ExportPositionSettings(FlatBuffers.FlatBufferBuilder fbb, EffectAsset.EffectAsset effectAsset, Effekseer.EffectAsset.PositionParameter positionParam)
		{
			// TODO
			FlatBuffers.Offset<FB.PositionSettings_Fixed> fixedOffset = new FlatBuffers.Offset<FB.PositionSettings_Fixed>();
			FlatBuffers.Offset<FB.PositionSettings_PVA> pvaOffset = new FlatBuffers.Offset<FB.PositionSettings_PVA>();

			if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.Fixed)
			{
				var param = positionParam.Fixed;
				FB.PositionSettings_Fixed.StartPositionSettings_Fixed(fbb);
				FB.PositionSettings_Fixed.AddRefEq(fbb, effectAsset.DynamicEquations.IndexOf(param.Location.DynamicEquation));
				FB.PositionSettings_Fixed.AddValue(fbb, FB.Vec3F.CreateVec3F(fbb, positionParam.Fixed.Location.Value.X, positionParam.Fixed.Location.Value.Y, positionParam.Fixed.Location.Value.Z));
				fixedOffset = FB.PositionSettings_Fixed.EndPositionSettings_Fixed(fbb);
			}
			else if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.PVA)
			{
				var param = positionParam.PVA;
				var location = ExportVec3FRange(fbb, effectAsset, param.Location);
				var velocity = ExportVec3FRange(fbb, effectAsset, param.Velocity);
				var acc = ExportVec3FRange(fbb, effectAsset, param.Acceleration);

				FB.PositionSettings_PVA.StartPositionSettings_PVA(fbb);
				FB.PositionSettings_PVA.AddPos(fbb, location);
				FB.PositionSettings_PVA.AddVel(fbb, velocity);
				FB.PositionSettings_PVA.AddAcc(fbb, acc);
				pvaOffset = FB.PositionSettings_PVA.EndPositionSettings_PVA(fbb);
			}
			else if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.Easing)
			{
				// TODO
				var param = positionParam.Easing;

				var start = ExportVec3FRange(fbb, effectAsset, param.Start);
				var end = ExportVec3FRange(fbb, effectAsset, param.End);
				var middle = ExportVec3FRange(fbb, effectAsset, param.Middle);

				float[] easingParams = null;
				if (param.Type == EffectAsset.EasingType.LeftRightSpeed)
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

				FB.EasingVec3F.StartEasingVec3F(fbb);
				FB.EasingVec3F.AddStart(fbb, start);
				FB.EasingVec3F.AddEnd(fbb, end);
				FB.EasingVec3F.AddMiddle(fbb, middle);
				FB.EasingVec3F.AddParams(fbb, FB.EasingVec3F.CreateParamsVector(fbb, easingParams));
				FB.EasingVec3F.AddType(fbb, (FB.Easing3Type)param.Type);
				FB.EasingVec3F.AddChannel(fbb, channel);
				FB.EasingVec3F.AddIsMiddleEnabled(fbb, param.IsMiddleEnabled);
				FB.EasingVec3F.AddIsIndividualEnabled(fbb, param.IsIndividualTypeEnabled);
				FB.EasingVec3F.AddTypes(fbb, FB.EasingVec3F.CreateTypesVector(fbb, types.ToArray()));
				var easing = FB.EasingVec3F.EndEasingVec3F(fbb);

				FB.PositionSettings_Easing.StartPositionSettings_Easing(fbb);
				FB.PositionSettings_Easing.AddLocation(fbb, easing);
				FB.PositionSettings_Easing.EndPositionSettings_Easing(fbb);
			}
			else if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.LocationFCurve)
			{
				// TODO
				var param = positionParam.LocationFCurve;

				//param.FCurve.X.
			}
			else if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.ViewOffset)
			{
				// TODO
				var param = positionParam.ViewOffset;
				var distance = ExportFloatRange(fbb, effectAsset, param.Distance);

				FB.PositionSettings_ViewOffset.StartPositionSettings_ViewOffset(fbb);
				FB.PositionSettings_ViewOffset.AddDistance(fbb, distance);
				FB.PositionSettings_ViewOffset.EndPositionSettings_ViewOffset(fbb);
			}
			else if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.NurbsCurve)
			{
				// TODO
				var param = positionParam.NurbsCurve;

				FB.PositionSettings_NurbsCurve.StartPositionSettings_NurbsCurve(fbb);

				// TODO
				FB.PositionSettings_NurbsCurve.AddIndex(fbb, -1);
				FB.PositionSettings_NurbsCurve.AddLoopType(fbb, (int)param.LoopType);
				FB.PositionSettings_NurbsCurve.AddScale(fbb, param.Scale);
				FB.PositionSettings_NurbsCurve.AddMoveSpeed(fbb, param.MoveSpeed);

				FB.PositionSettings_NurbsCurve.EndPositionSettings_NurbsCurve(fbb);
			}

			FB.PositionSettings.StartPositionSettings(fbb);
			FB.PositionSettings.AddType(fbb, (FB.PositionType)positionParam.Type);

			if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.Fixed)
			{
				FB.PositionSettings.AddFixed(fbb, fixedOffset);
			}
			else if (positionParam.Type == EffectAsset.PositionParameter.ParamaterType.PVA)
			{
				FB.PositionSettings.AddPva(fbb, pvaOffset);
			}

			return FB.PositionSettings.EndPositionSettings(fbb);
		}

		FlatBuffers.Offset<Effekseer.FB.FloatRange> ExportFloatRange(FlatBuffers.FlatBufferBuilder fbb, EffectAsset.EffectAsset effectAsset, EffectAsset.FloatWithRange param)
		{
			int refMin = -1;
			int refMax = -1;
			if (param.IsDynamicEquationEnabled)
			{
				refMin = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMin);
				refMax = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMax);
			}

			return Effekseer.FB.FloatRange.CreateFloatRange(fbb, refMin, refMax, param.Min, param.Max);
		}

		FlatBuffers.Offset<Effekseer.FB.Vec3FRange> ExportVec3FRange(FlatBuffers.FlatBufferBuilder fbb, EffectAsset.EffectAsset effectAsset, EffectAsset.Vector3WithRange param)
		{
			int refMin = -1;
			int refMax = -1;
			if (param.IsDynamicEquationEnabled)
			{
				refMin = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMin);
				refMax = effectAsset.DynamicEquations.IndexOf(param.DynamicEquationMax);
			}

			return Effekseer.FB.Vec3FRange.CreateVec3FRange(
				fbb,
				refMin,
				refMax,
				param.X.Min,
				param.Y.Min,
				param.Z.Min,
				param.X.Max,
				param.Y.Max,
				param.Z.Max);
		}

		class DependencyProperty
		{
			public List<string> Textures = new List<string>();
			public List<string> Curves = new List<string>();
		}

		DependencyProperty CollectDependencies(PartsTreeSystem.NodeTree nodeTree, EffectAsset.EffectAssetEnvironment env)
		{
			var textures = new SortedSet<string>();
			var curves = new SortedSet<string>();

			void collect(EffectAsset.Node node)
			{
				if (node is EffectAsset.ParticleNode pn)
				{
					if (pn.TextureTest != null)
					{
						var path = env.GetAssetPath(pn.TextureTest);
						textures.Add(path);
					}

					if (pn.PositionParam.Type == EffectAsset.PositionParameter.ParamaterType.NurbsCurve)
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

			collect(nodeTree.Root as EffectAsset.Node);

			var ret = new DependencyProperty();
			ret.Textures = textures.OrderBy(_ => _).ToList();

			return ret;
		}
	}
}