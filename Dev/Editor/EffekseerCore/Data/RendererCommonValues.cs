using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class RendererCommonValues
	{
		[Name(language = Language.Japanese, value = "色/歪み画像")]
		[Description(language = Language.Japanese, value = "色/歪みを表す画像")]
		[Name(language = Language.English, value = "Texture")]
		[Description(language = Language.English, value = "Image that represents color/distortion")]
		public Value.PathForImage ColorTexture
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "ブレンド")]
		[Name(language = Language.English, value = "Blend")]
		public Value.Enum<AlphaBlendType> AlphaBlend { get; private set; }

		[Name(language = Language.Japanese, value = "フィルタ")]
		[Name(language = Language.English, value = "Filter")]
		public Value.Enum<FilterType> Filter { get; private set; }

		[Name(language = Language.Japanese, value = "外側")]
		[Name(language = Language.English, value = "Wrap")]
		public Value.Enum<WrapType> Wrap { get; private set; }

		[Name(language = Language.Japanese, value = "深度書き込み")]
		[Name(language = Language.English, value = "Depth Set")]
		public Value.Boolean ZWrite { get; private set; }

		[Name(language = Language.Japanese, value = "深度テスト")]
		[Name(language = Language.English, value = "Depth Test")]
		public Value.Boolean ZTest { get; private set; }

		[Selector(ID = 0)]
		[Name(language = Language.Japanese, value = "フェードイン")]
		[Name(language = Language.English, value = "Fade-In")]
		public Value.Enum<FadeType> FadeInType
		{
			get;
			private set;
		}

        [Selected(ID = 0, Value = 0)]
        [IO(Export = true)]
		public NoneParamater FadeInNone
        {
            get;
            private set;
        }

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public FadeInParamater FadeIn
		{
			get;
			private set;
		}

		[Selector(ID = 1)]
		[Name(language = Language.Japanese, value = "フェードアウト")]
		[Name(language = Language.English, value = "Fade-Out")]
		public Value.Enum<FadeType> FadeOutType
		{
			get;
			private set;
		}

		[Selected(ID = 1, Value = 0)]
		[IO(Export = true)]
		public NoneParamater FadeOutNone
		{
			get;
			private set;
		}

		[Selected(ID = 1, Value = 1)]
		[IO(Export = true)]
		public FadeOutParamater FadeOut
		{
			get;
			private set;
		}

		[Selector(ID = 2)]
		[Name(language = Language.Japanese, value = "UV")]
		[Name(language = Language.English, value = "UV")]
		public Value.Enum<UVType> UV
		{
			get;
			private set;
		}

		[Selected(ID = 2, Value = 0)]
		[IO(Export = true)]
		public UVDefaultParamater UVDefault { get; private set; }

		[Selected(ID = 2, Value = 1)]
		[IO(Export = true)]
		public UVFixedParamater UVFixed { get; private set; }

		[Selected(ID = 2, Value = 2)]
		[IO(Export = true)]
		public UVAnimationParamater UVAnimation { get; private set; }

		[Selected(ID = 2, Value = 3)]
		[IO(Export = true)]
		public UVScrollParamater UVScroll { get; private set; }

		[Name(language = Language.Japanese, value = "色への影響")]
		[Description(language = Language.Japanese, value = "親ノードからの色への影響")]
		[Name(language = Language.English, value = "Inherit Color")]
		[Description(language = Language.English, value = "When this instance should copy its parent node's color")]
		public Value.Enum<ParentEffectType> ColorInheritType
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "歪み")]
		[Name(language = Language.English, value = "Distortion")]
		public Value.Boolean Distortion { get; private set; }

		[Name(language = Language.Japanese, value = "歪み強度")]
		[Name(language = Language.English, value = "Distortion\nIntensity")]
		public Value.Float DistortionIntensity { get; private set; }

		internal RendererCommonValues()
		{
            ColorTexture = new Value.PathForImage(Properties.Resources.ImageFilter, true, "");
			
			AlphaBlend = new Value.Enum<AlphaBlendType>(AlphaBlendType.Blend);
			Filter = new Value.Enum<FilterType>(FilterType.Linear);
			Wrap = new Value.Enum<WrapType>(WrapType.Repeat);

			FadeInType = new Value.Enum<FadeType>(FadeType.None);
			FadeInNone = new NoneParamater();
			FadeIn = new FadeInParamater();

			FadeOutType = new Value.Enum<FadeType>();
			FadeOutNone = new NoneParamater();
			FadeOut = new FadeOutParamater();

			UV = new Value.Enum<UVType>();

			UVDefault = new UVDefaultParamater();
			UVFixed = new UVFixedParamater();
			UVAnimation = new UVAnimationParamater();
			UVScroll = new UVScrollParamater();

			ZWrite = new Value.Boolean(false);
			ZTest = new Value.Boolean(true);

			ColorInheritType = new Value.Enum<ParentEffectType>(ParentEffectType.NotBind);

			Distortion = new Value.Boolean(false);
			DistortionIntensity = new Value.Float(1.0f, float.MaxValue, float.MinValue, 0.1f);
		}

		public class NoneParamater
		{
			internal NoneParamater()
			{
			}
		}

		public class FadeInParamater
		{
			[Name(value = "フレーム数", language = Language.Japanese)]
			[Description(language = Language.Japanese, value = "生成からフェードインが終了するまでのフレーム数")]
			[Name(value = "Frame Count", language = Language.English)]
			[Description(language = Language.English, value = "Duration in frames of the fade-in transition")]
			public Value.Float Frame { get; private set; }

			[Name(language = Language.Japanese, value = "始点速度")]
			[Description(language = Language.Japanese, value = "始点速度")]
			[Name(language = Language.English, value = "Ease In")]
			[Description(language = Language.English, value = "Initial speed (of the tween)")]
			public Value.Enum<EasingStart> StartSpeed
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "終点速度")]
			[Description(language = Language.Japanese, value = "終点速度")]
			[Name(language = Language.English, value = "Ease Out")]
			[Description(language = Language.English, value = "Final speed (of the tween)")]
			public Value.Enum<EasingEnd> EndSpeed
			{
				get;
				private set;
			}

			public FadeInParamater()
			{
				Frame = new Value.Float(1, float.MaxValue, 0);
				StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
				EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
			}
		}

		public class FadeOutParamater
		{
			[Name(value = "フレーム数", language = Language.Japanese)]
			[Description(language = Language.Japanese, value = "フェードアウトが開始してから終了するまでのフレーム数")]
			[Name(value = "Frame Count", language = Language.English)]
			[Description(language = Language.English, value = "Duration in frames of the fade-out transition")]
			public Value.Float Frame { get; private set; }

			[Name(language = Language.Japanese, value = "始点速度")]
			[Description(language = Language.Japanese, value = "始点速度")]
			[Name(language = Language.English, value = "Ease In")]
			[Description(language = Language.English, value = "Initial speed (of the tween)")]
			public Value.Enum<EasingStart> StartSpeed
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "終点速度")]
			[Description(language = Language.Japanese, value = "終点速度")]
			[Name(language = Language.English, value = "Ease Out")]
			[Description(language = Language.English, value = "Final speed (of the tween)")]
			public Value.Enum<EasingEnd> EndSpeed
			{
				get;
				private set;
			}

			public FadeOutParamater()
			{
				Frame = new Value.Float(1, float.MaxValue, 0);
				StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
				EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
			}
		}

		public class UVDefaultParamater
		{
		}

		public class UVFixedParamater
		{
			[Name(value = "始点", language = Language.Japanese)]
			[Name(value = "Start", language = Language.English)]
			public Value.Vector2D Start { get; private set; }
			[Name(value = "大きさ", language = Language.Japanese)]
			[Name(value = "Size", language = Language.English)]
			public Value.Vector2D Size { get; private set; }

			public UVFixedParamater()
			{
				Start = new Value.Vector2D();
				Size = new Value.Vector2D();
			}
		}

		public class UVAnimationParamater
		{
			[Name(value = "始点", language = Language.Japanese)]
			[Name(value = "Start", language = Language.English)]
			public Value.Vector2D Start { get; private set; }
			[Name(value = "大きさ", language = Language.Japanese)]
			[Name(value = "Size", language = Language.English)]
			public Value.Vector2D Size { get; private set; }

			[Name(value = "1枚あたりの時間", language = Language.Japanese)]
			[Name(value = "Frame Length", language = Language.English)]
			public Value.Int FrameLength { get; private set; }

			[Name(value = "横方向枚数", language = Language.Japanese)]
			[Name(value = "X-Count", language = Language.English)]
			public Value.Int FrameCountX { get; private set; }

			[Name(value = "縦方向枚数", language = Language.Japanese)]
			[Name(value = "Y-Count", language = Language.English)]
			public Value.Int FrameCountY { get; private set; }

			[Name(value = "ループ", language = Language.Japanese)]
			[Name(value = "Loop", language = Language.English)]
			public Value.Enum<LoopType> LoopType { get; private set; }

			public UVAnimationParamater()
			{
				Start = new Value.Vector2D();
				Size = new Value.Vector2D();
				FrameLength = new Value.Int(1, int.MaxValue, 1);
				FrameCountX = new Value.Int(1, int.MaxValue, 1);
				FrameCountY = new Value.Int(1, int.MaxValue, 1);
				LoopType = new Value.Enum<LoopType>(RendererCommonValues.LoopType.Once);
			}
		}

		public class UVScrollParamater
		{
			[Name(value = "始点", language = Language.Japanese)]
			[Name(value = "Start", language = Language.English)]
			public Value.Vector2D Start { get; private set; }
			[Name(value = "大きさ", language = Language.Japanese)]
			[Name(value = "Size", language = Language.English)]
			public Value.Vector2D Size { get; private set; }

			[Name(value = "移動速度", language = Language.Japanese)]
			[Name(value = "Scroll Speed", language = Language.English)]
			public Value.Vector2D Speed { get; private set; }

			public UVScrollParamater()
			{
				Start = new Value.Vector2D();
				Size = new Value.Vector2D();
				Speed = new Value.Vector2D();
			}
		}

		public enum FadeType : int
		{
			[Name(value = "有り", language = Language.Japanese)]
			[Name(value = "Enabled", language = Language.English)]
			Use = 1,
			[Name(value = "無し", language = Language.Japanese)]
			[Name(value = "Disabled", language = Language.English)]
			None = 0,
		}

		public enum FilterType : int
		{
			[Name(value = "最近傍", language = Language.Japanese)]
			[Name(value = "Nearest-Neighbor", language = Language.English)]
			Nearest = 0,
			[Name(value = "線形", language = Language.Japanese)]
			[Name(value = "Linear Interpolation", language = Language.English)]
			Linear = 1,
		}

		public enum WrapType : int
		{
			[Name(value = "繰り返し", language = Language.Japanese)]
			[Name(value = "Repeat", language = Language.English)]
			Repeat = 0,
			[Name(value = "クランプ", language = Language.Japanese)]
			[Name(value = "Clamp", language = Language.English)]
			Clamp = 1,
		}

		public enum UVType : int
		{
			[Name(value = "標準", language = Language.Japanese)]
			[Name(value = "Standard", language = Language.English)]
			Default = 0,
			[Name(value = "固定", language = Language.Japanese)]
			[Name(value = "Fixed", language = Language.English)]
			Fixed = 1,
			[Name(value = "アニメーション", language = Language.Japanese)]
			[Name(value = "Animation", language = Language.English)]
			Animation = 2,
			[Name(value = "スクロール", language = Language.Japanese)]
			[Name(value = "Scroll", language = Language.English)]
			Scroll = 3,
		}

		public enum LoopType : int
		{
			[Name(value = "なし", language = Language.Japanese)]
			[Name(value = "None", language = Language.English)]
			Once = 0,
			[Name(value = "ループ", language = Language.Japanese)]
			[Name(value = "Loop", language = Language.English)]
			Loop = 1,
			[Name(value = "逆ループ", language = Language.Japanese)]
			[Name(value = "Reverse Loop", language = Language.English)]
			ReverceLoop = 2,
		}
	}
}
