using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

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

		public int MaxCreation;
		public int TranslationEffectType;
		public int RotationEffectType;
		public int ScalingEffectType;
		public int RemoveWhenLifeIsExtinct;
		public int RemoveWhenParentIsRemoved;
		public int RemoveWhenChildrenIsExtinct;
		public int Life_Max;
		public int Life_Min;
		public float CreationTime_Max;
		public float CreationTime_Min;
		public float CreationTimeOffset_Max;
		public float CreationTimeOffset_Min;

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

			// Trigger's parameters
			data.Add(((byte)(value.TriggerParam.GenerationEnabled.GetValue() ? 1 : 0)).GetBytes());
			int triggerFlags =
				((value.TriggerParam.ToStartGeneration.Value != Data.TriggerType.None) ? (1 << 0) : 0) |
				((value.TriggerParam.ToStopGeneration.Value != Data.TriggerType.None) ? (1 << 1) : 0) |
				((value.TriggerParam.ToRemove.Value != Data.TriggerType.None) ? (1 << 2) : 0);
			data.Add(((byte)triggerFlags).GetBytes());

			if (value.TriggerParam.ToStartGeneration.Value != Data.TriggerType.None)
			{
				data.Add(((ushort)value.TriggerParam.ToStartGeneration.GetValue()).GetBytes());
			}
			if (value.TriggerParam.ToStopGeneration.Value != Data.TriggerType.None)
			{
				data.Add(((ushort)value.TriggerParam.ToStopGeneration.GetValue()).GetBytes());
			}
			if (value.TriggerParam.ToRemove.Value != Data.TriggerType.None)
			{
				data.Add(((ushort)value.TriggerParam.ToRemove.GetValue()).GetBytes());
			}

			return data.ToArray().ToArray();
		}

		static public CommonValues Create(Data.CommonValues value)
		{
			var s_value = new CommonValues();

			s_value.RefEqMaxGeneration = value.MaxGeneration.DynamicEquation.Index;
			s_value.RefEqLifeMax = value.Life.DynamicEquationMax.Index;
			s_value.RefEqLifeMin = value.Life.DynamicEquationMin.Index;
			s_value.RefEqGenerationTimeMax = value.GenerationTime.DynamicEquationMax.Index;
			s_value.RefEqGenerationTimeMin = value.GenerationTime.DynamicEquationMin.Index;
			s_value.RefEqGenerationTimeOffsetMax = value.GenerationTimeOffset.DynamicEquationMax.Index;
			s_value.RefEqGenerationTimeOffsetMin = value.GenerationTimeOffset.DynamicEquationMin.Index;

			s_value.MaxCreation = value.MaxGeneration.Value;
			if (value.MaxGeneration.Infinite)
			{
				s_value.MaxCreation = int.MaxValue;
			}

			s_value.TranslationEffectType = value.LocationEffectType.GetValueAsInt();
			s_value.RotationEffectType = value.RotationEffectType.GetValueAsInt();
			s_value.ScalingEffectType = value.ScaleEffectType.GetValueAsInt();

			if (value.RemoveWhenLifeIsExtinct)
			{
				s_value.RemoveWhenLifeIsExtinct = 1;
			}
			else
			{
				s_value.RemoveWhenLifeIsExtinct = 0;
			}

			if (value.RemoveWhenParentIsRemoved)
			{
				s_value.RemoveWhenParentIsRemoved = 1;
			}
			else
			{
				s_value.RemoveWhenParentIsRemoved = 0;
			}

			if (value.RemoveWhenAllChildrenAreRemoved)
			{
				s_value.RemoveWhenChildrenIsExtinct = 1;
			}
			else
			{
				s_value.RemoveWhenChildrenIsExtinct = 0;
			}

			s_value.Life_Max = value.Life.Max;
			s_value.Life_Min = value.Life.Min;
			s_value.CreationTime_Max = value.GenerationTime.Max;
			s_value.CreationTime_Min = value.GenerationTime.Min;
			s_value.CreationTimeOffset_Max = value.GenerationTimeOffset.Max;
			s_value.CreationTimeOffset_Min = value.GenerationTimeOffset.Min;

			return s_value;
		}
	}
}
