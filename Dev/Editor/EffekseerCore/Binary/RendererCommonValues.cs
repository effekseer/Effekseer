using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;


namespace Effekseer.Binary
{
	class RendererCommonValues
	{
#if MATERIAL_ENABLED
		public static byte[] GetBytes(Data.RendererCommonValues value, Dictionary<string, int> texture_and_index, Dictionary<string, int> normalTexture_and_index, Dictionary<string, int> distortionTexture_and_index, Dictionary<string, int> material_and_index)
#else
		public static byte[] GetBytes(Data.RendererCommonValues value, Dictionary<string, int> texture_and_index, Dictionary<string, int> distortionTexture_and_index)
#endif
		{
			List<byte[]> data = new List<byte[]>();

			var texInfo = new TextureInformation();
			var hasTexture = true;

#if MATERIAL_ENABLED
			data.Add(((int)value.Material.Value).GetBytes());


			if (value.Material.Value == Data.RendererCommonValues.MaterialType.Default)
			{
				// テクスチャ番号
				if (value.Distortion.Value)
				{
					if (value.ColorTexture.RelativePath != string.Empty &&
					distortionTexture_and_index.ContainsKey(value.ColorTexture.RelativePath) &&
					texInfo.Load(value.ColorTexture.AbsolutePath))
					{
						data.Add(distortionTexture_and_index[value.ColorTexture.RelativePath].GetBytes());
						hasTexture = true;
					}
					else
					{
						data.Add((-1).GetBytes());
						hasTexture = false;
					}
				}
				else
				{
					if (value.ColorTexture.RelativePath != string.Empty &&
						texture_and_index.ContainsKey(value.ColorTexture.RelativePath) &&
						texInfo.Load(value.ColorTexture.AbsolutePath))
					{
						data.Add(texture_and_index[value.ColorTexture.RelativePath].GetBytes());
						hasTexture = true;
					}
					else
					{
						data.Add((-1).GetBytes());
						hasTexture = false;
					}
				}
			}
			else
			{
				var materialInfo = new Utl.MaterialInformation();
				materialInfo.Load(value.MaterialFile.Path.AbsolutePath);
	
				var textures = value.MaterialFile.GetTextures(materialInfo);
				var uniforms = value.MaterialFile.GetUniforms(materialInfo);

				if(material_and_index.ContainsKey(value.MaterialFile.Path.RelativePath))
				{
					data.Add(material_and_index[value.MaterialFile.Path.RelativePath].GetBytes());
				}
				else
				{
					data.Add((-1).GetBytes());

				}

				data.Add(textures.Count.GetBytes());

				foreach (var texture in textures)
				{
					var texture_ = texture.Item1.Value as Data.Value.PathForImage;
					if (texture.Item2)
					{

						if (texture_.RelativePath != string.Empty &&
							normalTexture_and_index.ContainsKey(texture_.RelativePath) &&
							texInfo.Load(texture_.AbsolutePath))
						{
							data.Add((1).GetBytes());
							data.Add(normalTexture_and_index[texture_.RelativePath].GetBytes());
							hasTexture = true;
						}
						else
						{
							data.Add((1).GetBytes());
							data.Add((-1).GetBytes());
							hasTexture = false;
						}
					}
					else
					{
						if (texture_.RelativePath != string.Empty &&
texture_and_index.ContainsKey(texture_.RelativePath) &&
texInfo.Load(texture_.AbsolutePath))
						{
							data.Add((0).GetBytes());
							data.Add(texture_and_index[texture_.RelativePath].GetBytes());
							hasTexture = true;
						}
						else
						{
							data.Add((0).GetBytes());
							data.Add((-1).GetBytes());
							hasTexture = false;
						}
					}
				}

				data.Add(uniforms.Count.GetBytes());

				foreach (var uniform in uniforms)
				{
					float[] floats = new float[4];
					
					if(uniform.Value is Data.Value.Float)
					{
						floats[0] = (uniform.Value as Data.Value.Float).Value;
					}

					if (uniform.Value is Data.Value.Vector4D)
					{
						floats[0] = (uniform.Value as Data.Value.Vector4D).X.Value;
						floats[1] = (uniform.Value as Data.Value.Vector4D).Y.Value;
						floats[2] = (uniform.Value as Data.Value.Vector4D).Z.Value;
						floats[3] = (uniform.Value as Data.Value.Vector4D).W.Value;
					}

					data.Add(floats[0].GetBytes());
					data.Add(floats[1].GetBytes());
					data.Add(floats[2].GetBytes());
					data.Add(floats[3].GetBytes());
				}
			}
#else

			// テクスチャ番号
			if (value.Distortion.Value)
			{
				if (value.ColorTexture.RelativePath != string.Empty &&
				distortionTexture_and_index.ContainsKey(value.ColorTexture.RelativePath))
				{
					if(texInfo.Load(value.ColorTexture.AbsolutePath))
					{
						data.Add(distortionTexture_and_index[value.ColorTexture.RelativePath].GetBytes());
						hasTexture = true;
					}
					else
					{
						Utils.LogFileNotFound(value.ColorTexture.AbsolutePath);
						data.Add((-1).GetBytes());
						hasTexture = false;
					}
				}
				else
				{
					data.Add((-1).GetBytes());
					hasTexture = false;
				}
			}
			else
			{
				if (value.ColorTexture.RelativePath != string.Empty &&
					texture_and_index.ContainsKey(value.ColorTexture.RelativePath))
				{
					if(texInfo.Load(value.ColorTexture.AbsolutePath))
					{
						data.Add(texture_and_index[value.ColorTexture.RelativePath].GetBytes());
						hasTexture = true;
					}
					else
					{
						Utils.LogFileNotFound(value.ColorTexture.AbsolutePath);
						data.Add((-1).GetBytes());
						hasTexture = false;
					}
				}
				else
				{
					data.Add((-1).GetBytes());
					hasTexture = false;
				}
			}
#endif

			data.Add(value.AlphaBlend);
			data.Add(value.Filter);
			data.Add(value.Wrap);

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

			if (hasTexture)
			{
				var width = (float)texInfo.Width;
				var height = (float)texInfo.Height;

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
						var bytes1 = value_.GetBytes(1.0f / width);
						List<byte[]> _data = new List<byte[]>();
						data.Add(bytes1);
					}

					{
						var value_ = value.UVFCurve.Size;
						var bytes1 = value_.GetBytes(1.0f / height);
						List<byte[]> _data = new List<byte[]>();
						data.Add(bytes1);
					}
				}
			}
			else
			{
				data.Add(((int)Data.RendererCommonValues.UVType.Default).GetBytes());
			}

			// Inheritance
			data.Add(value.ColorInheritType.GetValueAsInt().GetBytes());

			// 歪み
			if (value.Distortion.GetValue())
			{
				data.Add((1).GetBytes());
			}
			else
			{
				data.Add((0).GetBytes());
			}

			data.Add(value.DistortionIntensity.GetBytes());

			return data.ToArray().ToArray();
		}
	}
}
