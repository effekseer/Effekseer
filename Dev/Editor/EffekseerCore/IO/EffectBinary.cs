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

				FB.BasicSettings.StartBasicSettings(fbb);
				FB.BasicSettings.AddMaxGeneration(fbb, 1);
				FB.BasicSettings.AddLife(fbb, FB.IntRange.CreateIntRange(fbb, 100, 100));
				FB.BasicSettings.AddGenerationTime(fbb, FB.FloatRange.CreateFloatRange(fbb, 0, 100));
				FB.BasicSettings.AddGenerationTimeOffset(fbb, FB.FloatRange.CreateFloatRange(fbb, 0, 0));
				var bas = FB.BasicSettings.EndBasicSettings(fbb);


				FB.Node.StartNode(fbb);

				if (node is EffectAsset.ParticleNode pn)
				{
					FB.Node.AddBasicSettings(fbb, bas);
					if (pn.DrawingValues.Type == Data.RendererValues.ParamaterType.Sprite)
					{
						FB.Node.AddType(fbb, FB.EffectNodeType.EffectNodeType_Sprite);
					}
				}
				else if (node is EffectAsset.RootNode rn)
				{
					FB.Node.AddType(fbb, FB.EffectNodeType.EffectNodeType_Root);
				}

				FB.Node.AddChildren(fbb, childrenOffset);

				return FB.Node.EndNode(fbb);
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

		class DependencyProperty
		{
			public List<string> Textures = new List<string>();
		}

		DependencyProperty CollectDependencies(PartsTreeSystem.NodeTree nodeTree, EffectAsset.EffectAssetEnvironment env)
		{
			var textures = new SortedSet<string>();

			void collect(EffectAsset.Node node)
			{
				if (node is EffectAsset.ParticleNode pn)
				{
					if (pn.TextureTest != null)
					{
						var path = env.GetAssetPath(pn.TextureTest);
						textures.Add(path);
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