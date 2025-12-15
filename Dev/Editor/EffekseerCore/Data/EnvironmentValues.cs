using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class EnvironmentGroundValues
	{
		[Key(key = "EnvironmentGround_IsShown")]
		[Undo(Undo = false)]
		public Value.Boolean IsShown { get; private set; } = new Value.Boolean(false);

		[Key(key = "EnvironmentGround_IsCollisionEnabled")]
		[Undo(Undo = false)]
		public Value.Boolean IsCollisionEnabled { get; private set; } = new Value.Boolean(false);

		[Key(key = "EnvironmentGround_Height")]
		[Undo(Undo = false)]
		public Value.Float Height { get; private set; } = new Value.Float(0.0f, 1000.0f, -1000.0f);

		[Key(key = "EnvironmentGround_Extent")]
		[Undo(Undo = false)]
		public Value.Int Extent { get; private set; } = new Value.Int(10, 1000, 1);
	}

	public class EnvironmentBackgroundValues
	{
		[Key(key = "Environment_BackgroundColor")]
		[Undo(Undo = false)]
		public Value.Color BackgroundColor
		{
			get;
			private set;
		}

		/// <summary>
		/// this value is initialized lazily because it cannot decide using language in the constructor
		/// </summary>
		Value.PathForImage LasyBackgroundImage;

		[Key(key = "Environment_BackgroundImage")]
		[Undo(Undo = false)]
		public Value.PathForImage BackgroundImage
		{
			get
			{
				if (LasyBackgroundImage == null)
				{
					LasyBackgroundImage = new Value.PathForImage(null, new MultiLanguageString("ImageFilter"), false, "");
				}
				return LasyBackgroundImage;
			}
		}

		public EnvironmentBackgroundValues()
		{
			BackgroundColor = new Value.Color(0, 0, 0, 255);
		}
	}

	public class EnvironmentLightingValues
	{
		[Key(key = "Environment_LightDirection")]
		[Undo(Undo = false)]
		public Value.Vector3D LightDirection
		{
			get;
			private set;
		}

		[Key(key = "Environment_LightColor")]
		[Undo(Undo = false)]
		public Value.Color LightColor
		{
			get;
			private set;
		}

		[Key(key = "Environment_LightAmbientColor")]
		[Undo(Undo = false)]
		public Value.Color LightAmbientColor
		{
			get;
			private set;
		}

		public EnvironmentLightingValues()
		{
			LightDirection = new Value.Vector3D(1, 1, 1, 1, -1, 1, -1, 1, -1, 0.1f, 0.1f, 0.1f);
			LightColor = new Value.Color(215, 215, 215, 255);
			LightAmbientColor = new Value.Color(40, 40, 40, 255);
		}
	}

	public class EnvironmentPostEffectValues
	{
		[Selector(ID = 0)]
		[Key(key = "Environment_BloomSwitch")]
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
		[Key(key = "Environment_TonemapSelector")]
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

		public EnvironmentPostEffectValues()
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
			[Key(key = "EffectSwitch_Off")]
			Off = 0,
			[Key(key = "EffectSwitch_On")]
			On = 1,
		}

		public enum TonemapAlgorithm : int
		{
			[Key(key = "TonemapAlgorithm_Off")]
			Off = 0,
			[Key(key = "TonemapAlgorithm_Reinhard")]
			Reinhard = 1,
		}

		public class BloomParamater
		{
			[Key(key = "Environment_Bloom_Intensity")]
			[Undo(Undo = false)]
			public Value.Float Intensity
			{
				get;
				private set;
			}

			[Key(key = "Environment_Bloom_Threshold")]
			[Undo(Undo = false)]
			public Value.Float Threshold
			{
				get;
				private set;
			}

			[Key(key = "Environment_Bloom_SoftKnee")]
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
				SoftKnee = new Value.Float(0.5f, 1.0f, 0.0f, 0.1f);
			}
		}

		public class TonemapReinhardParameter
		{

			[Key(key = "Environment_TonemapReinhard_Exposure")]
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

	public class EnvironmentValues
	{
		[IO(Export = true, Import = true)]
		[TreeNode(key = "Environment_Background", id = "Environment_Background")]
		public EnvironmentBackgroundValues Background { get; private set; }

		[IO(Export = true, Import = true)]
		[TreeNode(key = "Environment_Ground", id = "Environment_Ground")]
		public EnvironmentGroundValues Ground { get; private set; } = new EnvironmentGroundValues();

		[IO(Export = true, Import = true)]
		[TreeNode(key = "Environment_Lighting", id = "Environment_Lighting")]
		public EnvironmentLightingValues Lighting { get; private set; }

		[IO(Export = true, Import = true)]
		[TreeNode(key = "Environment_PostEffect", id = "Environment_PostEffect")]
		public EnvironmentPostEffectValues PostEffect { get; private set; }

		public EnvironmentValues()
		{
			Background = new EnvironmentBackgroundValues();

			Lighting = new EnvironmentLightingValues();

			PostEffect = new EnvironmentPostEffectValues();
		}
	}
}
