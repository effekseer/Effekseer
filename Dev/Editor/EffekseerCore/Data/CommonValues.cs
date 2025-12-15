using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using Effekseer.InternalScript;

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

		public enum GenerationTimingType
		{
			[Key(key = "BasicSettings_GenerationTiming_Continuous")]
			Continuous,
			[Key(key = "BasicSettings_GenerationTiming_Trigger")]
			Trigger,
		}

		public class RemovalValues
		{
			[Key(key = "BasicSettings_Removal_WhenLifeIsExtinct")]
			public Value.Boolean WhenLifeIsExtinct
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Removal_WhenParentIsRemoved")]
			public Value.Boolean WhenParentIsRemoved
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Removal_WhenAllChildrenAreRemoved")]
			public Value.Boolean WhenAllChildrenAreRemoved
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Removal_TriggerToRemove")]
			public Value.Enum<TriggerType> TriggerToRemove
			{
				get;
				private set;
			}

			public RemovalValues()
			{
				WhenLifeIsExtinct = new Value.Boolean(true);
				WhenParentIsRemoved = new Value.Boolean(false);
				WhenAllChildrenAreRemoved = new Value.Boolean(false);
				TriggerToRemove = new Value.Enum<TriggerType>(TriggerType.None);
			}
		}

		public class GenerationValues
		{
			[Selector(ID = 0)]
			[Key(key = "BasicSettings_GenerationTiming")]
			[Name(value = "Generation Mode")]
			public Value.Enum<GenerationTimingType> Timing
			{
				get;
				private set;
			}

			[Selected(ID = 0, Value = (int)GenerationTimingType.Continuous)]
			[Key(key = "BasicSettings_GenerationTime")]
			public Value.FloatWithRandom GenerationTime
			{
				get;
				private set;
			}

			[Selected(ID = 0, Value = (int)GenerationTimingType.Continuous)]
			[Key(key = "BasicSettings_GenerationTimeOffset")]
			public Value.FloatWithRandom GenerationTimeOffset
			{
				get;
				private set;
			}

			[Selected(ID = 0, Value = (int)GenerationTimingType.Continuous)]
			[Key(key = "BasicSettings_Generation_StartTrigger")]
			public Value.Enum<TriggerType> ToStartGeneration
			{
				get;
				private set;
			}

			[Selected(ID = 0, Value = (int)GenerationTimingType.Continuous)]
			[Key(key = "BasicSettings_Generation_StopTrigger")]
			public Value.Enum<TriggerType> ToStopGeneration
			{
				get;
				private set;
			}

			[Selected(ID = 0, Value = (int)GenerationTimingType.Trigger)]
			[Key(key = "BasicSettings_Generation_SpawnTrigger")]
			[Name(value = "Trigger to Generate")]
			public Value.Enum<TriggerType> Trigger
			{
				get;
				private set;
			}

			[Selected(ID = 0, Value = (int)GenerationTimingType.Trigger)]
			[Key(key = "BasicSettings_Generation_SpawnCount")]
			[Name(value = "Particles per Trigger")]
			public Value.IntWithRandom TriggerCount
			{
				get;
				private set;
			}

			public GenerationValues()
			{
				Timing = new Value.Enum<GenerationTimingType>(GenerationTimingType.Continuous);
				GenerationTime = new Value.FloatWithRandom(1.0f, float.MaxValue, 0.00001f);
				GenerationTimeOffset = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);
				ToStartGeneration = new Value.Enum<TriggerType>(TriggerType.None);
				ToStopGeneration = new Value.Enum<TriggerType>(TriggerType.None);
				Trigger = new Value.Enum<TriggerType>(TriggerType.None);
				TriggerCount = new Value.IntWithRandom(1, int.MaxValue, 0, DrawnAs.CenterAndAmplitude, 1);
			}
		}

		public class LegacyTriggerParameter
		{
			[Key(key = "BasicSettings_Trigger_ToStartGeneration")]
			[Shown(Shown = false)]
			public Value.Enum<TriggerType> ToStartGeneration
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Trigger_ToStopGeneration")]
			[Shown(Shown = false)]
			public Value.Enum<TriggerType> ToStopGeneration
			{
				get;
				private set;
			}

			[Key(key = "BasicSettings_Trigger_ToRemove")]
			[Shown(Shown = false)]
			public Value.Enum<TriggerType> ToRemove
			{
				get;
				private set;
			}

			public LegacyTriggerParameter()
			{
				ToStartGeneration = new Value.Enum<TriggerType>(TriggerType.None);
				ToStopGeneration = new Value.Enum<TriggerType>(TriggerType.None);
				ToRemove = new Value.Enum<TriggerType>(TriggerType.None);
			}
		}

		[Key(key = "BasicSettings_Life")]
		public Value.IntWithRandom Life
		{
			get;
			private set;
		}

		[IO(Export = true)]
		[Name(value = "Generation Timing")]
		[TreeNode(key = "BasicSettings_Generation", id = "BasicSettings_Generation")]
		public GenerationValues Generation
		{
			get;
			private set;
		}

		[IO(Export = true)]
		[Name(value = "Removal Timing")]
		[TreeNode(key = "BasicSettings_Removal", id = "BasicSettings_Removal")]
		public RemovalValues Removal
		{
			get;
			private set;
		}

		// Legacy fields (import only)
		[IO(Import = true, Export = false)]
		[Shown(Shown = false)]
		public Value.Boolean RemoveWhenLifeIsExtinct
		{
			get;
			private set;
		}

		[IO(Import = true, Export = false)]
		[Shown(Shown = false)]
		public Value.Boolean RemoveWhenParentIsRemoved
		{
			get;
			private set;
		}

		[IO(Import = true, Export = false)]
		[Shown(Shown = false)]
		public Value.Boolean RemoveWhenAllChildrenAreRemoved
		{
			get;
			private set;
		}

		[IO(Import = true, Export = false)]
		[Shown(Shown = false)]
		public Value.FloatWithRandom GenerationTime
		{
			get;
			private set;
		}

		[IO(Import = true, Export = false)]
		[Shown(Shown = false)]
		public Value.FloatWithRandom GenerationTimeOffset
		{
			get;
			private set;
		}

		[IO(Import = true, Export = false)]
		[Shown(Shown = false)]
		public LegacyTriggerParameter TriggerParam
		{
			get;
			private set;
		}

		public class LODParameter
		{
			[Key(key = "LODParameter_MatchingLODs")]
			[Shown(Shown = false)]
			public Value.Int MatchingLODs
			{
				get;
				set;
			}
		
			[Key(key = "LODParameter_LodBehaviour")]
			[Shown(Shown = false)]
			public Value.Enum<LODBehaviourType> LodBehaviour
			{
				get;
				set;
			}
		
			public LODParameter()
			{
				MatchingLODs = new Value.Int(0b1111);
				LodBehaviour = new Value.Enum<LODBehaviourType>(LODBehaviourType.Hide);
			}
		}

		[IO(Export = true)]
		public LODParameter LodParameter
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
			Life = new Value.IntWithRandom(100, int.MaxValue, 1);
			Generation = new GenerationValues();
			Removal = new RemovalValues();
			RemoveWhenLifeIsExtinct = new Value.Boolean(true);
			RemoveWhenParentIsRemoved = new Value.Boolean(false);
			RemoveWhenAllChildrenAreRemoved = new Value.Boolean(false);
			GenerationTime = new Value.FloatWithRandom(1.0f, float.MaxValue, 0.00001f);
			GenerationTimeOffset = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);
			TriggerParam = new LegacyTriggerParameter();
			SteeringBehaviorParam = new SteeringBehaviorParameter();
			LodParameter = new LODParameter();
			

			// dynamic parameter
			MaxGeneration.CanSelectDynamicEquation = true;
			Life.CanSelectDynamicEquation = true;
			Generation.GenerationTime.CanSelectDynamicEquation = true;
			Generation.GenerationTimeOffset.CanSelectDynamicEquation = true;
			Generation.TriggerCount.CanSelectDynamicEquation = true;
			GenerationTime.CanSelectDynamicEquation = true;
			GenerationTimeOffset.CanSelectDynamicEquation = true;
		}
	}
}
