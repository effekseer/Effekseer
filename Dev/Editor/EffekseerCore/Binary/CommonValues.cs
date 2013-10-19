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
		public int MaxCreation;
		public int TranslationEffectType;
		public int RotationEffectType;
		public int ScalingEffectType;
		public int RemoveWhenLifeIsExtinct;
		public int RemoveWhenParentIsRemoved;
		public int RemoveWhenChildrenIsExtinct;
		public int Life_Max;
		public int Life_Min;
		public float CreationTime;
		public float CreationTimeOffset;

		public static byte[] GetBytes(Data.CommonValues value)
		{
			List<byte[]> data = new List<byte[]>();

			var bytes = CommonValues.Create(value).GetBytes();
			data.Add(bytes.Count().GetBytes());
			data.Add(bytes);

			return data.ToArray().ToArray();
		}

		static public CommonValues Create(Data.CommonValues value)
		{
			var s_value = new CommonValues();

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
			s_value.CreationTime = value.GenerationTime;
			s_value.CreationTimeOffset = value.GenerationTimeOffset;

			return s_value;
		}
	}
}
