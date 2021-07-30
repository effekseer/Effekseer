using System;
using System.Collections.Generic;
using Effekseer.Binary.RenderData;
using Effekseer.Data;
using Effekseer.Data.Group;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class RendererCommonValues
	{
		private static readonly MaterialSerializer MaterialSerializerInstance = new MaterialSerializer();

		public static byte[] GetBytes(Data.RendererCommonValues value,
			Data.AdvancedRenderCommonValues advanceValue,
			SortedDictionary<string, int> texture_and_index,
			SortedDictionary<string, int> normalTexture_and_index,
			SortedDictionary<string, int> distortionTexture_and_index,
			SortedDictionary<string, int> material_and_index,
			ExporterVersion version)
		{
			List<byte[]> data = new List<byte[]>();

			var texInfoRepo = new TextureInformationRepository();
			var texInfo = texInfoRepo.Texture;

			data.Add(((int)value.Material.Value).GetBytes());

			if (version >= ExporterVersion.Ver16Alpha1)
			{
				if (value.Material.Value == Data.RendererCommonValues.MaterialType.Default ||
				value.Material.Value == Data.RendererCommonValues.MaterialType.Lighting)
				{
					data.Add(BitConverter.GetBytes(value.EmissiveScaling));
				}
			}

			data.AddRange(GetTextureValues(value,
				advanceValue,
				texture_and_index,
				normalTexture_and_index,
				distortionTexture_and_index,
				material_and_index,
				version,
				texInfoRepo));

			data.Add(value.AlphaBlend);
			data.Add(value.Filter);
			data.Add(value.Wrap);

			data.Add(value.Filter2);
			data.Add(value.Wrap2);

			if (version >= ExporterVersion.Ver16Alpha1)
			{
				data.Add(advanceValue.AlphaTextureParam.Filter);
				data.Add(advanceValue.AlphaTextureParam.Wrap);

				data.Add(advanceValue.UVDistortionTextureParam.Filter);
				data.Add(advanceValue.UVDistortionTextureParam.Wrap);

				data.Add(advanceValue.BlendTextureParams.BlendTextureParam.Filter);
				data.Add(advanceValue.BlendTextureParams.BlendTextureParam.Wrap);

				data.Add(advanceValue.BlendTextureParams.BlendAlphaTextureParam.Filter);
				data.Add(advanceValue.BlendTextureParams.BlendAlphaTextureParam.Wrap);

				data.Add(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Filter);
				data.Add(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Wrap);
			}

			data.Add(value.ZTest.GetValue() ? 1.GetBytes() : 0.GetBytes());
			data.Add(value.ZWrite.GetValue() ? 1.GetBytes() : 0.GetBytes());

			data.Add(value.FadeInType);
			if (value.FadeInType.Value == Data.RendererCommonValues.FadeType.Use)
			{
				data.Add(value.FadeIn.Frame.GetBytes());

				var easing = Utl.MathUtl.Easing((float)value.FadeIn.StartSpeed.Value, (float)value.FadeIn.EndSpeed.Value);
				data.Add(BitConverter.GetBytes(easing[0]));
				data.Add(BitConverter.GetBytes(easing[1]));
				data.Add(BitConverter.GetBytes(easing[2]));
			}

			data.Add(value.FadeOutType);
			if (value.FadeOutType.Value == Data.RendererCommonValues.FadeType.Use)
			{
				data.Add(value.FadeOut.Frame.GetBytes());

				var easing = Utl.MathUtl.Easing((float)value.FadeOut.StartSpeed.Value, (float)value.FadeOut.EndSpeed.Value);
				data.Add(BitConverter.GetBytes(easing[0]));
				data.Add(BitConverter.GetBytes(easing[1]));
				data.Add(BitConverter.GetBytes(easing[2]));
			}

			data.Add(new BasicUvSerializer(value, version).SerializeUv(texInfo));


			if (version >= ExporterVersion.Ver16Alpha1)
			{
				AddUvBytes(advanceValue, data, texInfoRepo);
			}


			// Inheritance
			data.Add(value.ColorInheritType.GetValueAsInt().GetBytes());

			// Distortion
			data.Add(value.DistortionIntensity.GetBytes());

			// Custom data1 from 1.5
			data.Add(value.CustomData1.CustomData);
			if (value.CustomData1.CustomData.Value == Data.CustomDataType.Fixed2D)
			{
				data.Add(BitConverter.GetBytes(value.CustomData1.Fixed.X.Value));
				data.Add(BitConverter.GetBytes(value.CustomData1.Fixed.Y.Value));
			}
			else if (value.CustomData1.CustomData.Value == Data.CustomDataType.Random2D)
			{
				var value_ = value.CustomData1.Random;
				var bytes1 = value_.GetBytes(1.0f);
				data.Add(bytes1);
			}
			else if (value.CustomData1.CustomData.Value == Data.CustomDataType.Easing2D)
			{
				var easing = Utl.MathUtl.Easing((float)value.CustomData1.Easing.StartSpeed.Value, (float)value.CustomData1.Easing.EndSpeed.Value);

				List<byte[]> _data = new List<byte[]>();
				_data.Add(value.CustomData1.Easing.Start.GetBytes(1.0f));
				_data.Add(value.CustomData1.Easing.End.GetBytes(1.0f));
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
				var __data = _data.ToArray().ToArray();
				data.Add(__data);
			}
			else if (value.CustomData1.CustomData.Value == Data.CustomDataType.FCurve2D)
			{
				var value_ = value.CustomData1.FCurve;
				var bytes1 = value_.GetBytes();
				data.Add(bytes1);
			}
			else if (value.CustomData1.CustomData.Value == Data.CustomDataType.Fixed4D)
			{
				data.Add(BitConverter.GetBytes(value.CustomData1.Fixed4.X.Value));
				data.Add(BitConverter.GetBytes(value.CustomData1.Fixed4.Y.Value));
				data.Add(BitConverter.GetBytes(value.CustomData1.Fixed4.Z.Value));
				data.Add(BitConverter.GetBytes(value.CustomData1.Fixed4.W.Value));
			}
			else if (value.CustomData1.CustomData.Value == Data.CustomDataType.FCurveColor)
			{
				var bytes = value.CustomData1.FCurveColor.GetBytes();
				data.Add(bytes);
			}

			// Custom data2 from 1.5
			data.Add(value.CustomData2.CustomData);
			if (value.CustomData2.CustomData.Value == Data.CustomDataType.Fixed2D)
			{
				data.Add(BitConverter.GetBytes(value.CustomData2.Fixed.X.Value));
				data.Add(BitConverter.GetBytes(value.CustomData2.Fixed.Y.Value));
			}
			else if (value.CustomData2.CustomData.Value == Data.CustomDataType.Random2D)
			{
				var value_ = value.CustomData2.Random;
				var bytes1 = value_.GetBytes(1.0f);
				data.Add(bytes1);
			}
			else if (value.CustomData2.CustomData.Value == Data.CustomDataType.Easing2D)
			{
				var easing = Utl.MathUtl.Easing((float)value.CustomData2.Easing.StartSpeed.Value, (float)value.CustomData2.Easing.EndSpeed.Value);

				List<byte[]> _data = new List<byte[]>();
				_data.Add(value.CustomData2.Easing.Start.GetBytes(1.0f));
				_data.Add(value.CustomData2.Easing.End.GetBytes(1.0f));
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
				var __data = _data.ToArray().ToArray();
				data.Add(__data);
			}
			else if (value.CustomData2.CustomData.Value == Data.CustomDataType.FCurve2D)
			{
				var value_ = value.CustomData2.FCurve;
				var bytes1 = value_.GetBytes();
				data.Add(bytes1);
			}
			else if (value.CustomData2.CustomData.Value == Data.CustomDataType.Fixed4D)
			{
				data.Add(BitConverter.GetBytes(value.CustomData2.Fixed4.X.Value));
				data.Add(BitConverter.GetBytes(value.CustomData2.Fixed4.Y.Value));
				data.Add(BitConverter.GetBytes(value.CustomData2.Fixed4.Z.Value));
				data.Add(BitConverter.GetBytes(value.CustomData2.Fixed4.W.Value));
			}
			else if (value.CustomData2.CustomData.Value == Data.CustomDataType.FCurveColor)
			{
				var bytes = value.CustomData2.FCurveColor.GetBytes();
				data.Add(bytes);
			}

			if (version >= ExporterVersion.Ver16Alpha1)
			{
				if (version >= ExporterVersion.Ver16Alpha6)
				{
					if (advanceValue.AlphaCutoffParam.Enabled)
					{
						data.Add((1).GetBytes());
						data.Add(AlphaCutoffValues.GetBytes(advanceValue.AlphaCutoffParam, version));
					}
					else
					{
						data.Add((0).GetBytes());
					}
				}
				else
				{
					data.Add(AlphaCutoffValues.GetBytes(advanceValue.AlphaCutoffParam, version));
				}
			}

			if (version >= ExporterVersion.Ver16Alpha3)
			{
				if (advanceValue.FalloffParam.Enabled)
				{
					data.Add((1).GetBytes());

					data.Add(advanceValue.FalloffParam.ColorBlendType);
					data.Add(advanceValue.FalloffParam.BeginColor);
					data.Add(advanceValue.FalloffParam.EndColor);
					data.Add(BitConverter.GetBytes(advanceValue.FalloffParam.Pow.Value));
				}
				else
				{
					data.Add((0).GetBytes());
				}
			}

			if (advanceValue.SoftParticleParams.Enabled)
			{
				if (version >= ExporterVersion.Ver16Alpha4)
				{
					data.Add(advanceValue.SoftParticleParams.Distance.GetBytes());
				}
				if (version >= ExporterVersion.Ver16Alpha5)
				{
					data.Add(advanceValue.SoftParticleParams.DistanceNear.GetBytes());
					data.Add(advanceValue.SoftParticleParams.DistanceNearOffset.GetBytes());
				}
			}
			else
			{
				if (version >= ExporterVersion.Ver16Alpha4)
				{
					data.Add((0.0f).GetBytes());
				}
				if (version >= ExporterVersion.Ver16Alpha5)
				{
					data.Add((0.0f).GetBytes());
					data.Add((0.0f).GetBytes());
				}
			}

			return data.ToArray().ToArray();
		}

		private static IEnumerable<byte[]> GetTextureValues(
			Data.RendererCommonValues value,
			AdvancedRenderCommonValues advanceValue,
			SortedDictionary<string, int> texture_and_index,
			SortedDictionary<string, int> normalTexture_and_index,
			SortedDictionary<string, int> distortionTexture_and_index,
			SortedDictionary<string, int> material_and_index,
			ExporterVersion version,
			TextureInformationRepository texInfoRepo)
		{
			var aggregator = new TextureValuesAggregator(value, advanceValue, texInfoRepo);
			MaterialSerializerInstance.AddMaterialData(version, value, aggregator,
				texture_and_index, distortionTexture_and_index, normalTexture_and_index, material_and_index);
			return aggregator.CurrentData;
		}

		private static void AddUvBytes(AdvancedRenderCommonValues advanceValue,
			List<byte[]> data, TextureInformationRepository repo)
		{
			data.Add(GetUvBytes(repo.Alpha, advanceValue.AlphaTextureParam));
			data.Add(GetUvBytes(repo.UvDistortion, advanceValue.UVDistortionTextureParam));

			// uv distortion intensity
			data.Add(advanceValue.UVDistortionTextureParam.UVDistortionIntensity.GetBytes());

			data.Add(GetUvBytes(repo.Blend, advanceValue.BlendTextureParams.BlendTextureParam));

			// blend texture blend type
			if (advanceValue.BlendTextureParams.Enabled &&
				advanceValue.BlendTextureParams.BlendTextureParam.Texture.RelativePath != string.Empty)
			{
				data.Add(advanceValue.BlendTextureParams.BlendTextureParam.BlendType);
			}
			else
			{
				data.Add((-1).GetBytes());
			}

			data.Add(GetUvBytes(repo.BlendAlpha, advanceValue.BlendTextureParams.BlendAlphaTextureParam));
			data.Add(GetUvBytes(repo.BlendUvDistortion, advanceValue.BlendTextureParams.BlendUVDistortionTextureParam));

			// blend uv distortion intensity
			data.Add(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.UVDistortionIntensity.GetBytes());
		}

		private static byte[] GetUvBytes(TextureInformation texInfoL, IUvCommandValues param)
		{
			var serializer = new AdvancedUvSerializer(param);
			return serializer.SerializeUv(texInfoL);
		}
	}
}
