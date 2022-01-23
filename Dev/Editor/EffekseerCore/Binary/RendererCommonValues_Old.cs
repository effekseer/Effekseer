using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;


namespace Effekseer.Binary
{
	class RendererCommonValues_Old
	{
		delegate int GetTexIDAndInfo(Data.Value.PathForImage image, SortedDictionary<string, int> texAndInd, ref TextureInformation texInfoRef);

		public static byte[] GetBytes(Data.RendererCommonValues value,
			Data.AdvancedRenderCommonValues advanceValue,
			SortedDictionary<string, int> texture_and_index,
			SortedDictionary<string, int> normalTexture_and_index,
			SortedDictionary<string, int> distortionTexture_and_index,
			SortedDictionary<string, int> material_and_index,
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

			Func<Data.Value.PathForImage, int, SortedDictionary<string,int>, int> getTexIDAndStoreSize = (Data.Value.PathForImage image, int number, SortedDictionary<string, int> texAndInd) =>
			{
				var tempTexInfo = new TextureInformation();

				if (texAndInd.ContainsKey(image.RelativePath) && tempTexInfo.Load(image.AbsolutePath))
				{
					if(value.UVTextureReferenceTarget.Value != Data.UVTextureReferenceTargetType.None && number == (int)value.UVTextureReferenceTarget.Value)
					{
						texInfo.Load(image.AbsolutePath);
					}

					return texAndInd[image.RelativePath];
				}
				else
				{
					return -1;
				}
			};

			GetTexIDAndInfo getTexIDAndInfo = (Data.Value.PathForImage image, SortedDictionary<string, int> texAndInd, ref TextureInformation texInfoRef) =>
			{
				var tempTexInfo = new TextureInformation();

				if (texAndInd.ContainsKey(image.RelativePath) && tempTexInfo.Load(image.AbsolutePath))
				{
					texInfoRef.Load(image.AbsolutePath);
					return texAndInd[image.RelativePath];
				}

				return -1;
			};

			if (value.Material.Value == Data.RendererCommonValues.MaterialType.Default)
			{
				// texture1
				data.Add(getTexIDAndStoreSize(value.ColorTexture, 1, texture_and_index).GetBytes());

				// texture2
				data.Add((-1).GetBytes());

				if (version >= ExporterVersion.Ver16Alpha1)
				{
					// alpha texture
					if (advanceValue.AlphaTextureParam.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.AlphaTextureParam.Texture, texture_and_index, ref alphaTexInfo).GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}

					// uv distortion texture
					if (advanceValue.UVDistortionTextureParam.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.UVDistortionTextureParam.Texture, texture_and_index, ref uvDistortionTexInfo).GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}

					// blend texture
					if (advanceValue.BlendTextureParams.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendTextureParam.Texture, texture_and_index, ref blendTexInfo).GetBytes());

