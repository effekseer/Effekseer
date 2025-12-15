using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utils;

namespace Effekseer.Binary
{
	[StructLayout(LayoutKind.Sequential)]
	struct CommonValues
	{
		public int RefEqMaxGeneration;
		public int RefEqLifeMax;
		public int RefEqLifeMin;
		public int RefEqGenerationTimeMax;
		public int RefEqGenerationTimeMin;
		public int RefEqGenerationTimeOffsetMax;
		public int RefEqGenerationTimeOffsetMin;
		public int RefEqTriggerGenerationCountMax;
		public int RefEqTriggerGenerationCountMin;

		public int MaxCreation;
		public int TranslationEffectType;
		public int RotationEffectType;
		public int ScalingEffectType;
		public int GenerationType;
		public int RemovalFlags;
		public int Life_Max;
		public int Life_Min;
		public float CreationTime_Max;
		public float CreationTime_Min;
		public float CreationTimeOffset_Max;
		public float CreationTimeOffset_Min;
		public int TriggerGenerationCount_Max;
		public int TriggerGenerationCount_Min;
		public ushort TriggerToStartGeneration;
		public ushort TriggerToStopGeneration;
		public ushort TriggerToRemove;
		public ushort TriggerToGenerate;

		public static byte[] GetBytes(Data.CommonValues value)
		{
			List<byte[]> data = new List<byte[]>();

			var bytes = CommonValues.Create(value).GetBytes();
			data.Add(bytes.Count().GetBytes());
			data.Add(bytes);

			if (value.LocationEffectType == Data.TranslationParentEffectType.NotBind_FollowParent ||
				value.LocationEffectType == Data.TranslationParentEffectType.WhenCreating_FollowParent)
			{
				data.Add(value.SteeringBehaviorParam.MaxFollowSpeed.Max.GetBytes());
				data.Add(value.SteeringBehaviorParam.MaxFollowSpeed.Min.GetBytes());
				data.Add(value.SteeringBehaviorParam.SteeringSpeed.Max.GetBytes());
				data.Add(value.SteeringBehaviorParam.SteeringSpeed.Min.GetBytes());
			}

			{
				data.Add(value.LodParameter.MatchingLODs.GetValue().GetBytes());
				data.Add(value.LodParameter.LodBehaviour.GetValueAsInt().GetBytes());
			}

			return data.ToArray().ToArray();
		}

		static public CommonValues Create(Data.CommonValues value)
		{
			var s_value = new CommonValues();

			s_value.RefEqMaxGeneration = value.MaxGeneration.DynamicEquation.Index;
			s_value.RefEqLifeMax = value.Life.DynamicEquationMax.Index;
			s_value.RefEqLifeMin = value.Life.DynamicEquationMin.Index;
			s_value.RefEqGenerationTimeMax = value.Generation.GenerationTime.DynamicEquationMax.Index;
			s_value.RefEqGenerationTimeMin = value.Generation.GenerationTime.DynamicEquationMin.Index;
			s_value.RefEqGenerationTimeOffsetMax = value.Generation.GenerationTimeOffset.DynamicEquationMax.Index;
			s_value.RefEqGenerationTimeOffsetMin = value.Generation.GenerationTimeOffset.DynamicEquationMin.Index;
			s_value.RefEqTriggerGenerationCountMax = value.Generation.TriggerCount.DynamicEquationMax.Index;
			s_value.RefEqTriggerGenerationCountMin = value.Generation.TriggerCount.DynamicEquationMin.Index;

			s_value.MaxCreation = value.MaxGeneration.Value;
			if (value.MaxGeneration.Infinite)
			{
				s_value.MaxCreation = int.MaxValue;
			}

			s_value.TranslationEffectType = value.LocationEffectType.GetValueAsInt();
			s_value.RotationEffectType = value.RotationEffectType.GetValueAsInt();
			s_value.ScalingEffectType = value.ScaleEffectType.GetValueAsInt();

			int removalFlags = 0;
			if (value.Removal.WhenLifeIsExtinct)
			{
				removalFlags |= 1 << 0;
			}

			if (value.Removal.WhenParentIsRemoved)
			{
				removalFlags |= 1 << 1;
			}

			if (value.Removal.WhenAllChildrenAreRemoved)
			{
				removalFlags |= 1 << 2;
			}

			if (value.Removal.TriggerToRemove.Value != Data.TriggerType.None)
			{
				removalFlags |= 1 << 3;
			}

			s_value.RemovalFlags = removalFlags;

			s_value.Life_Max = value.Life.Max;
			s_value.Life_Min = value.Life.Min;
			s_value.CreationTime_Max = value.Generation.GenerationTime.Max;
			s_value.CreationTime_Min = value.Generation.GenerationTime.Min;
			s_value.CreationTimeOffset_Max = value.Generation.GenerationTimeOffset.Max;
			s_value.CreationTimeOffset_Min = value.Generation.GenerationTimeOffset.Min;
			s_value.GenerationType = value.Generation.Timing.GetValueAsInt();
			s_value.TriggerGenerationCount_Max = value.Generation.TriggerCount.Max;
			s_value.TriggerGenerationCount_Min = value.Generation.TriggerCount.Min;
			s_value.TriggerToStartGeneration = (ushort)value.Generation.ToStartGeneration.GetValue();
			s_value.TriggerToStopGeneration = (ushort)value.Generation.ToStopGeneration.GetValue();
			s_value.TriggerToRemove = (ushort)value.Removal.TriggerToRemove.GetValue();
			s_value.TriggerToGenerate = (ushort)value.Generation.Trigger.GetValue();

			return s_value;
		}
	}
}
