using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Data.Group;

namespace Effekseer.Data
{
	public class AlphaTextureParameter : IUvCommandValues
	{
		[IO(Export = true)]
		[Key(key = "AlphaTextureParameter_Texture")]
		public Value.PathForImage Texture { get; private set; }

		[IO(Export = true)]
		[Key(key = "AlphaTextureParameter_Filter")]
		public Value.Enum<RendererCommonValues.FilterType> Filter { get; private set; }

		[IO(Export = true)]
		[Key(key = "AlphaTextureParameter_Wrap")]
		public Value.Enum<RendererCommonValues.WrapType> Wrap { get; private set; }

		[Selector(ID = 101)]
		[IO(Export = true)]
		[Key(key = "BRS_UV2")]
		public Value.Enum<RendererCommonValues.UVType> UV { get; private set; }

		[Selected(ID = 101, Value = 0)]
		[IO(Export = true)]
		public RendererCommonValues.UVDefaultParamater UVDefault { get; private set; }

		[Selected(ID = 101, Value = 1)]
		[IO(Export = true)]
		public RendererCommonValues.UVFixedParamater UVFixed { get; private set; }

		[Selected(ID = 101, Value = 2)]
		[IO(Export = true)]
		public RendererCommonValues.UVAnimationParamater UVAnimation { get; private set; }

		[Selected(ID = 101, Value = 3)]
		[IO(Export = true)]
		public RendererCommonValues.UVScrollParamater UVScroll { get; private set; }

		[Selected(ID = 101, Value = 4)]
		[IO(Export = true)]
		public RendererCommonValues.UVFCurveParamater UVFCurve { get; private set; }

		public AlphaTextureParameter(Value.Path basepath)
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

	public class UVDistortionTextureParameter : IUvCommandValues
	{
		[IO(Export = true)]
		[Key(key = "UVDistortionTextureParameter_Texture")]
		public Value.PathForImage Texture { get; private set; }

		[IO(Export = true)]
		[Key(key = "UVDistortionTextureParameter_Intensity")]
		public Value.Float UVDistortionIntensity { get; private set; }

		[IO(Export = true)]
		[Key(key = "UVDistortionTextureParameter_Filter")]
		public Value.Enum<RendererCommonValues.FilterType> Filter { get; private set; }

		[IO(Export = true)]
		[Key(key = "UVDistortionTextureParameter_Wrap")]
		public Value.Enum<RendererCommonValues.WrapType> Wrap { get; private set; }

		[Selector(ID = 102)]
		[IO(Export = true)]
		[Key(key = "BRS_UV3")]
		public Value.Enum<RendererCommonValues.UVType> UV { get; private set; }

		[Selected(ID = 102, Value = 0)]
		[IO(Export = true)]
		public RendererCommonValues.UVDefaultParamater UVDefault { get; private set; }

		[Selected(ID = 102, Value = 1)]
		[IO(Export = true)]
		public RendererCommonValues.UVFixedParamater UVFixed { get; private set; }

		[Selected(ID = 102, Value = 2)]
		[IO(Export = true)]
		public RendererCommonValues.UVAnimationParamater UVAnimation { get; private set; }

		[Selected(ID = 102, Value = 3)]
		[IO(Export = true)]
		public RendererCommonValues.UVScrollParamater UVScroll { get; private set; }

		[Selected(ID = 102, Value = 4)]
		[IO(Export = true)]
		public RendererCommonValues.UVFCurveParamater UVFCurve { get; private set; }

		public UVDistortionTextureParameter(Value.Path basepath)
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

	public class AlphaCutoffParameter
	{
		[Selector(ID = 0)]
		[IO(Export = true)]
		[Key(key = "AlphaCutoffParameter_ParameterType")]
		public Value.Enum<ParameterType> Type { get; private set; }

		[Selected(ID = 0, Value = 0)]
		[IO(Export = true)]
		public FixedParameter Fixed { get; private set; }

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public FourPointInterpolationParameter FourPointInterpolation { get; private set; }

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		public FloatEasingParamater Easing { get; private set; }

