using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class LocationAbsValues
	{
		public static byte[] GetBytes(Data.LocationAbsValues value, Data.ParentEffectType parentEffectType)
		{
			List<byte[]> data = new List<byte[]>();
			data.Add(value.Type.GetValueAsInt().GetBytes());

			if (value.Type.GetValue() == Data.LocationAbsValues.ParamaterType.Gravity)
			{
				var bytes = TranslationAbs_Gravity_Values.Create(value.Gravity).GetBytes();
				data.Add(bytes.Count().GetBytes());
				data.Add(bytes);
			}
			else if (value.Type.GetValue() == Data.LocationAbsValues.ParamaterType.None)
			{
				data.Add(((int)0).GetBytes());
			}

			return data.ToArray().ToArray();
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	struct TranslationAbs_Gravity_Values
	{
		public Vector3D Gravity;

		static public TranslationAbs_Gravity_Values Create(Data.LocationAbsValues.GravityParamater value)
		{
			var s_value = new TranslationAbs_Gravity_Values();

			s_value.Gravity = new Vector3D(
				value.Gravity.X,
				value.Gravity.Y,
				value.Gravity.Z);

			return s_value;
		}
	}
}
