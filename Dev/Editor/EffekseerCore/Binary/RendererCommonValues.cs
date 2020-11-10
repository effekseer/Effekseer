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

			int GetTexIdAndStoreSize(PathForImage image, int number, Dictionary<string, int> texAndInd)
			{
				var tempTexInfo = new TextureInformation();

				if (!texAndInd.ContainsKey(image.RelativePath) || !tempTexInfo.Load(image.AbsolutePath))
				{
					return -1;
				}

				if (value.UVTextureReferenceTarget.Value != Data.UVTextureReferenceTargetType.None && number == (int)value.UVTextureReferenceTarget.Value)
				{
					texInfo.Load(image.AbsolutePath);
				}

				return texAndInd[image.RelativePath];

			}

			int GetTexIdAndInfo(PathForImage image, Dictionary<string, int> texAndInd, ref TextureInformation texInfoRef)
			{
				var tempTexInfo = new TextureInformation();

				if (!texAndInd.ContainsKey(image.RelativePath) || !tempTexInfo.Load(image.AbsolutePath))
				{
					return -1;
				}

				texInfoRef.Load(image.AbsolutePath);
				return texAndInd[image.RelativePath];
			}

			int ReadTexIdAndInfo(bool isEnabled, PathForImage path, Dictionary<string, int> texAndInd, ref TextureInformation textureInfo)
			{
				return isEnabled
					? GetTexIdAndInfo(path, texAndInd, ref textureInfo)
					: -1;
			}

			int ReadAlphaTexture(Dictionary<string, int> texAndInd)
			{
				return ReadTexIdAndInfo(advanceValue.EnableAlphaTexture,
					advanceValue.AlphaTextureParam.Texture,
					texAndInd,
					ref alphaTexInfo);
			}

			int ReadUvDistortionTexture(Dictionary<string, int> texAndInd)
			{
				return ReadTexIdAndInfo(advanceValue.EnableUVDistortionTexture,
					advanceValue.UVDistortionTextureParam.Texture,
					texAndInd,
					ref uvDistortionTexInfo);
			}

			int ReadBlendAlphaTexture(Dictionary<string, int> texAndInd)
			{
				return ReadTexIdAndInfo(advanceValue2.BlendTextureParams.EnableBlendAlphaTexture,
					advanceValue2.BlendTextureParams.BlendAlphaTextureParam.Texture,
					texAndInd,
					ref blendAlphaTexInfo);
			}

			int ReadBlendUvDistortionTexture(Dictionary<string, int> texAndInd)
			{
				return ReadTexIdAndInfo(advanceValue2.BlendTextureParams.EnableBlendUVDistortionTexture,
					advanceValue2.BlendTextureParams.BlendUVDistortionTextureParam.Texture,
					texAndInd,
					ref blendUVDistortionTexInfo);
			}

			int ReadBlendTexture(Dictionary<string, int> texAndInd)
			{
				return GetTexIdAndInfo(advanceValue2.BlendTextureParams.BlendTextureParam.Texture,
					texAndInd,
					ref blendTexInfo);
			}

			IEnumerable<int> ReadTextureInfo(Dictionary<string, int> texAndInd)
			{
				if (version < ExporterVersion.Ver16Alpha1)
				{
					yield break;
				}

				// alpha texture
				yield return ReadAlphaTexture(texAndInd);
				// uv distortion texture
				yield return ReadUvDistortionTexture(texAndInd);

				// blend texture
				if (advanceValue2.EnableBlendTexture)
				{
					yield return ReadBlendTexture(texAndInd);
					// blend alpha texture
					yield return ReadBlendAlphaTexture(texAndInd);
					// blend uv distortion texture
					yield return ReadBlendUvDistortionTexture(texAndInd);
				}
				else
				{
					yield return -1;
					yield return -1;
					yield return -1;
				}
			}
			
			IEnumerable<int> ReadAsDefaultMaterial()
			{
				// texture1
				yield return GetTexIdAndStoreSize(value.ColorTexture, 1, texture_and_index);

				// texture2
				yield return -1;

				foreach (int i in ReadTextureInfo(texture_and_index))
				{
					yield return i;
				}
			}
			
			IEnumerable<int> ReadAsBackDistortion()
			{
				// texture1
				yield return GetTexIdAndStoreSize(value.ColorTexture, 1, distortionTexture_and_index);

				// texture2
				yield return -1;

				foreach (var element in ReadTextureInfo(distortionTexture_and_index))
				{
					yield return element;
				}
			}
			
			IEnumerable<int> ReadAsLighting()
			{
				// texture1
				yield return GetTexIdAndStoreSize(value.ColorTexture, 1, texture_and_index);

				// texture2
				yield return GetTexIdAndStoreSize(value.NormalTexture, 2, normalTexture_and_index);

				foreach (var item in ReadTextureInfo(texture_and_index))
				{
					yield return item;
				}
			}
			
			IEnumerable<byte[]> ReadAsFile()
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

				yield return material_and_index.ContainsKey(value.MaterialFile.Path.RelativePath)
					? material_and_index[value.MaterialFile.Path.RelativePath].GetBytes()
					: (-1).GetBytes();

				yield return textures.Length.GetBytes();

				foreach (var texture in textures)
				{
					var texture_ = texture.Item1.Value as Data.Value.PathForImage;
					if (texture.Item2.Type == TextureType.Value)
					{
						yield return 1.GetBytes();
						yield return GetTexIdAndStoreSize(texture_, texture.Item2.Priority, normalTexture_and_index).GetBytes();
					}
					else
					{
						yield return 0.GetBytes();
						yield return GetTexIdAndStoreSize(texture_, texture.Item2.Priority, texture_and_index).GetBytes();
					}
				}

				yield return uniforms.Count.GetBytes();

				foreach (var uniform in uniforms)
				{
					float[] floats = GetUniformsVertexes(uniform);

					yield return floats[0].GetBytes();
					yield return floats[1].GetBytes();
					yield return floats[2].GetBytes();
					yield return floats[3].GetBytes();
				}
			}

			IEnumerable<byte[]> ReadCommandValues()
			{
				switch (value.Material.Value)
				{
					case Data.RendererCommonValues.MaterialType.Default:
						return ReadAsDefaultMaterial().Select(x => x.GetBytes());
					case Data.RendererCommonValues.MaterialType.BackDistortion:
						return ReadAsBackDistortion().Select(x => x.GetBytes());
					case Data.RendererCommonValues.MaterialType.Lighting:
						return ReadAsLighting().Select(x => x.GetBytes());
					default:
						return ReadAsFile();
				}
			}

			data.AddRange(ReadCommandValues());

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

			// sprcification change(1.5)
			float width = 128.0f;
			float height = 128.0f;

			if (texInfo.Width > 0 && texInfo.Height > 0)
			{
				width = (float)texInfo.Width;
				height = (float)texInfo.Height;
			}

			data.Add(value.UV);
			if (value.UV.Value == Data.RendererCommonValues.UVType.Default)
			{
			}
			else if (value.UV.Value == Data.RendererCommonValues.UVType.Fixed)
			{
				var value_ = value.UVFixed;
				data.Add((value_.Start.X / width).GetBytes());
				data.Add((value_.Start.Y / height).GetBytes());
				data.Add((value_.Size.X / width).GetBytes());
				data.Add((value_.Size.Y / height).GetBytes());
			}
			else if (value.UV.Value == Data.RendererCommonValues.UVType.Animation)
			{
				var value_ = value.UVAnimation;

				data.Add((value_.AnimationParams.Start.X / width).GetBytes());
				data.Add((value_.AnimationParams.Start.Y / height).GetBytes());
				data.Add((value_.AnimationParams.Size.X / width).GetBytes());
				data.Add((value_.AnimationParams.Size.Y / height).GetBytes());

				if (value_.AnimationParams.FrameLength.Infinite)
				{
					var inf = int.MaxValue / 100;
					data.Add(inf.GetBytes());
				}
				else
				{
					data.Add(value_.AnimationParams.FrameLength.Value.Value.GetBytes());
				}

				data.Add(value_.AnimationParams.FrameCountX.Value.GetBytes());
				data.Add(value_.AnimationParams.FrameCountY.Value.GetBytes());
				data.Add(value_.AnimationParams.LoopType);

				data.Add(value_.AnimationParams.StartSheet.Max.GetBytes());
				data.Add(value_.AnimationParams.StartSheet.Min.GetBytes());

				data.Add(value_.FlipbookInterpolationType);
			}
			else if (value.UV.Value == Data.RendererCommonValues.UVType.Scroll)
			{
				var value_ = value.UVScroll;
				data.Add((value_.Start.X.Max / width).GetBytes());
				data.Add((value_.Start.Y.Max / height).GetBytes());
				data.Add((value_.Start.X.Min / width).GetBytes());
				data.Add((value_.Start.Y.Min / height).GetBytes());

				data.Add((value_.Size.X.Max / width).GetBytes());
				data.Add((value_.Size.Y.Max / height).GetBytes());
				data.Add((value_.Size.X.Min / width).GetBytes());
				data.Add((value_.Size.Y.Min / height).GetBytes());

				data.Add((value_.Speed.X.Max / width).GetBytes());
				data.Add((value_.Speed.Y.Max / height).GetBytes());
				data.Add((value_.Speed.X.Min / width).GetBytes());
				data.Add((value_.Speed.Y.Min / height).GetBytes());
			}
			else if (value.UV.Value == Data.RendererCommonValues.UVType.FCurve)
			{
				{
					var value_ = value.UVFCurve.Start;
					var bytes1 = value_.GetBytes(1.0f / width, 1.0f / height);
					List<byte[]> _data = new List<byte[]>();
					data.Add(bytes1);
				}

				{
					var value_ = value.UVFCurve.Size;
					var bytes1 = value_.GetBytes(1.0f / width, 1.0f / height);
					List<byte[]> _data = new List<byte[]>();
					data.Add(bytes1);
				}
			}


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
			if(value.CustomData1.CustomData.Value == Data.CustomDataType.Fixed2D)
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
			else if(value.CustomData1.CustomData.Value == Data.CustomDataType.FCurve2D)
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
			List<byte[]> data = new List<byte[]>();

			// sprcification change(1.5)
			float width = 128.0f;
			float height = 128.0f;

			if (_TexInfo.Width > width && _TexInfo.Height > height)
			{
				width = (float)_TexInfo.Width;
				height = (float)_TexInfo.Height;
			}

			data.Add(_UVType);
			if (_UVType == Data.RendererCommonValues.UVType.Default)
			{
			}
			else if (_UVType == Data.RendererCommonValues.UVType.Fixed)
			{
				var value_ = _Fixed;
				data.Add((value_.Start.X / width).GetBytes());
				data.Add((value_.Start.Y / height).GetBytes());
				data.Add((value_.Size.X / width).GetBytes());
				data.Add((value_.Size.Y / height).GetBytes());
			}
			else if (_UVType == Data.RendererCommonValues.UVType.Animation)
			{
				var value_ = _Animation;
				data.Add((value_.Start.X / width).GetBytes());
				data.Add((value_.Start.Y / height).GetBytes());
				data.Add((value_.Size.X / width).GetBytes());
				data.Add((value_.Size.Y / height).GetBytes());

				if (value_.FrameLength.Infinite)
				{
					var inf = int.MaxValue / 100;
					data.Add(inf.GetBytes());
				}
				else
				{
					data.Add(value_.FrameLength.Value.Value.GetBytes());
				}
			
				data.Add(value_.FrameCountX.Value.GetBytes());
				data.Add(value_.FrameCountY.Value.GetBytes());
				data.Add(value_.LoopType);

				data.Add(value_.StartSheet.Max.GetBytes());
				data.Add(value_.StartSheet.Min.GetBytes());

			}
			else if (_UVType == Data.RendererCommonValues.UVType.Scroll)
			{
				var value_ = _Scroll;
				data.Add((value_.Start.X.Max / width).GetBytes());
				data.Add((value_.Start.Y.Max / height).GetBytes());
				data.Add((value_.Start.X.Min / width).GetBytes());
				data.Add((value_.Start.Y.Min / height).GetBytes());

				data.Add((value_.Size.X.Max / width).GetBytes());
				data.Add((value_.Size.Y.Max / height).GetBytes());
				data.Add((value_.Size.X.Min / width).GetBytes());
				data.Add((value_.Size.Y.Min / height).GetBytes());

				data.Add((value_.Speed.X.Max / width).GetBytes());
				data.Add((value_.Speed.Y.Max / height).GetBytes());
				data.Add((value_.Speed.X.Min / width).GetBytes());
				data.Add((value_.Speed.Y.Min / height).GetBytes());
			}
			else if (_UVType == Data.RendererCommonValues.UVType.FCurve)
			{
				{
					var value_ = _FCurve.Start;
					var bytes1 = value_.GetBytes(1.0f / width);
					List<byte[]> _data = new List<byte[]>();
					data.Add(bytes1);
				}

				{
					var value_ = _FCurve.Size;
					var bytes1 = value_.GetBytes(1.0f / height);
					List<byte[]> _data = new List<byte[]>();
					data.Add(bytes1);
				}
			}

			return data.ToArray().ToArray();
		}
	}

}
