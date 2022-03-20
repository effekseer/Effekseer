using System.Collections.Generic;
using System.Linq;
using Effekseer.Data;
using Effekseer.Data.Value;
using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal sealed class MaterialSerializer
	{
		private void AddTextureInfo(ExporterVersion version,
			SortedDictionary<string, int> texAndInd,
			TextureValuesAggregator aggregator)
		{
			if (version < ExporterVersion.Ver16Alpha1)
			{
				return;
			}

			aggregator.AddAlphaTexture(texAndInd);
			aggregator.AddUvDistortionTexture(texAndInd);

			if (aggregator.IsBlendTextureEnabled)
			{
				aggregator.AddBlendTexture(texAndInd);
				aggregator.AddBlendAlphaTexture(texAndInd);
				aggregator.AddBlendUvDistortionTexture(texAndInd);
			}
			else
			{
				aggregator.AddInt(-1);
				aggregator.AddInt(-1);
				aggregator.AddInt(-1);
			}
		}

		private void AddAsDefaultMaterial(ExporterVersion version,
			SortedDictionary<string, int> texToIndex,
			Data.RendererCommonValues value,
			TextureValuesAggregator aggregator)
		{
			aggregator.AddTexIdAndStoreSize(value.ColorTexture, 1, texToIndex);
			aggregator.AddInt(-1);
			AddTextureInfo(version, texToIndex, aggregator);
		}

		private void AddAsBackDistortion(ExporterVersion version,
			SortedDictionary<string, int> distortionToIndex,
			Data.RendererCommonValues value,
			TextureValuesAggregator aggregator)
		{
			aggregator.AddTexIdAndStoreSize(value.ColorTexture, 1, distortionToIndex);
			aggregator.AddInt(-1);
			AddTextureInfo(version, distortionToIndex, aggregator);
		}

		private void AddAsLighting(ExporterVersion version,
			SortedDictionary<string, int> texToIndex,
			SortedDictionary<string, int> normalTexToIndex,
			Data.RendererCommonValues value,
			TextureValuesAggregator aggregator)
		{
			aggregator.AddTexIdAndStoreSize(value.ColorTexture, 1, texToIndex);
			aggregator.AddTexIdAndStoreSize(value.NormalTexture, 2, normalTexToIndex);
			AddTextureInfo(version, texToIndex, aggregator);
		}

		private void AddAsFile(
			ExporterVersion version,
			TextureValuesAggregator aggregator,
			Data.RendererCommonValues value,
			SortedDictionary<string, int> texToIndex,
			SortedDictionary<string, int> normalToIndex,
			SortedDictionary<string, int> materialToIndex)
		{
			var materialInfo = Core.ResourceCache.LoadMaterialInformation(value.MaterialFile.Path.AbsolutePath) ??
				new MaterialInformation();
			var textures = value.MaterialFile.GetTextures(materialInfo).Where(_ => _.Item1 != null).ToArray();
			var uniforms = value.MaterialFile.GetUniforms(materialInfo);

			// maximum slot limitation
			if (textures.Length > Constant.UserTextureSlotCount)
			{
				textures = textures.Take(Constant.UserTextureSlotCount).ToArray();
			}

			aggregator.AddInt(materialToIndex.ContainsKey(value.MaterialFile.Path.RelativePath)
				? materialToIndex[value.MaterialFile.Path.RelativePath]
				: (-1));

			aggregator.AddInt(textures.Length);

			foreach (var texture in textures)
			{
				var texture_ = texture.Item1.Value as Data.Value.PathForImage;
				if (texture.Item2.Type == TextureType.Value)
				{
					aggregator.AddInt(1);
					aggregator.AddTexIdAndStoreSize(texture_, texture.Item2.Priority, normalToIndex);
				}
				else
				{
					aggregator.AddInt(0);
					aggregator.AddTexIdAndStoreSize(texture_, texture.Item2.Priority, texToIndex);
				}
			}

			aggregator.AddInt(uniforms.Count);

			foreach (var uniform in uniforms)
			{
				float[] floats = GetUniformsVertexes(uniform);

				aggregator.AddFloat(floats[0]);
				aggregator.AddFloat(floats[1]);
				aggregator.AddFloat(floats[2]);
				aggregator.AddFloat(floats[3]);
			}

			if (version >= ExporterVersion.Ver17Alpha3)
			{
				var gradients = value.MaterialFile.GetGradients(materialInfo);

				aggregator.AddInt(gradients.Count);

				foreach (var gradient in gradients)
				{
					var gradientValue = gradient.Item1.Value as Data.Value.Gradient;
					aggregator.AddData(gradientValue.ToBinary());
				}
			}
		}

		public void AddMaterialData(ExporterVersion version,
			Data.RendererCommonValues value,
			TextureValuesAggregator aggregator,
			SortedDictionary<string, int> texToIndex,
			SortedDictionary<string, int> distortionToIndex,
			SortedDictionary<string, int> normalToIndex,
			SortedDictionary<string, int> materialToIndex)
		{
			switch (value.Material.Value)
			{
				case Data.RendererCommonValues.MaterialType.Default:
					AddAsDefaultMaterial(version, texToIndex, value, aggregator);
					break;
				case Data.RendererCommonValues.MaterialType.BackDistortion:
					AddAsBackDistortion(version, distortionToIndex, value, aggregator);
					break;
				case Data.RendererCommonValues.MaterialType.Lighting:
					AddAsLighting(version, texToIndex, normalToIndex, value, aggregator);
					break;
				default:
					AddAsFile(version, aggregator, value, texToIndex, normalToIndex, materialToIndex);
					break;
			}
		}

		static float[] GetUniformsVertexes(Tuple35<MaterialFileParameter.ValueStatus, MaterialInformation.UniformInformation> tuple35)
		{
			float[] floats1 = new float[4];

			if (tuple35.Item1 == null)
			{
				floats1 = tuple35.Item2.DefaultValues.ToArray();
			}
			else if (tuple35.Item1.Value is Float float1)
			{
				floats1[0] = float1.Value;
			}
			else if (tuple35.Item1.Value is Vector2D float2)
			{
				floats1[0] = float2.X.Value;
				floats1[1] = float2.Y.Value;
			}
			else if (tuple35.Item1.Value is Data.Value.Vector3D float3)
			{
				floats1[0] = float3.X.Value;
				floats1[1] = float3.Y.Value;
				floats1[2] = float3.Z.Value;
			}
			else if (tuple35.Item1.Value is Vector4D float4)
			{
				floats1[0] = float4.X.Value;
				floats1[1] = float4.Y.Value;
				floats1[2] = float4.Z.Value;
				floats1[3] = float4.W.Value;
			}

			return floats1;
		}
	}
}