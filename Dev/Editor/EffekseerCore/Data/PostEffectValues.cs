using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class PostEffectValues
	{
		
		[Selector(ID = 0)]
		[Name(language = Language.Japanese, value = "ブルーム")]
		[Description(language = Language.Japanese, value = "明るい光源からの光が周囲に漏れるように見えるエフェクト")]
		[Name(language = Language.English, value = "Bloom")]
		[Description(language = Language.English, value = "The effect produces fringes of light extending from the borders of bright areas in an image.")]
		[Undo(Undo = false)]
		public Value.Enum<EffectSwitch> BloomSwitch
		{
			get;
			private set;
		}

        [Selected(ID = 0, Value = 0)]
        [IO(Export = true)]
		public NoneParamater BloomNone
        {
            get;
            private set;
        }

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public BloomParamater Bloom
		{
			get;
			private set;
		}
		
		[Selector(ID = 0)]
		[Name(language = Language.Japanese, value = "トーンマッピング")]
		[Description(language = Language.Japanese, value = "")]
		[Name(language = Language.English, value = "Tone mapping")]
		[Description(language = Language.English, value = "")]
		[Undo(Undo = false)]
		public Value.Enum<TonemapAlgorithm> TonemapSelector
		{
			get;
			private set;
		}
		
        [Selected(ID = 0, Value = 0)]
        [IO(Export = true)]
		public NoneParamater TonemapNone
        {
            get;
            private set;
        }

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public TonemapReinhardParameter TonemapReinhard
		{
			get;
			private set;
		}

        public PostEffectValues()
		{
			BloomSwitch = new Value.Enum<EffectSwitch>();
			BloomNone = new NoneParamater();
			Bloom = new BloomParamater();
			TonemapSelector = new Value.Enum<TonemapAlgorithm>();
			TonemapNone = new NoneParamater();
			TonemapReinhard = new TonemapReinhardParameter();
		}

		
		public class NoneParamater
		{
			internal NoneParamater()
			{
			}
		}
		
		public enum EffectSwitch : int
		{
			[Name(value = "Off", language = Language.Japanese)]
			[Name(value = "Off", language = Language.English)]
			Off = 0,
			[Name(value = "On", language = Language.Japanese)]
			[Name(value = "On", language = Language.English)]
			On = 1,
		}
		
		public enum TonemapAlgorithm : int
		{
			[Name(value = "Off", language = Language.Japanese)]
			[Name(value = "Off", language = Language.English)]
			Off = 0,
			[Name(value = "Reinhard", language = Language.Japanese)]
			[Name(value = "Reinhard", language = Language.English)]
			Reinhard = 1,
		}

		public class BloomParamater
		{
			[Name(language = Language.Japanese, value = "明るさ")]
			[Description(language = Language.Japanese, value = "ブルームの明るさ")]
			[Name(language = Language.English, value = "Intensity")]
			[Description(language = Language.English, value = "")]
			[Undo(Undo = false)]
			public Value.Float Intensity
			{
				get;
				private set;
			}
		
			[Name(language = Language.Japanese, value = "しきい値")]
			[Description(language = Language.Japanese, value = "ブルームのしきい値")]
			[Name(language = Language.English, value = "Threshold")]
			[Description(language = Language.English, value = "")]
			[Undo(Undo = false)]
			public Value.Float Threshold
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "緩やかさ")]
			[Description(language = Language.Japanese, value = "ブルームのしきい値付近の緩やかさ")]
			[Name(language = Language.English, value = "Soft Knee")]
			[Description(language = Language.English, value = "")]
			[Undo(Undo = false)]
			public Value.Float SoftKnee
			{
				get;
				private set;
			}
			
			internal BloomParamater()
			{
				Intensity = new Value.Float(1.0f, 100.0f, 0.0f, 0.1f);
				Threshold = new Value.Float(1.0f, 100.0f, 0.0f, 0.1f);
				SoftKnee  = new Value.Float(0.5f, 1.0f, 0.0f, 0.1f);
			}
		}
		
		public class TonemapReinhardParameter
		{
			[Name(language = Language.Japanese, value = "露光")]
			[Description(language = Language.Japanese, value = "")]
			[Name(language = Language.English, value = "Exposure")]
			[Description(language = Language.English, value = "")]
			[Undo(Undo = false)]
			public Value.Float Exposure
			{
				get;
				private set;
			}
			
			internal TonemapReinhardParameter()
			{
				Exposure = new Value.Float(1.0f, 100.0f, 0.0f, 0.1f);
			}
		}
	}
}
