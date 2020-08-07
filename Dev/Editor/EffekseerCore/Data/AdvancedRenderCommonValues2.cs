using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum AdvancedAlphaBlendType : int
	{
		[Key(key = "AdvancedAlphaBlendType_AlphaBlend")]
		Blend = 0,
		[Key(key = "AlphaBlendType_Add")]
		Add = 1,
		[Key(key = "AlphaBlendType_Sub")]
		Sub = 2,
		[Key(key = "AlphaBlendType_Mul")]
		Mul = 3,
	}

	public class BlendTextureParameter
	{
		[IO(Export = true)]
		[Key(key = "BlendTextureParameter_Texture")]
		public Value.PathForImage Texture { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendTextureParameter_BlendType")]
		public Value.Enum<AdvancedAlphaBlendType> BlendType { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendTextureParameter_Filter")]
		public Value.Enum<RendererCommonValues.FilterType> Filter { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendTextureParameter_Wrap")]
		public Value.Enum<RendererCommonValues.WrapType> Wrap { get; private set; }

		[Selector(ID = 103)]
		[IO(Export = true)]
		[Key(key = "BRS_UV4")]
		public Value.Enum<RendererCommonValues.UVType> UV { get; private set; }

		[Selected(ID = 103, Value = 0)]
		[IO(Export = true)]
		public RendererCommonValues.UVDefaultParamater UVDefault { get; private set; }

		[Selected(ID = 103, Value = 1)]
		[IO(Export = true)]
		public RendererCommonValues.UVFixedParamater UVFixed { get; private set; }

		[Selected(ID = 103, Value = 2)]
		[IO(Export = true)]
		public RendererCommonValues.UVAnimationParamater UVAnimation { get; private set; }

		[Selected(ID = 103, Value = 3)]
		[IO(Export = true)]
		public RendererCommonValues.UVScrollParamater UVScroll { get; private set; }

		[Selected(ID = 103, Value = 4)]
		[IO(Export = true)]
		public RendererCommonValues.UVFCurveParamater UVFCurve { get; private set; }

		public BlendTextureParameter(Value.Path basepath)
		{
			Texture = new Value.PathForImage(basepath, Resources.GetString("ImageFilter"), true, "");
			BlendType = new Value.Enum<AdvancedAlphaBlendType>(AdvancedAlphaBlendType.Blend);
			Filter = new Value.Enum<RendererCommonValues.FilterType>(RendererCommonValues.FilterType.Linear);
			Wrap = new Value.Enum<RendererCommonValues.WrapType>(RendererCommonValues.WrapType.Repeat);
			UV = new Value.Enum<RendererCommonValues.UVType>();
			UVDefault = new RendererCommonValues.UVDefaultParamater();
			UVFixed = new RendererCommonValues.UVFixedParamater();
			UVAnimation = new RendererCommonValues.UVAnimationParamater();
			UVScroll = new RendererCommonValues.UVScrollParamater();
			UVFCurve = new RendererCommonValues.UVFCurveParamater();
		}
	}

	public class BlendAlphaTextureParameter
	{
		[IO(Export = true)]
		[Key(key = "BlendAlphaTextureParameter_Texture")]
		public Value.PathForImage Texture { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendAlphaTextureParameter_Filter")]
		public Value.Enum<RendererCommonValues.FilterType> Filter { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendAlphaTextureParameter_Wrap")]
		public Value.Enum<RendererCommonValues.WrapType> Wrap { get; private set; }

		[Selector(ID = 104)]
		[IO(Export = true)]
		[Key(key = "BRS_UV5")]
		public Value.Enum<RendererCommonValues.UVType> UV { get; private set; }

		[Selected(ID = 104, Value = 0)]
		[IO(Export = true)]
		public RendererCommonValues.UVDefaultParamater UVDefault { get; private set; }

		[Selected(ID = 104, Value = 1)]
		[IO(Export = true)]
		public RendererCommonValues.UVFixedParamater UVFixed { get; private set; }

		[Selected(ID = 104, Value = 2)]
		[IO(Export = true)]
		public RendererCommonValues.UVAnimationParamater UVAnimation { get; private set; }

		[Selected(ID = 104, Value = 3)]
		[IO(Export = true)]
		public RendererCommonValues.UVScrollParamater UVScroll { get; private set; }

		[Selected(ID = 104, Value = 4)]
		[IO(Export = true)]
		public RendererCommonValues.UVFCurveParamater UVFCurve { get; private set; }

		public BlendAlphaTextureParameter(Value.Path basepath)
		{
			Texture = new Value.PathForImage(basepath, Resources.GetString("ImageFilter"), true, "");
			Filter = new Value.Enum<RendererCommonValues.FilterType>(RendererCommonValues.FilterType.Linear);
			Wrap = new Value.Enum<RendererCommonValues.WrapType>(RendererCommonValues.WrapType.Repeat);
			UV = new Value.Enum<RendererCommonValues.UVType>();
			UVDefault = new RendererCommonValues.UVDefaultParamater();
			UVFixed = new RendererCommonValues.UVFixedParamater();
			UVAnimation = new RendererCommonValues.UVAnimationParamater();
			UVScroll = new RendererCommonValues.UVScrollParamater();
			UVFCurve = new RendererCommonValues.UVFCurveParamater();
		}
	}

	public class BlendUVDistortionTextureParameter
	{
		[IO(Export = true)]
		[Key(key = "BlendUVDistortionTextureParameter_Texture")]
		public Value.PathForImage Texture { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendUVDistortionTextureParameter_Intensity")]
		public Value.Float UVDistortionIntensity { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendUVDistortionTextureParameter_Filter")]
		public Value.Enum<RendererCommonValues.FilterType> Filter { get; private set; }

		[IO(Export = true)]
		[Key(key = "BlendUVDistortionTextureParameter_Wrap")]
		public Value.Enum<RendererCommonValues.WrapType> Wrap { get; private set; }

		[Selector(ID = 105)]
		[IO(Export = true)]
		[Key(key = "BRS_UV6")]
		public Value.Enum<RendererCommonValues.UVType> UV { get; private set; }

		[Selected(ID = 105, Value = 0)]
		[IO(Export = true)]
		public RendererCommonValues.UVDefaultParamater UVDefault { get; private set; }

		[Selected(ID = 105, Value = 1)]
		[IO(Export = true)]
		public RendererCommonValues.UVFixedParamater UVFixed { get; private set; }

		[Selected(ID = 105, Value = 2)]
		[IO(Export = true)]
		public RendererCommonValues.UVAnimationParamater UVAnimation { get; private set; }

		[Selected(ID = 105, Value = 3)]
		[IO(Export = true)]
		public RendererCommonValues.UVScrollParamater UVScroll { get; private set; }

		[Selected(ID = 105, Value = 4)]
		[IO(Export = true)]
		public RendererCommonValues.UVFCurveParamater UVFCurve { get; private set; }

		public BlendUVDistortionTextureParameter(Value.Path basepath)
		{
			Texture = new Value.PathForImage(basepath, Resources.GetString("ImageFilter"), true, "");
			UVDistortionIntensity = new Value.Float(0.01f, 100.0f, -100.0f, 0.01f);
			Filter = new Value.Enum<RendererCommonValues.FilterType>(RendererCommonValues.FilterType.Linear);
			Wrap = new Value.Enum<RendererCommonValues.WrapType>(RendererCommonValues.WrapType.Repeat);
			UV = new Value.Enum<RendererCommonValues.UVType>();
			UVDefault = new RendererCommonValues.UVDefaultParamater();
			UVFixed = new RendererCommonValues.UVFixedParamater();
			UVAnimation = new RendererCommonValues.UVAnimationParamater();
			UVScroll = new RendererCommonValues.UVScrollParamater();
			UVFCurve = new RendererCommonValues.UVFCurveParamater();
		}
	}

	public class BlendTextureParameters
	{
		[IO(Export = true)]
		public BlendTextureParameter BlendTextureParam { get; private set; }

		[Selector(ID = 400)]
		[IO(Export = true)]
		[Key(key = "BlendTextureParameters_EnableBlendAlphaTexture")]
		public Value.Boolean EnableBlendAlphaTexture { get; private set; }

		[Selected(ID = 400, Value = 0)]
		[IO(Export = true)]
		public BlendAlphaTextureParameter BlendAlphaTextureParam { get; private set; }

		[Selector(ID = 500)]
		[IO(Export = true)]
		[Key(key = "BlendTextureParameters_EnableBlendUVDistortionTexture")]
		public Value.Boolean EnableBlendUVDistortionTexture { get; private set; }

		[Selected(ID = 500, Value = 0)]
		[IO(Export = true)]
		public BlendUVDistortionTextureParameter BlendUVDistortionTextureParam { get; private set; }

		public BlendTextureParameters(Value.Path basepath)
		{
			BlendTextureParam = new BlendTextureParameter(basepath);

			EnableBlendAlphaTexture = new Value.Boolean(false);
			BlendAlphaTextureParam = new BlendAlphaTextureParameter(basepath);

			EnableBlendUVDistortionTexture = new Value.Boolean(false);
			BlendUVDistortionTextureParam = new BlendUVDistortionTextureParameter(basepath);
		}
	}

	public class AdvancedRenderCommonValues2
	{
		[Selector(ID = 300)]
		[IO(Export = true)]
		[Key(key = "AdvancedRenderCommonValues_EnableBlendTexture")]
		public Value.Boolean EnableBlendTexture { get; private set; }

		[Selected(ID = 300, Value = 0)]
		[IO(Export = true)]
		public BlendTextureParameters BlendTextureParams { get; private set; }

        public AdvancedRenderCommonValues2(Value.Path basepath)
        {
			EnableBlendTexture = new Value.Boolean(false);
			BlendTextureParams = new BlendTextureParameters(basepath);
        }
    }
}