		[Selected(ID = 0, Value = 3)]
		[IO(Export = true)]
		[Key(key = "FCurve")]
		public Value.FCurveScalar FCurve { get; private set; }

		[Selected(ID = 0, Value = 0)]
		[Selected(ID = 0, Value = 1)]
		[Selected(ID = 0, Value = 2)]
		[Selected(ID = 0, Value = 3)]
		[IO(Export = true)]
		public EdgeParameter EdgeParam { get; private set; }

		public class FixedParameter
		{
			[IO(Export = true)]
			[Key(key = "AlphaCutoffParameter_FixedParameter_Fixed")]
			public Value.Float Threshold { get; private set; }

			internal FixedParameter()
			{
				Threshold = new Value.Float(0.0f, 1.0f, 0.0f, 0.05f);
			}
		}

		public class FourPointInterpolationParameter
		{
			[IO(Export = true)]
			[Key(key = "AlphaCuroffParameter_FPIParameter_BeginThreshold")]
			public Value.FloatWithRandom BeginThreshold { get; private set; }

			[IO(Export = true)]
			[Key(key = "AlphaCuroffParameter_FPIParameter_TransitionFrameNum")]
			public Value.IntWithRandom TransitionFrameNum { get; private set; }

			[IO(Export = true)]
			[Key(key = "AlphaCuroffParameter_FPIParameter_No2Threshold")]
			public Value.FloatWithRandom No2Threshold { get; private set; }

			[IO(Export = true)]
			[Key(key = "AlphaCuroffParameter_FPIParameter_No3Threshold")]
			public Value.FloatWithRandom No3Threshold { get; private set; }

			[IO(Export = true)]
			[Key(key = "AlphaCuroffParameter_FPIParameter_TransitionFrameNum2")]
			public Value.IntWithRandom TransitionFrameNum2 { get; private set; }

			[IO(Export = true)]
			[Key(key = "AlphaCuroffParameter_FPIParameter_EndThreshold")]
			public Value.FloatWithRandom EndThreshold { get; private set; }


			internal FourPointInterpolationParameter()
			{
				BeginThreshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
				TransitionFrameNum = new Value.IntWithRandom(0, int.MaxValue, 0);
				No2Threshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
				No3Threshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
				TransitionFrameNum2 = new Value.IntWithRandom(0, int.MaxValue, 0);
				EndThreshold = new Value.FloatWithRandom(0.0f, 1.0f, 0.0f, DrawnAs.CenterAndAmplitude, 0.05f);
			}
		}

		public class EdgeParameter
		{
			[IO(Export = true)]
			[Key(key = "AlphaCutoffParameter_EdgeParameter_Threshold")]
			public Value.Float EdgeThreshold { get; private set; }

			[IO(Export = true)]
			[Key(key = "AlphaCutoffParameter_EdgeParameter_Color")]
			public Value.Color EdgeColor { get; private set; }

			[IO(Export = true)]
			[Key(key = "AlphaCutoffParameter_EdgeParameter_ColorScaling")]
			public Value.Int EdgeColorScaling { get; private set; }

			public EdgeParameter()
			{
				EdgeThreshold = new Value.Float(0.0f, 1.0f, 0.0f, 0.01f);
				EdgeColor = new Value.Color(255, 255, 255, 255);
				EdgeColorScaling = new Value.Int(1, int.MaxValue, 0);
			}
		}

		public enum ParameterType : int
		{
			[Key(key = "AlphaCutoffParameter_ParameterType_Fixed")]
			Fixed = 0,

			[Key(key = "AlphaCutoffParameter_ParameterType_FPI")]
			FourPointInterpolation = 1,

			[Key(key = "AlphaCutoffParameter_ParameterType_Easing")]
			Easing = 2,

			[Key(key = "AlphaCutoffParameter_ParameterType_FCurve")]
			FCurve = 3,
		}

