using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class ScaleValues
	{
		public static byte[] GetBytes(Data.ScaleValues value, Data.ParentEffectType parentEffectType)
		{
			float magnification = 1.0f;

			List<byte[]> data = new List<byte[]>();
			data.Add(value.Type.GetValueAsInt().GetBytes());

			if (value.Type.GetValue() == Data.ScaleValues.ParamaterType.Fixed)
			{
				var refBuf = value.Fixed.Scale.DynamicEquation.Index.GetBytes();
				var mainBuf = Scaling_Fixed_Values.Create(value.Fixed,magnification).GetBytes();
				data.Add((mainBuf.Count() + refBuf.Count()).GetBytes());
				data.Add(refBuf);
				data.Add(mainBuf);
			}
			else if (value.Type.GetValue() == Data.ScaleValues.ParamaterType.PVA)
			{
				var refBuf1_1 = value.PVA.Scale.DynamicEquationMax.Index.GetBytes();
				var refBuf1_2 = value.PVA.Scale.DynamicEquationMin.Index.GetBytes();
				var refBuf2_1 = value.PVA.Velocity.DynamicEquationMax.Index.GetBytes();
				var refBuf2_2 = value.PVA.Velocity.DynamicEquationMin.Index.GetBytes();
				var refBuf3_1 = value.PVA.Acceleration.DynamicEquationMax.Index.GetBytes();
				var refBuf3_2 = value.PVA.Acceleration.DynamicEquationMin.Index.GetBytes();

				var mainBuf = Scaling_PVA_Values.Create(value.PVA, magnification).GetBytes();
				data.Add((mainBuf.Count() + refBuf1_1.Count() * 6).GetBytes());
				data.Add(refBuf1_1);
				data.Add(refBuf1_2);
				data.Add(refBuf2_1);
				data.Add(refBuf2_2);
				data.Add(refBuf3_1);
				data.Add(refBuf3_2);
				data.Add(mainBuf);
			}
			else if (value.Type.GetValue() == Data.ScaleValues.ParamaterType.Easing)
			{
				var easing = Utl.MathUtl.Easing(
					(float)value.Easing.StartSpeed.Value,
					(float)value.Easing.EndSpeed.Value);

				var refBuf1_1 = value.Easing.Start.DynamicEquationMax.Index.GetBytes();
				var refBuf1_2 = value.Easing.Start.DynamicEquationMin.Index.GetBytes();
				var refBuf2_1 = value.Easing.End.DynamicEquationMax.Index.GetBytes();
				var refBuf2_2 = value.Easing.End.DynamicEquationMin.Index.GetBytes();

				List<byte[]> _data = new List<byte[]>();
				_data.Add(refBuf1_1);
				_data.Add(refBuf1_2);
				_data.Add(refBuf2_1);
				_data.Add(refBuf2_2);
				_data.Add(value.Easing.Start.GetBytes(magnification));
				_data.Add(value.Easing.End.GetBytes(magnification));
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
				var __data = _data.ToArray().ToArray();
				data.Add(__data.Count().GetBytes());
				data.Add(__data);
			}
			else if (value.Type.GetValue() == Data.ScaleValues.ParamaterType.SinglePVA)
			{
				var bytes = Scaling_SinglePVA_Values.Create(value.SinglePVA, magnification).GetBytes();
				data.Add(bytes.Count().GetBytes());
				data.Add(bytes);
			}
			else if (value.Type.GetValue() == Data.ScaleValues.ParamaterType.SingleEasing)
			{
				var easing = Utl.MathUtl.Easing(
					(float)value.SingleEasing.StartSpeed.Value,
					(float)value.SingleEasing.EndSpeed.Value);

				List<byte[]> _data = new List<byte[]>();
                _data.Add(value.SingleEasing.Start.Max.GetBytes());
                _data.Add(value.SingleEasing.Start.Min.GetBytes());
                _data.Add(value.SingleEasing.End.Max.GetBytes());
                _data.Add(value.SingleEasing.End.Min.GetBytes());
                _data.Add(BitConverter.GetBytes(easing[0]));
                _data.Add(BitConverter.GetBytes(easing[1]));
                _data.Add(BitConverter.GetBytes(easing[2]));
				var __data = _data.ToArray().ToArray();
				data.Add(__data.Count().GetBytes());
				data.Add(__data);
			}
			else if (value.Type.GetValue() == Data.ScaleValues.ParamaterType.FCurve)
			{
				var bytes = value.FCurve.FCurve.GetBytes();

				data.Add(bytes.Count().GetBytes());
				data.Add(bytes);
			}

			return data.ToArray().ToArray();
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	struct Scaling_Fixed_Values
	{
		public Vector3D Position;

		static public Scaling_Fixed_Values Create(Data.ScaleValues.FixedParamater value, float magnification)
		{
			var s_value = new Scaling_Fixed_Values();

			s_value.Position = new Vector3D(
				value.Scale.X * magnification,
				value.Scale.Y * magnification,
				value.Scale.Z * magnification);

			return s_value;
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	struct Scaling_PVA_Values
	{
		public Vector3D Position_Max;
		public Vector3D Position_Min;

		public Vector3D Velocity_Max;
		public Vector3D Velocity_Min;

		public Vector3D Acceleration_Max;
		public Vector3D Acceleration_Min;

		static public Scaling_PVA_Values Create(Data.ScaleValues.PVAParamater value, float magnification)
		{
			var s_value = new Scaling_PVA_Values();

			s_value.Position_Min = new Vector3D(
				value.Scale.X.Min * magnification,
				value.Scale.Y.Min * magnification,
				value.Scale.Z.Min * magnification);

			s_value.Position_Max = new Vector3D(
				value.Scale.X.Max * magnification,
				value.Scale.Y.Max * magnification,
				value.Scale.Z.Max * magnification);

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

	[StructLayout(LayoutKind.Sequential)]
	struct Scaling_SinglePVA_Values
	{
		public float Position_Max;
		public float Position_Min;

		public float Velocity_Max;
		public float Velocity_Min;

		public float Acceleration_Max;
		public float Acceleration_Min;

		static public Scaling_SinglePVA_Values Create(Data.ScaleValues.SinglePVAParamater value, float magnification)
		{
			var s_value = new Scaling_SinglePVA_Values();

			s_value.Position_Min = value.Scale.Min * magnification;
			s_value.Position_Max = value.Scale.Max * magnification;
			s_value.Velocity_Min = value.Velocity.Min * magnification;
			s_value.Velocity_Max = value.Velocity.Max * magnification;
			s_value.Acceleration_Min = value.Acceleration.Min * magnification;
			s_value.Acceleration_Max = value.Acceleration.Max * magnification;

			return s_value;
		}
	}
}
