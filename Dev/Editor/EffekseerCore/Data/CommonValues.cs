using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class CommonValues
	{
		[Key(key = "BasicSettings_MaxGeneration")]

		public Value.IntWithInifinite MaxGeneration
		{
			get;
			private set;
		}

#if __EFFEKSEER_BUILD_VERSION16__
		[Selector(ID = 0)]
		[Key(key = "BasicSettings_LocationEffectType")]
		public Value.Enum<TranslationParentEffectType> LocationEffectType
		{
			get;
			private set;
		}
#else
		[Key(key = "BasicSettings_LocationEffectType")]
		public Value.Enum<ParentEffectType> LocationEffectType
		{
			get;
			private set;
		}
#endif

#if __EFFEKSEER_BUILD_VERSION16__
		[Selected(ID = 0, Value = 4)]
		[Selected(ID = 0, Value = 5)]
		[IO(Export = true)]
		public SteeringBehaviorParameter SteeringBehaviorParam
		{
			get;
			private set;
		}

		public class SteeringBehaviorParameter
		{
			[Name(language = Language.Japanese, value = "最大追従速度")]
			[Name(language = Language.English, value = "Max Follow Speed")]
			[IO(Export = true)]
			public Value.FloatWithRandom MaxFollowSpeed
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "操舵速度(%)")]
			[Name(language = Language.English, value = "Steering Speed(%)")]
			[IO(Export = true)]
			public Value.FloatWithRandom SteeringSpeed
			{
				get;
				private set;
			}

			public SteeringBehaviorParameter()
			{
				MaxFollowSpeed = new Value.FloatWithRandom(10, float.MaxValue, 0.0f);
				SteeringSpeed = new Value.FloatWithRandom(30, 100, 0);
			}
		}
#endif

		[Key(key = "BasicSettings_RotationEffectType")]
		public Value.Enum<ParentEffectType> RotationEffectType
		{
			get;
			private set;
		}

		[Key(key = "BasicSettings_ScaleEffectType")]
		public Value.Enum<ParentEffectType> ScaleEffectType
		{
			get;
			private set;
		}

		[Key(key = "BasicSettings_RemoveWhenLifeIsExtinct")]
		public Value.Boolean RemoveWhenLifeIsExtinct
		{
			get;
			private set;
		}

		[Key(key = "BasicSettings_RemoveWhenParentIsRemoved")]
		public Value.Boolean RemoveWhenParentIsRemoved
		{
			get;
			private set;
		}

		[Key(key = "BasicSettings_RemoveWhenAllChildrenAreRemoved")]
		public Value.Boolean RemoveWhenAllChildrenAreRemoved
		{
			get;
			private set;
		}

		[Key(key = "BasicSettings_Life")]
		public Value.IntWithRandom Life
		{
			get;
			private set;
		}

		[Key(key = "BasicSettings_GenerationTime")]
		public Value.FloatWithRandom GenerationTime
		{
			get;
			private set;
		}

		[Key(key = "BasicSettings_GenerationTimeOffset")]
		public Value.FloatWithRandom GenerationTimeOffset
		{
			get;
			private set;
		}

		internal CommonValues()
		{
			MaxGeneration = new Value.IntWithInifinite(1, false, int.MaxValue, 1);
#if __EFFEKSEER_BUILD_VERSION16__
			LocationEffectType = new Value.Enum<TranslationParentEffectType>(TranslationParentEffectType.Already);
#else
			LocationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
#endif
			RotationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			ScaleEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RemoveWhenLifeIsExtinct = new Value.Boolean(true);
			RemoveWhenParentIsRemoved = new Value.Boolean(false);
			RemoveWhenAllChildrenAreRemoved = new Value.Boolean(false);
			Life = new Value.IntWithRandom(100, int.MaxValue, 1);
			GenerationTime = new Value.FloatWithRandom(1.0f, float.MaxValue, 0.00001f);
			GenerationTimeOffset = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);

#if __EFFEKSEER_BUILD_VERSION16__
			SteeringBehaviorParam = new SteeringBehaviorParameter();
#endif

			// dynamic parameter
			MaxGeneration.CanSelectDynamicEquation = true;
			Life.CanSelectDynamicEquation = true;
			GenerationTime.CanSelectDynamicEquation = true;
			GenerationTimeOffset.CanSelectDynamicEquation = true;
		}
	}
}