		public AlphaCutoffParameter()
		{
			Type = new Value.Enum<ParameterType>(ParameterType.Fixed);
			Fixed = new FixedParameter();
			FourPointInterpolation = new FourPointInterpolationParameter();
			Easing = new FloatEasingParamater(0.0f, 1.0f, 0.0f);
			FCurve = new Value.FCurveScalar(0.0f, 100.0f);

			Fixed.Threshold.CanSelectDynamicEquation = true;
			Easing.Start.CanSelectDynamicEquation = true;
			Easing.End.CanSelectDynamicEquation = true;

			EdgeParam = new EdgeParameter();
		}
	}

	public class FalloffParameter
	{
		[IO(Export = true)]
		[Key(key = "FalloffParameter_ColorBlendType")]
		public Value.Enum<BlendType> ColorBlendType { get; private set; }

		[IO(Export = true)]
		[Key(key = "FalloffParameter_BeginColor")]
		public Value.Color BeginColor { get; private set; }

		[IO(Export = true)]
		[Key(key = "FalloffParameter_EndColor")]
		public Value.Color EndColor { get; private set; }

		[IO(Export = true)]
		[Key(key = "FalloffParameter_Pow")]
		public Value.Float Pow { get; private set; }

		public FalloffParameter()
		{
			ColorBlendType = new Value.Enum<BlendType>(BlendType.Add);
			BeginColor = new Value.Color(0, 0, 0, 255);
			EndColor = new Value.Color(255, 255, 255, 255);
			Pow = new Value.Float(1, 100, 1);
		}

		public enum BlendType : int
		{
			[Key(key = "FalloffParameter_BlendType_Add")]
			Add = 0,

			[Key(key = "FalloffParameter_BlendType_Sub")]
			Sub = 1,

			[Key(key = "FalloffParameter_BlendType_Mul")]
			Mul = 2,
		}
	}


	public class AdvancedRenderCommonValues
	{
		const int FalloffParameterID = 1000;

		[Selector(ID = 100)]
		[IO(Export = true)]
		[Key(key = "AdvancedRenderCommonValues_EnableAlphaTexture")]
		public Value.Boolean EnableAlphaTexture { get; private set; }

		[IO(Export = true)]
		[Selected(ID = 100, Value = 0)]
		public AlphaTextureParameter AlphaTextureParam { get; private set; }


		[Selector(ID = 200)]
		[IO(Export = true)]
		[Key(key = "AdvancedRenderCommonValues_EnableUVDistortionTexture")]
		public Value.Boolean EnableUVDistortionTexture { get; private set; }

		[IO(Export = true)]
		[Selected(ID = 200, Value = 0)]
		public UVDistortionTextureParameter UVDistortionTextureParam { get; private set; }

		[IO(Export = true)]
		public AlphaCutoffParameter AlphaCutoffParam { get; private set; }

		[Selector(ID = FalloffParameterID)]
		[IO(Export = true)]
		[Key(key = "ModelParameter_EnableFalloff")]
		public Value.Boolean EnableFalloff { get; private set; } = new Value.Boolean(false);

		[Selected(ID = FalloffParameterID, Value = 0)]
		[IO(Export = true)]
		public FalloffParameter FalloffParam { get; private set; } = new FalloffParameter();

		[Key(key = "AdvancedRenderCommonValues_SoftParticleDistance")]
		public Value.Float SoftParticleDistance { get; private set; } = new Value.Float(0.0f, float.MaxValue, 0.0f, 0.1f);

		public AdvancedRenderCommonValues(Value.Path basepath)
        {
			EnableAlphaTexture = new Value.Boolean(false);
			AlphaTextureParam = new AlphaTextureParameter(basepath);

			EnableUVDistortionTexture = new Value.Boolean(false);
			UVDistortionTextureParam = new UVDistortionTextureParameter(basepath);

			AlphaCutoffParam = new AlphaCutoffParameter();
        }
    }
}
