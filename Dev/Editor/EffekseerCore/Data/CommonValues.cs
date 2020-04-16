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

		[Key(key = "BasicSettings_LocationEffectType")]
		public Value.Enum<ParentEffectType> LocationEffectType
		{
			get;
			private set;
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

		internal CommonValues()
		{
			MaxGeneration = new Value.IntWithInifinite(1, false, int.MaxValue, 1);
			LocationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RotationEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			ScaleEffectType = new Value.Enum<ParentEffectType>(ParentEffectType.Already);
			RemoveWhenLifeIsExtinct = new Value.Boolean(true);
			RemoveWhenParentIsRemoved = new Value.Boolean(false);
			RemoveWhenAllChildrenAreRemoved = new Value.Boolean(false);
			Life = new Value.IntWithRandom(100, int.MaxValue, 1);
			GenerationTime = new Value.FloatWithRandom(1.0f, float.MaxValue, 0.00001f);
			GenerationTimeOffset = new Value.FloatWithRandom(0, float.MaxValue, float.MinValue);

			// dynamic parameter
			MaxGeneration.CanSelectDynamicEquation = true;
			Life.CanSelectDynamicEquation = true;
			GenerationTime.CanSelectDynamicEquation = true;
			GenerationTimeOffset.CanSelectDynamicEquation = true;
		}
	}
}
