using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Data.Group;

namespace Effekseer.Data
{
	public class AlphaTextureParameter : IUvCommandValues, IToggleMode
	{
		[IO(Export = true)]
		[Shown(Shown = false)]
		public Value.Boolean Enabled { get; private set; } = new Value.Boolean(false);

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

	public class UVDistortionTextureParameter : IUvCommandValues, IToggleMode
	{
		[IO(Export = true)]
		[Shown(Shown = false)]
		public Value.Boolean Enabled { get; private set; } = new Value.Boolean(false);

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

	public class AlphaCutoffParameter : IToggleMode
	{
		[IO(Export = true)]
		[Shown(Shown = false)]
		public Value.Boolean Enabled { get; private set; } = new Value.Boolean(false);

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
		[TreeNode(id = "AlphaCutoffParameter_Easing", key = "Easing_Parameter", type = TreeNodeType.Small)]
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
			public Value.Float EdgeColorScaling { get; private set; }

			public EdgeParameter()
			{
				EdgeThreshold = new Value.Float(0.0f, 1.0f, 0.0f, 0.01f);
				EdgeColor = new Value.Color(255, 255, 255, 255);
				EdgeColorScaling = new Value.Float(1, int.MaxValue, 0);
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

	public class FalloffParameter : IToggleMode
	{
		[IO(Export = true)]
		[Shown(Shown = false)]
		public Value.Boolean Enabled { get; private set; } = new Value.Boolean(false);

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

	public class BlendTextureParameter : IUvCommandValues
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

	public class BlendAlphaTextureParameter : IUvCommandValues
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

	public class BlendUVDistortionTextureParameter : IUvCommandValues
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

	public class BlendTextureParameters : IToggleMode
	{
		[IO(Export = true)]
		[Shown(Shown = false)]
		public Value.Boolean Enabled { get; private set; } = new Value.Boolean(false);

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

	public class SoftParticleParameters : IToggleMode
	{
		[IO(Export = true)]
		[Shown(Shown = false)]
		public Value.Boolean Enabled { get; private set; } = new Value.Boolean(false);

		[IO(Export = true)]
		[Key(key = "SoftParticleParameters_DistanceFar")]
		public Value.Float Distance { get; private set; } = new Value.Float(0.0f, float.MaxValue, 0.0f, 0.1f);

		[IO(Export = true)]
		[Key(key = "SoftParticleParameters_DistanceNear")]
		public Value.Float DistanceNear { get; private set; } = new Value.Float(0.0f, float.MaxValue, 0.0f, 0.1f);

		[IO(Export = true)]
		[Key(key = "SoftParticleParameters_DistanceNearOffset")]
		public Value.Float DistanceNearOffset { get; private set; } = new Value.Float(0.0f, float.MaxValue, 0.0f, 0.1f);
	}

	public class AdvancedRenderCommonValues
	{
		const int FalloffParameterID = 1000;

		[IO(Export = true)]
		[TreeNode(id = "AlphaTextureParameter_Texture_Name", key = "AlphaTextureParameter_Texture_Name")]
		public AlphaTextureParameter AlphaTextureParam { get; private set; }

		[IO(Export = true)]
		[TreeNode(id = "UVDistortionTextureParameter_Texture_Name", key = "UVDistortionTextureParameter_Texture_Name")]
		public UVDistortionTextureParameter UVDistortionTextureParam { get; private set; }

		[IO(Export = true)]
		[TreeNode(id = "AlphaCutoffParameter_ParameterType_Name", key = "AlphaCutoffParameter_ParameterType_Name")]
		public AlphaCutoffParameter AlphaCutoffParam { get; private set; }

		[IO(Export = true)]
		[TreeNode(id = "FalloffParameter_Name", key = "FalloffParameter_Name")]
		public FalloffParameter FalloffParam { get; private set; } = new FalloffParameter();

		[IO(Export = true)]
		[TreeNode(id = "BlendTextureParameters_Name", key = "BlendTextureParameters_Name")]
		public BlendTextureParameters BlendTextureParams { get; private set; }

		[IO(Export = true)]
		[TreeNode(id = "SoftParticleParameters_Name", key = "SoftParticleParameters_Name")]
		public SoftParticleParameters SoftParticleParams { get; private set; }

		public AdvancedRenderCommonValues(Value.Path basepath)
        {
			AlphaTextureParam = new AlphaTextureParameter(basepath);
			UVDistortionTextureParam = new UVDistortionTextureParameter(basepath);
			AlphaCutoffParam = new AlphaCutoffParameter();
			BlendTextureParams = new BlendTextureParameters(basepath);
			SoftParticleParams = new SoftParticleParameters();
		}
    }
}
