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

		[Selector(ID = 0)]
		[Key(key = "BasicSettings_LocationEffectType")]
		public Value.Enum<TranslationParentEffectType> LocationEffectType
		{
			get;
			private set;
		}

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
			[Key(key = "BasicSettings_SteeringBehaviorParemeter_MaxFollowSpeed")]
			[IO(Export = true)]
			public Value.FloatWithRandom MaxFollowSpeed
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_SteeringBehaviorParameter_SteeringSpeed")]
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

		public class TriggerParameter
		{
			[Key(key = "BasicSettings_Trigger_GenerationEnabled")]
			public Value.Boolean GenerationEnabled
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Trigger_ToStartGeneration")]
			public Value.Enum<TriggerType> ToStartGeneration
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Trigger_ToStopGeneration")]
			public Value.Enum<TriggerType> ToStopGeneration
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Trigger_ToRemove")]
			public Value.Enum<TriggerType> ToRemove
			{
				get;
				private set;
			}

			public TriggerParameter()
			{
				GenerationEnabled = new Value.Boolean(true);
				ToStartGeneration = new Value.Enum<TriggerType>(TriggerType.None);
				ToStopGeneration = new Value.Enum<TriggerType>(TriggerType.None);
				ToRemove = new Value.Enum<TriggerType>(TriggerType.None);
			}
		}

		[IO(Export = true)]
		public TriggerParameter TriggerParam
		{
			get;
			private set;
		}

		internal CommonValues()
		{
			MaxGeneration = new Value.IntWithInifinite(1, false, int.MaxValue, 1);
			LocationEffectType = new Value.Enum<TranslationParentEffectType>(TranslationParentEffectType.Already);
			RotationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			ScaleEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RemoveWhenLifeIsExtinct = new Value.Boolean(true);
			RemoveWhenParentIsRemoved = new Value.Boolean(false);
			RemoveWhenAllChildrenAreRemoved = new Value.Boolean(false);
			Life = new Value.IntWithRandom(100, int.MaxValue, 1);
			GenerationTime = new Value.FloatWithRandom(1.0f, float.MaxValue, 0.00001f);
			GenerationTimeOffset = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);
			SteeringBehaviorParam = new SteeringBehaviorParameter();
			TriggerParam = new TriggerParameter();

			// dynamic parameter
			MaxGeneration.CanSelectDynamicEquation = true;
			Life.CanSelectDynamicEquation = true;
			GenerationTime.CanSelectDynamicEquation = true;
			GenerationTimeOffset.CanSelectDynamicEquation = true;
		}
	}
}