						// blend alpha texture
						if (advanceValue.BlendTextureParams.EnableBlendAlphaTexture)
						{
							data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendAlphaTextureParam.Texture, texture_and_index, ref blendAlphaTexInfo).GetBytes());
						}
						else
						{
							data.Add((-1).GetBytes());
						}

						// blend uv distortion texture
						if (advanceValue.BlendTextureParams.EnableBlendUVDistortionTexture)
						{
							data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Texture, texture_and_index, ref blendUVDistortionTexInfo).GetBytes());
						}
						else
						{
							data.Add((-1).GetBytes());
						}
					}
					else
					{
						data.Add((-1).GetBytes());
						data.Add((-1).GetBytes());
						data.Add((-1).GetBytes());
					}
				}
			}
			else if (value.Material.Value == Data.RendererCommonValues.MaterialType.BackDistortion)
			{
				// texture1
				data.Add(getTexIDAndStoreSize(value.ColorTexture, 1, distortionTexture_and_index).GetBytes());

				// texture2
				data.Add((-1).GetBytes());

				if (version >= ExporterVersion.Ver16Alpha1)
				{
					// alpha texture
					if (advanceValue.AlphaTextureParam.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.AlphaTextureParam.Texture, distortionTexture_and_index, ref alphaTexInfo).GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}

					// uv distortion texture
					if (advanceValue.UVDistortionTextureParam.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.UVDistortionTextureParam.Texture, distortionTexture_and_index, ref uvDistortionTexInfo).GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}

					// blend texture
					if (advanceValue.BlendTextureParams.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendTextureParam.Texture, distortionTexture_and_index, ref blendTexInfo).GetBytes());

						// blend alpha texture
						if (advanceValue.BlendTextureParams.EnableBlendAlphaTexture)
						{
							data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendAlphaTextureParam.Texture, distortionTexture_and_index, ref blendAlphaTexInfo).GetBytes());
						}
						else
						{
							data.Add((-1).GetBytes());
						}

						// blend uv distortion texture
						if (advanceValue.BlendTextureParams.EnableBlendUVDistortionTexture)
						{
							data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Texture, distortionTexture_and_index, ref blendUVDistortionTexInfo).GetBytes());
						}
						else
						{
							data.Add((-1).GetBytes());
						}
					}
					else
					{
						data.Add((-1).GetBytes());
						data.Add((-1).GetBytes());
						data.Add((-1).GetBytes());
					}
				}
			}
			else if (value.Material.Value == Data.RendererCommonValues.MaterialType.Lighting)
			{
				// texture1
				data.Add(getTexIDAndStoreSize(value.ColorTexture, 1, texture_and_index).GetBytes());

				// texture2
				data.Add(getTexIDAndStoreSize(value.NormalTexture, 2, normalTexture_and_index).GetBytes());

				if (version >= ExporterVersion.Ver16Alpha1)
				{
					// alpha texture
					if (advanceValue.AlphaTextureParam.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.AlphaTextureParam.Texture, texture_and_index, ref alphaTexInfo).GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}

					// uv distortion texture
					if (advanceValue.UVDistortionTextureParam.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.UVDistortionTextureParam.Texture, texture_and_index, ref uvDistortionTexInfo).GetBytes());
					}
					else
					{
						data.Add((-1).GetBytes());
					}

					// blend texture
					if (advanceValue.BlendTextureParams.Enabled)
					{
						data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendTextureParam.Texture, texture_and_index, ref blendTexInfo).GetBytes());

						// blend alpha texture
						if (advanceValue.BlendTextureParams.EnableBlendAlphaTexture)
						{
							data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendAlphaTextureParam.Texture, texture_and_index, ref blendAlphaTexInfo).GetBytes());
						}
						else
						{
							data.Add((-1).GetBytes());
						}

						// blend uv distortion texture
						if (advanceValue.BlendTextureParams.EnableBlendUVDistortionTexture)
						{
							data.Add(getTexIDAndInfo(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Texture, texture_and_index, ref blendUVDistortionTexInfo).GetBytes());
						}
						else
						{
							data.Add((-1).GetBytes());
						}
					}
					else
					{
						data.Add((-1).GetBytes());
						data.Add((-1).GetBytes());
						data.Add((-1).GetBytes());
					}
				}
			}
			else
			{
				var materialInfo = Core.ResourceCache.LoadMaterialInformation(value.MaterialFile.Path.AbsolutePath);
				if(materialInfo == null)
				{
					materialInfo = new MaterialInformation();
				}

				var textures = value.MaterialFile.GetTextures(materialInfo).Where(_ => _.Item1 != null).ToArray();
				var uniforms = value.MaterialFile.GetUniforms(materialInfo);

				// maximum slot limitation
				if(textures.Length > Constant.UserTextureSlotCount)
				{
					textures = textures.Take(Constant.UserTextureSlotCount).ToArray();
				}

				if(material_and_index.ContainsKey(value.MaterialFile.Path.RelativePath))
				{
					data.Add(material_and_index[value.MaterialFile.Path.RelativePath].GetBytes());
				}
				else
				{
					data.Add((-1).GetBytes());

				}

				data.Add(textures.Length.GetBytes());

				foreach (var texture in textures)
				{
					var texture_ = texture.Item1.Value as Data.Value.PathForImage;
					if (texture.Item2.Type == TextureType.Value)
					{
						data.Add((1).GetBytes());
						data.Add(getTexIDAndStoreSize(texture_, texture.Item2.Priority, normalTexture_and_index).GetBytes());
					}
					else
					{
						data.Add((0).GetBytes());
						data.Add(getTexIDAndStoreSize(texture_, texture.Item2.Priority, texture_and_index).GetBytes());

					}
				}

				data.Add(uniforms.Count.GetBytes());

				foreach (var uniform in uniforms)
				{
					float[] floats = new float[4];
					
					if(uniform.Item1 == null)
					{
						floats = uniform.Item2.DefaultValues.ToArray();
					}
					else if(uniform.Item1.Value is Data.Value.Float)
					{
						floats[0] = (uniform.Item1.Value as Data.Value.Float).Value;
					}
					else if (uniform.Item1.Value is Data.Value.Vector2D)
					{
						floats[0] = (uniform.Item1.Value as Data.Value.Vector2D).X.Value;
						floats[1] = (uniform.Item1.Value as Data.Value.Vector2D).Y.Value;
					}
					else if (uniform.Item1.Value is Data.Value.Vector3D)
					{
						floats[0] = (uniform.Item1.Value as Data.Value.Vector3D).X.Value;
						floats[1] = (uniform.Item1.Value as Data.Value.Vector3D).Y.Value;
						floats[2] = (uniform.Item1.Value as Data.Value.Vector3D).Z.Value;
					}
					else if (uniform.Item1.Value is Data.Value.Vector4D)
					{
						floats[0] = (uniform.Item1.Value as Data.Value.Vector4D).X.Value;
						floats[1] = (uniform.Item1.Value as Data.Value.Vector4D).Y.Value;
						floats[2] = (uniform.Item1.Value as Data.Value.Vector4D).Z.Value;
						floats[3] = (uniform.Item1.Value as Data.Value.Vector4D).W.Value;
					}

					data.Add(floats[0].GetBytes());
					data.Add(floats[1].GetBytes());
					data.Add(floats[2].GetBytes());
					data.Add(floats[3].GetBytes());
				}
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

				data.Add(advanceValue.BlendTextureParams.BlendTextureParam.Filter);
				data.Add(advanceValue.BlendTextureParams.BlendTextureParam.Wrap);

				data.Add(advanceValue.BlendTextureParams.BlendAlphaTextureParam.Filter);
				data.Add(advanceValue.BlendTextureParams.BlendAlphaTextureParam.Wrap);

				data.Add(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Filter);
				data.Add(advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.Wrap);
			}

			if (value.ZTest.GetValue())
			{
				data.Add((1).GetBytes());
			}
			else
			{
				data.Add((0).GetBytes());
			}

			if (value.ZWrite.GetValue())
			{
				data.Add((1).GetBytes());
			}
			else
			{
				data.Add((0).GetBytes());
			}

			data.Add(value.FadeInType);
			if (value.FadeInType.Value == Data.RendererCommonValues.FadeInMethod.Use)
			{
				data.Add(value.FadeIn.Frame.GetBytes());

				var easing = Utl.MathUtl.Easing((float)value.FadeIn.StartSpeed.Value, (float)value.FadeIn.EndSpeed.Value);
				data.Add(BitConverter.GetBytes(easing[0]));
				data.Add(BitConverter.GetBytes(easing[1]));
				data.Add(BitConverter.GetBytes(easing[2]));
			}

			data.Add(value.FadeOutType);
			if (value.FadeOutType.Value == Data.RendererCommonValues.FadeOutMethod.WithinLifetime ||
				value.FadeOutType.Value == Data.RendererCommonValues.FadeOutMethod.AfterRemoved)
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

				if (version >= ExporterVersion.Ver1600)
				{
					data.Add(value_.FlipbookInterpolationType);
				}
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
				data.Add((advanceValue.UVDistortionTextureParam.UVDistortionIntensity).GetBytes());

				// blend texture
				data.Add(GetUVBytes
				(
				blendTexInfo,
				advanceValue.BlendTextureParams.BlendTextureParam.UV,
				advanceValue.BlendTextureParams.BlendTextureParam.UVFixed,
				advanceValue.BlendTextureParams.BlendTextureParam.UVAnimation,
				advanceValue.BlendTextureParams.BlendTextureParam.UVScroll,
				advanceValue.BlendTextureParams.BlendTextureParam.UVFCurve
				));

				// blend texture blend type
				if (advanceValue.BlendTextureParams.Enabled && advanceValue.BlendTextureParams.BlendTextureParam.Texture.RelativePath != string.Empty)
				{
					data.Add(advanceValue.BlendTextureParams.BlendTextureParam.BlendType);
				}
				else
				{
					data.Add((-1).GetBytes());
				}

				// blend alpha texture
				data.Add(GetUVBytes
				(
					blendAlphaTexInfo,
					advanceValue.BlendTextureParams.BlendAlphaTextureParam.UV,
					advanceValue.BlendTextureParams.BlendAlphaTextureParam.UVFixed,
					advanceValue.BlendTextureParams.BlendAlphaTextureParam.UVAnimation,
					advanceValue.BlendTextureParams.BlendAlphaTextureParam.UVScroll,
					advanceValue.BlendTextureParams.BlendAlphaTextureParam.UVFCurve
				));

				// blend uv distoriton texture
				data.Add(GetUVBytes
				(
					blendUVDistortionTexInfo,
					advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.UV,
					advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.UVFixed,
					advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.UVAnimation,
					advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.UVScroll,
					advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.UVFCurve
				));

				// blend uv distoriton intensity
				data.Add((advanceValue.BlendTextureParams.BlendUVDistortionTextureParam.UVDistortionIntensity).GetBytes());
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

			if (_TexInfo.Width > 0 && _TexInfo.Height > 0)
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
					var bytes1 = value_.GetBytes(1.0f / width, 1.0f / height);
					data.Add(bytes1);
				}

				{
					var value_ = _FCurve.Size;
					var bytes1 = value_.GetBytes(1.0f / width, 1.0f / height);
					data.Add(bytes1);
				}
			}

			return data.ToArray().ToArray();
		}
	}

}
