using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;


namespace Effekseer.Binary
{
	class RendererCommonValues
	{
		public static byte[] GetBytes(Data.RendererCommonValues value, Dictionary<string, int> texture_and_index)
		{
			List<byte[]> data = new List<byte[]>();

			var texInfo = new TextureInformation();
			var hasColorTexture = true;

			// テクスチャ番号
			if (value.ColorTexture.RelativePath != string.Empty && 
				texture_and_index.ContainsKey(value.ColorTexture.RelativePath) &&
				texInfo.Load(value.ColorTexture.AbsolutePath))
			{
				data.Add(texture_and_index[value.ColorTexture.RelativePath].GetBytes());
				hasColorTexture = true;
			}
			else
			{
				data.Add((-1).GetBytes());
				hasColorTexture = false;
			}

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

			if (hasColorTexture)
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

					data.Add(value_.FrameLength.Value.GetBytes());
					data.Add(value_.FrameCountX.Value.GetBytes());
					data.Add(value_.FrameCountY.Value.GetBytes());
					data.Add(value_.LoopType);
				}
				else if (value.UV.Value == Data.RendererCommonValues.UVType.Scroll)
				{
					var value_ = value.UVScroll;
					data.Add((value_.Start.X / width).GetBytes());
					data.Add((value_.Start.Y / height).GetBytes());
					data.Add((value_.Size.X / width).GetBytes());
					data.Add((value_.Size.Y / height).GetBytes());

					data.Add((value_.Speed.X / width).GetBytes());
					data.Add((value_.Speed.Y / height).GetBytes());
				}
			}
			else
			{
				data.Add(((int)Data.RendererCommonValues.UVType.Default).GetBytes());
			}

			// 歪み
			if (value.Distortion.GetValue())
			{
				data.Add((1).GetBytes());
			}
			else
			{
				data.Add((0).GetBytes());
			}

			return data.ToArray().ToArray();
		}
	}
}
