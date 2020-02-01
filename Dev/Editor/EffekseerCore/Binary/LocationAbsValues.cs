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

			// Force field
			List<Data.LocalForceField> lffs = new List<Data.LocalForceField>
			{
				value.LocalForceField1,
				value.LocalForceField2,
				value.LocalForceField3,
			};

			data.Add((lffs.Count).GetBytes());

			foreach(var lff in lffs)
			{
				data.Add(lff.Type.GetValueAsInt().GetBytes());

				if(lff.Type.Value == Data.LocalForceFieldType.Turbulence)
				{
					data.Add(lff.Turbulence.Seed.Value.GetBytes());
					data.Add(lff.Turbulence.FieldScale.Value.GetBytes());
					data.Add(lff.Turbulence.Strength.Value.GetBytes());
					data.Add(lff.Turbulence.Octave.Value.GetBytes());
				}
			}

			data.Add(value.Type.GetValueAsInt().GetBytes());

			if (value.Type.GetValue() == Data.LocationAbsValues.ParamaterType.Gravity)
			{
				var bytes = TranslationAbs_Gravity_Values.Create(value.Gravity).GetBytes();
				data.Add(bytes.Count().GetBytes());
				data.Add(bytes);
			}
			else if (value.Type.GetValue() == Data.LocationAbsValues.ParamaterType.AttractiveForce)
			{
				var bytes = TranslationAbs_AttractiveForce_Values.Create(value.AttractiveForce).GetBytes();
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
	
	[StructLayout(LayoutKind.Sequential)]
	struct TranslationAbs_AttractiveForce_Values
	{
		public float Force;
		public float Control;
		public float MinRange;
		public float MaxRange;

		static public TranslationAbs_AttractiveForce_Values Create(Data.LocationAbsValues.AttractiveForceParamater value)
		{
			var s_value = new TranslationAbs_AttractiveForce_Values();

			s_value.Force = value.Force.Value;
			s_value.Control = value.Control.Value;
			s_value.MinRange = value.MinRange.Value;
			s_value.MaxRange = value.MaxRange.Value;

			return s_value;
		}
	}
}
