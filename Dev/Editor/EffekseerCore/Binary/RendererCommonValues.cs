using System;
using System.Collections.Generic;
using Effekseer.Binary.RenderData;
using Effekseer.Data;
using Effekseer.Data.Group;
using Effekseer.Utils;

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
			System.Func<string, string> convertLoadingFilePath)
		{
			List<byte[]> data = new List<byte[]>();

			var texInfoRepo = new TextureInformationRepository();
			var texInfo = texInfoRepo.Texture;

			data.Add(((int)value.Material.Value).GetBytes());

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
				texInfoRepo,
				convertLoadingFilePath));

			data.Add(value.AlphaBlend);
			data.Add(value.Filter);
			data.Add(value.Wrap);

			data.Add(value.Filter2);
			data.Add(value.Wrap2);

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
			if (value.FadeInType.Value == Data.RendererCommonValues.FadeInMethod.Use)
			{
				data.Add(value.FadeIn.Frame.GetBytes());

				var easing = MathUtl.Easing((float)value.FadeIn.StartSpeed.Value, (float)value.FadeIn.EndSpeed.Value);
				data.Add(BitConverter.GetBytes(easing[0]));
				data.Add(BitConverter.GetBytes(easing[1]));
				data.Add(BitConverter.GetBytes(easing[2]));
			}

			data.Add(value.FadeOutType);
			if (value.FadeOutType.Value == Data.RendererCommonValues.FadeOutMethod.WithinLifetime ||
				value.FadeOutType.Value == Data.RendererCommonValues.FadeOutMethod.AfterRemoved)
			{
				data.Add(value.FadeOut.Frame.GetBytes());

				var easing = MathUtl.Easing((float)value.FadeOut.StartSpeed.Value, (float)value.FadeOut.EndSpeed.Value);
				data.Add(BitConverter.GetBytes(easing[0]));
				data.Add(BitConverter.GetBytes(easing[1]));
				data.Add(BitConverter.GetBytes(easing[2]));
			}

			data.Add(new BasicUvSerializer(value).SerializeUv(texInfo));

			{
				AddUvBytes(advanceValue, data, texInfoRepo);
			}

			data.Add(BitConverter.GetBytes(value.UVFlipHorizontalProbability.Value));

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
				var easing = MathUtl.Easing((float)value.CustomData1.Easing.StartSpeed.Value, (float)value.CustomData1.Easing.EndSpeed.Value);

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
				var easing = MathUtl.Easing((float)value.CustomData2.Easing.StartSpeed.Value, (float)value.CustomData2.Easing.EndSpeed.Value);

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

			{
				if (advanceValue.AlphaCutoffParam.Enabled)
				{
					data.Add((1).GetBytes());
					data.Add(AlphaCutoffValues.GetBytes(advanceValue.AlphaCutoffParam));
				}
				else
				{
					data.Add((0).GetBytes());
				}
			}

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
				{
					data.Add(advanceValue.SoftParticleParams.Distance.GetBytes());
				}
				{
					data.Add(advanceValue.SoftParticleParams.DistanceNear.GetBytes());
					data.Add(advanceValue.SoftParticleParams.DistanceNearOffset.GetBytes());
				}
			}
			else
			{
				{
					data.Add((0.0f).GetBytes());
				}
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
			TextureInformationRepository texInfoRepo,
			System.Func<string, string> convertLoadingFilePath)
		{
			var aggregator = new TextureValuesAggregator(value, advanceValue, texInfoRepo, convertLoadingFilePath);
			MaterialSerializerInstance.AddMaterialData(value, aggregator,
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