using System;
using System.Collections.Generic;
using System.Linq;
using Effekseer.Data;
using Effekseer.Data.Value;
using Effekseer.Utl;


namespace Effekseer.Binary
{
	class RendererCommonValues
	{
		private static readonly MaterialSerializer MaterialSerializerInstance = new MaterialSerializer();

		public static byte[] GetBytes(Data.RendererCommonValues value,
			Data.AdvancedRenderCommonValues advanceValue,
			Data.AdvancedRenderCommonValues2 advanceValue2,
			Dictionary<string, int> texture_and_index,
			Dictionary<string, int> normalTexture_and_index,
			Dictionary<string, int> distortionTexture_and_index,
			Dictionary<string, int> material_and_index,
			ExporterVersion version)
		{
			List<byte[]> data = new List<byte[]>();

			var texInfo = new TextureInformation();

			var alphaTexInfo = new TextureInformation();

			var uvDistortionTexInfo = new TextureInformation();

			var blendTexInfo = new TextureInformation();

			var blendAlphaTexInfo = new TextureInformation();

			var blendUVDistortionTexInfo = new TextureInformation();

			data.Add(((int)value.Material.Value).GetBytes());

			if (version >= ExporterVersion.Ver16Alpha1)
			{
				if (value.Material.Value == Data.RendererCommonValues.MaterialType.Default ||
				value.Material.Value == Data.RendererCommonValues.MaterialType.Lighting)
				{
					data.Add(BitConverter.GetBytes(value.EmissiveScaling));
				}
			}

			{
				var aggregator = new TextureValuesAggregator(value, advanceValue, advanceValue2,
					texInfo, alphaTexInfo, uvDistortionTexInfo, blendTexInfo, blendAlphaTexInfo, blendUVDistortionTexInfo);
				MaterialSerializerInstance.AddMaterialData(version, value, aggregator,
					texture_and_index, distortionTexture_and_index, normalTexture_and_index, material_and_index);
				data.AddRange(aggregator.CurrentData);
			}

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

				data.Add(advanceValue2.BlendTextureParams.BlendTextureParam.Filter);
				data.Add(advanceValue2.BlendTextureParams.BlendTextureParam.Wrap);

				data.Add(advanceValue2.BlendTextureParams.BlendAlphaTextureParam.Filter);
				data.Add(advanceValue2.BlendTextureParams.BlendAlphaTextureParam.Wrap);

				data.Add(advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.Filter);
				data.Add(advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.Wrap);
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
			
			// specification change(1.5)
			float width = 128.0f;
			float height = 128.0f;

			if (texInfo.Width > 0 && texInfo.Height > 0)
			{
				width = (float) texInfo.Width;
				height = (float) texInfo.Height;
			}
			data.Add(SerializeUv(value, width, height));


			if (version >= ExporterVersion.Ver16Alpha1)
			{
				// alpha texture
				data.Add(GetUVBytes
				(
				alphaTexInfo,
				advanceValue.AlphaTextureParam.UV,
				advanceValue.AlphaTextureParam.UVFixed,
				advanceValue.AlphaTextureParam.UVAnimation,
				advanceValue.AlphaTextureParam.UVScroll,
				advanceValue.AlphaTextureParam.UVFCurve
				));

				// uv distortion texture
				data.Add(GetUVBytes
				(
				uvDistortionTexInfo,
				advanceValue.UVDistortionTextureParam.UV,
				advanceValue.UVDistortionTextureParam.UVFixed,
				advanceValue.UVDistortionTextureParam.UVAnimation,
				advanceValue.UVDistortionTextureParam.UVScroll,
				advanceValue.UVDistortionTextureParam.UVFCurve
				));

				// uv distortion intensity
				data.Add(advanceValue.UVDistortionTextureParam.UVDistortionIntensity.GetBytes());

				// blend texture
				data.Add(GetUVBytes
				(
				blendTexInfo,
				advanceValue2.BlendTextureParams.BlendTextureParam.UV,
				advanceValue2.BlendTextureParams.BlendTextureParam.UVFixed,
				advanceValue2.BlendTextureParams.BlendTextureParam.UVAnimation,
				advanceValue2.BlendTextureParams.BlendTextureParam.UVScroll,
				advanceValue2.BlendTextureParams.BlendTextureParam.UVFCurve
				));

				// blend texture blend type
				if (advanceValue2.EnableBlendTexture && advanceValue2.BlendTextureParams.BlendTextureParam.Texture.RelativePath != string.Empty)
				{
					data.Add(advanceValue2.BlendTextureParams.BlendTextureParam.BlendType);
				}
				else
				{
					data.Add((-1).GetBytes());
				}

				// blend alpha texture
				data.Add(GetUVBytes
				(
					blendAlphaTexInfo,
					advanceValue2.BlendTextureParams.BlendAlphaTextureParam.UV,
					advanceValue2.BlendTextureParams.BlendAlphaTextureParam.UVFixed,
					advanceValue2.BlendTextureParams.BlendAlphaTextureParam.UVAnimation,
					advanceValue2.BlendTextureParams.BlendAlphaTextureParam.UVScroll,
					advanceValue2.BlendTextureParams.BlendAlphaTextureParam.UVFCurve
				));

				// blend uv distoriton texture
				data.Add(GetUVBytes
				(
					blendUVDistortionTexInfo,
					advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.UV,
					advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.UVFixed,
					advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.UVAnimation,
					advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.UVScroll,
					advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.UVFCurve
				));

				// blend uv distoriton intensity
				data.Add(advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.UVDistortionIntensity.GetBytes());
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
				data.Add(AlphaCutoffValues.GetBytes(advanceValue.AlphaCutoffParam, version));
			}

			if (version >= ExporterVersion.Ver16Alpha3)
			{
				if (advanceValue.EnableFalloff)
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

			return data.ToArray().ToArray();
		}

		private static byte[] SerializeUv(Data.RendererCommonValues value, float width, float height)
		{
			var aggregator = new UvAggregator(width, height);
			aggregator.Add(value.UV);

			switch (value.UV.Value)
			{
				case Data.RendererCommonValues.UVType.Default:
					break;
				case Data.RendererCommonValues.UVType.Fixed:
					SerializeFixedUv(value.UVFixed, aggregator);
					break;
				case Data.RendererCommonValues.UVType.Animation:
					SerializeAnimationUvRefactor(value.UVAnimation.AnimationParams, aggregator);
					aggregator.Add(value.UVAnimation.FlipbookInterpolationType);
					break;
				case Data.RendererCommonValues.UVType.Scroll:
					SerializeScrollUvRefactor(value.UVScroll, aggregator);
					break;
				case Data.RendererCommonValues.UVType.FCurve:
					SerializeFCurveUv(value.UVFCurve, aggregator);
					break;
			}

			return aggregator.CurrentData.ToArray().ToArray();
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

		public static byte[] GetUVBytes(TextureInformation _TexInfo,
						Data.Value.Enum<Data.RendererCommonValues.UVType> _UVType,
						Data.RendererCommonValues.UVFixedParamater _Fixed,
						Data.RendererCommonValues.UVAnimationParamater _Animation,
						Data.RendererCommonValues.UVScrollParamater _Scroll,
						Data.RendererCommonValues.UVFCurveParamater _FCurve)
		{
			// sprcification change(1.5)
			float width = 128.0f;
			float height = 128.0f;

			if (_TexInfo.Width > width && _TexInfo.Height > height)
			{
				width = (float)_TexInfo.Width;
				height = (float)_TexInfo.Height;
			}

			var aggregator = new UvAggregator(width, height);
			aggregator.Add(_UVType);

			switch (_UVType.Value)
			{
				case Data.RendererCommonValues.UVType.Default:
					break;
				case Data.RendererCommonValues.UVType.Fixed:
					SerializeFixedUv(_Fixed, aggregator);
					break;
				case Data.RendererCommonValues.UVType.Animation:
					SerializeAnimationUvRefactor(_Animation, aggregator);
					break;
				case Data.RendererCommonValues.UVType.Scroll:
					SerializeScrollUvRefactor(_Scroll, aggregator);
					break;
				case Data.RendererCommonValues.UVType.FCurve:
					SerializeFCurveUv(_FCurve, aggregator);
					break;
			}

			return aggregator.CurrentData.ToArray().ToArray();
		}

		private static void SerializeFCurveUv(Data.RendererCommonValues.UVFCurveParamater curve, UvAggregator aggregator)
		{
			var width = aggregator.Width;
			var height = aggregator.Height;
			aggregator.Add(curve.Start.GetBytes(1.0f / width, 1.0f / height));
			aggregator.Add(curve.Size.GetBytes(1.0f / width, 1.0f / height));
		}

		private static void SerializeFixedUv(Data.RendererCommonValues.UVFixedParamater uvFixed,
			UvAggregator aggregator)
		{
			aggregator.AddVector2D(uvFixed.Start);
			aggregator.AddVector2D(uvFixed.Size);
		}

		private static void SerializeAnimationUvRefactor(Data.RendererCommonValues.UVAnimationParamater uvAnimation, UvAggregator aggregator)
		{
			aggregator.AddVector2D(uvAnimation.Start);
			aggregator.AddVector2D(uvAnimation.Size);

			var frameLength = uvAnimation.FrameLength.Infinite
				? (int.MaxValue / 100)
				: uvAnimation.FrameLength.Value.Value;
			aggregator.AddInt(frameLength);

			aggregator.AddInt(uvAnimation.FrameCountX.Value);
			aggregator.AddInt(uvAnimation.FrameCountY.Value);
			aggregator.Add(uvAnimation.LoopType);

			aggregator.AddInt(uvAnimation.StartSheet.Max);
			aggregator.AddInt(uvAnimation.StartSheet.Min);
		}


		private static void SerializeScrollUvRefactor(
			Data.RendererCommonValues.UVScrollParamater uvScroll,
			UvAggregator aggregator)
		{
			aggregator.AddRandomVector2D(uvScroll.Start);
			aggregator.AddRandomVector2D(uvScroll.Size);
			aggregator.AddRandomVector2D(uvScroll.Speed);
		}

		private class UvSerializer
		{
			
		}

		private sealed class UvAggregator
		{
			public float Width { get; }
			public float Height { get; }
			private readonly List<byte[]> _data = new List<byte[]>();

			public IEnumerable<byte[]> CurrentData => _data;

			public UvAggregator(float width, float height)
			{
				Width = width;
				Height = height;
			}

			public void AddInt(int value)
			{
				_data.Add(value.GetBytes());
			}

			public void AddFloat(float value)
			{
				_data.Add(value.GetBytes());
			}

			public void Add(byte[] value)
			{
				_data.Add(value);
			}

			public void AddVector2D(Vector2D vector)
			{
				AddFloat(vector.X / Width);
				AddFloat(vector.Y / Height);
			}

			public void AddRandomVector2D(Vector2DWithRandom vector)
			{
				AddFloat(vector.X.Max / Width);
				AddFloat(vector.Y.Max / Height);
				AddFloat(vector.X.Min / Width);
				AddFloat(vector.Y.Min / Height);
			}
		}

		private sealed class MaterialSerializer
		{
			private void AddTextureInfo(ExporterVersion version,
				Dictionary<string, int> texAndInd,
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
				Dictionary<string, int> texture_and_index,
				Data.RendererCommonValues value,
				TextureValuesAggregator aggregator)
			{
				aggregator.AddTexIdAndStoreSize(value.ColorTexture, 1, texture_and_index);
				aggregator.AddInt(-1);
				AddTextureInfo(version, texture_and_index, aggregator);
			}

			private void AddAsBackDistortion(ExporterVersion version,
				Dictionary<string, int> distortionTexture_and_index,
				Data.RendererCommonValues value,
				TextureValuesAggregator aggregator)
			{
				aggregator.AddTexIdAndStoreSize(value.ColorTexture, 1, distortionTexture_and_index);
				aggregator.AddInt(-1);
				AddTextureInfo(version, distortionTexture_and_index, aggregator);
			}

			private void AddAsLighting(ExporterVersion version,
				Dictionary<string, int> texture_and_index,
				Dictionary<string, int> normalTexture_and_index,
				Data.RendererCommonValues value,
				TextureValuesAggregator aggregator)
			{
				aggregator.AddTexIdAndStoreSize(value.ColorTexture, 1, texture_and_index);
				aggregator.AddTexIdAndStoreSize(value.NormalTexture, 2, normalTexture_and_index);
				AddTextureInfo(version, texture_and_index, aggregator);
			}

			private void AddAsFile(TextureValuesAggregator aggregator,
				Data.RendererCommonValues value,
				Dictionary<string, int> texture_and_index,
				Dictionary<string, int> normalTexture_and_index,
				Dictionary<string, int> material_and_index)
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

				aggregator.AddInt(material_and_index.ContainsKey(value.MaterialFile.Path.RelativePath)
					? material_and_index[value.MaterialFile.Path.RelativePath]
					: (-1));

				aggregator.AddInt(textures.Length);

				foreach (var texture in textures)
				{
					var texture_ = texture.Item1.Value as Data.Value.PathForImage;
					if (texture.Item2.Type == TextureType.Value)
					{
						aggregator.AddInt(1);
						aggregator.AddTexIdAndStoreSize(texture_, texture.Item2.Priority, normalTexture_and_index);
					}
					else
					{
						aggregator.AddInt(0);
						aggregator.AddTexIdAndStoreSize(texture_, texture.Item2.Priority, texture_and_index);
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
			}

			public void AddMaterialData(ExporterVersion version,
				Data.RendererCommonValues value,
				TextureValuesAggregator aggregator,
				Dictionary<string, int> texture_and_index,
				Dictionary<string, int> distortionTexture_and_index,
				Dictionary<string, int> normalTexture_and_index,
				Dictionary<string, int> material_and_index)
			{
				switch (value.Material.Value)
				{
					case Data.RendererCommonValues.MaterialType.Default:
						AddAsDefaultMaterial(version, texture_and_index, value, aggregator);
						break;
					case Data.RendererCommonValues.MaterialType.BackDistortion:
						AddAsBackDistortion(version, distortionTexture_and_index, value, aggregator);
						break;
					case Data.RendererCommonValues.MaterialType.Lighting:
						AddAsLighting(version, texture_and_index, normalTexture_and_index, value, aggregator);
						break;
					default:
						AddAsFile(aggregator, value, texture_and_index, normalTexture_and_index, material_and_index);
						break;
				}
			}
		}

		private sealed class TextureValuesAggregator
		{
			private readonly Data.RendererCommonValues _value;
			private readonly AdvancedRenderCommonValues _advanceValue;
			private readonly AdvancedRenderCommonValues2 _advanceValue2;
			private TextureInformation _texInfo;
			private TextureInformation _alpha;
			private TextureInformation _uvDistortion;
			private TextureInformation _blend;
			private TextureInformation _blendAlpha;
			private TextureInformation _blendUvDistortion;
			private readonly List<byte[]> _data = new List<byte[]>();

			public TextureValuesAggregator(
				Data.RendererCommonValues value,
				AdvancedRenderCommonValues advanceValue,
				AdvancedRenderCommonValues2 advanceValue2,
				TextureInformation texInfo,
				TextureInformation alpha,
				TextureInformation uvDistortion,
				TextureInformation blend,
				TextureInformation blendAlpha,
				TextureInformation blendUvDistortion)
			{
				_value = value;
				_advanceValue = advanceValue;
				_advanceValue2 = advanceValue2;
				_texInfo = texInfo;
				_alpha = alpha;
				_uvDistortion = uvDistortion;
				_blend = blend;
				_blendAlpha = blendAlpha;
				_blendUvDistortion = blendUvDistortion;
			}

			public IEnumerable<byte[]> CurrentData => _data;
			public bool IsBlendTextureEnabled => _advanceValue2.EnableBlendTexture;

			public void AddInt(int value)
			{
				_data.Add(value.GetBytes());
			}

			public void AddFloat(float value)
			{
				_data.Add(value.GetBytes());
			}

			public void AddTexIdAndStoreSize(PathForImage image, int number, Dictionary<string, int> texAndInd)
			{
				var tempTexInfo = new TextureInformation();

				if (!texAndInd.ContainsKey(image.RelativePath) || !tempTexInfo.Load(image.AbsolutePath))
				{
					AddInt(-1);
					return;
				}

				if (_value.UVTextureReferenceTarget.Value != Data.UVTextureReferenceTargetType.None
					&& number == (int)_value.UVTextureReferenceTarget.Value)
				{
					_texInfo.Load(image.AbsolutePath);
				}

				AddInt(texAndInd[image.RelativePath]);
			}

			private int GetTexIdAndInfo(PathForImage image, Dictionary<string, int> texAndInd, ref TextureInformation texInfoRef)
			{
				var tempTexInfo = new TextureInformation();

				if (!texAndInd.ContainsKey(image.RelativePath) || !tempTexInfo.Load(image.AbsolutePath))
				{
					return -1;
				}

				texInfoRef.Load(image.AbsolutePath);
				return texAndInd[image.RelativePath];
			}

			private void AddTexIdAndInfo(bool isEnabled, PathForImage path, Dictionary<string, int> texAndInd, ref TextureInformation textureInfo)
			{
				var value = isEnabled
					? GetTexIdAndInfo(path, texAndInd, ref textureInfo)
					: -1;
				AddInt(value);
			}

			public void AddAlphaTexture(Dictionary<string, int> texAndInd)
			{
				AddTexIdAndInfo(_advanceValue.EnableAlphaTexture,
					_advanceValue.AlphaTextureParam.Texture,
					texAndInd,
					ref _alpha);
			}

			public void AddUvDistortionTexture(Dictionary<string, int> texAndInd)
			{
				AddTexIdAndInfo(_advanceValue.EnableUVDistortionTexture,
					_advanceValue.UVDistortionTextureParam.Texture,
					texAndInd,
					ref _uvDistortion);
			}

			public void AddBlendAlphaTexture(Dictionary<string, int> texAndInd)
			{
				AddTexIdAndInfo(_advanceValue2.BlendTextureParams.EnableBlendAlphaTexture,
					_advanceValue2.BlendTextureParams.BlendAlphaTextureParam.Texture,
					texAndInd,
					ref _blendAlpha);
			}

			public void AddBlendUvDistortionTexture(Dictionary<string, int> texAndInd)
			{
				AddTexIdAndInfo(_advanceValue2.BlendTextureParams.EnableBlendUVDistortionTexture,
					_advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.Texture,
					texAndInd,
					ref _blendUvDistortion);
			}

			public void AddBlendTexture(Dictionary<string, int> texAndInd)
			{
				GetTexIdAndInfo(_advanceValue2.BlendTextureParams.BlendTextureParam.Texture,
					texAndInd,
					ref _blend);
			}
		}
	}

}
