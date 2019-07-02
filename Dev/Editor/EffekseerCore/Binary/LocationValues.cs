using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class LocationValues
	{
		public static byte[] GetBytes(Data.LocationValues value, Data.ParentEffectType parentEffectType)
		{
			//if (parentEffectType != Data.ParentEffectType.NotBind) magnification = 1.0f;

			List<byte[]> data = new List<byte[]>();
			data.Add(value.Type.GetValueAsInt().GetBytes());

			if (value.Type.GetValue() == Data.LocationValues.ParamaterType.Fixed)
			{
				var refBuf = value.Fixed.Location.DynamicEquation.Index.GetBytes();
				var mainBuf = Translation_Fixed_Values.Create(value.Fixed, 1.0f).GetBytes();
				data.Add((mainBuf.Count() + refBuf.Count()).GetBytes());
				data.Add(refBuf);
				data.Add(mainBuf);
			}
			else if (value.Type.GetValue() == Data.LocationValues.ParamaterType.PVA)
			{
				var refBuf1_1 = value.PVA.Location.DynamicEquationMax.Index.GetBytes();
				var refBuf1_2 = value.PVA.Location.DynamicEquationMin.Index.GetBytes();
				var refBuf2_1 = value.PVA.Velocity.DynamicEquationMax.Index.GetBytes();
				var refBuf2_2 = value.PVA.Velocity.DynamicEquationMin.Index.GetBytes();
				var refBuf3_1 = value.PVA.Acceleration.DynamicEquationMax.Index.GetBytes();
				var refBuf3_2 = value.PVA.Acceleration.DynamicEquationMin.Index.GetBytes();

				var mainBuf = Translation_PVA_Values.Create(value.PVA, 1.0f).GetBytes();
				data.Add((mainBuf.Count() + refBuf1_1.Count() * 6).GetBytes());
				data.Add(refBuf1_1);
				data.Add(refBuf1_2);
				data.Add(refBuf2_1);
				data.Add(refBuf2_2);
				data.Add(refBuf3_1);
				data.Add(refBuf3_2);
				data.Add(mainBuf);
			}
			else if (value.Type.GetValue() == Data.LocationValues.ParamaterType.Easing)
			{
				var easing = Utl.MathUtl.Easing((float)value.Easing.StartSpeed.Value, (float)value.Easing.EndSpeed.Value);

				var refBuf1_1 = value.Easing.Start.DynamicEquationMax.Index.GetBytes();
				var refBuf1_2 = value.Easing.Start.DynamicEquationMin.Index.GetBytes();
				var refBuf2_1 = value.Easing.End.DynamicEquationMax.Index.GetBytes();
				var refBuf2_2 = value.Easing.End.DynamicEquationMin.Index.GetBytes();

				List<byte[]> _data = new List<byte[]>();
				_data.Add(refBuf1_1);
				_data.Add(refBuf1_2);
				_data.Add(refBuf2_1);
				_data.Add(refBuf2_2);
				_data.Add(value.Easing.Start.GetBytes(1.0f));
				_data.Add(value.Easing.End.GetBytes(1.0f));
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
				var __data = _data.ToArray().ToArray();
				data.Add(__data.Count().GetBytes());
				data.Add(__data);
			}
			else if (value.Type.GetValue() == Data.LocationValues.ParamaterType.LocationFCurve)
			{
				var bytes1 = value.LocationFCurve.FCurve.GetBytes();

				List<byte[]> _data = new List<byte[]>();
				data.Add(bytes1.Count().GetBytes());
				data.Add(bytes1);
			}

			return data.ToArray().ToArray();
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	struct Translation_Fixed_Values
	{
		public Vector3D Position;

		static public Translation_Fixed_Values Create(Data.LocationValues.FixedParamater value, float magnification)
		{
			var s_value = new Translation_Fixed_Values();

			s_value.Position = new Vector3D(
				value.Location.X * magnification,
				value.Location.Y * magnification,
				value.Location.Z * magnification);

			return s_value;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	struct Translation_PVA_Values
	{
		public Vector3D Position_Max;
		public Vector3D Position_Min;

		public Vector3D Velocity_Max;
		public Vector3D Velocity_Min;

		public Vector3D Acceleration_Max;
		public Vector3D Acceleration_Min;

		static public Translation_PVA_Values Create(Data.LocationValues.PVAParamater value, float magnification)
		{
			var s_value = new Translation_PVA_Values();
	
			s_value.Position_Min = new Vector3D(
				value.Location.X.Min * magnification,
				value.Location.Y.Min * magnification,
				value.Location.Z.Min * magnification);

			s_value.Position_Max = new Vector3D(
				value.Location.X.Max * magnification,
				value.Location.Y.Max * magnification,
				value.Location.Z.Max * magnification);

			s_value.Velocity_Min = new Vector3D(
				value.Velocity.X.Min * magnification,
				value.Velocity.Y.Min * magnification,
				value.Velocity.Z.Min * magnification);

			s_value.Velocity_Max = new Vector3D(
				value.Velocity.X.Max * magnification,
				value.Velocity.Y.Max * magnification,
				value.Velocity.Z.Max * magnification);

			s_value.Acceleration_Min = new Vector3D(
				value.Acceleration.X.Min * magnification,
				value.Acceleration.Y.Min * magnification,
				value.Acceleration.Z.Min * magnification);

			s_value.Acceleration_Max = new Vector3D(
				value.Acceleration.X.Max * magnification,
				value.Acceleration.Y.Max * magnification,
				value.Acceleration.Z.Max * magnification);

			return s_value;
		}
	}
}
