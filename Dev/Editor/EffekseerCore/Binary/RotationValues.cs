using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class RotationValues
	{
		public static byte[] GetBytes(Data.RotationValues value)
		{
			List<byte[]> data = new List<byte[]>();
			data.Add(value.Type.GetValueAsInt().GetBytes());

			if (value.Type.GetValue() == Data.RotationValues.ParamaterType.Fixed)
			{
				var refBuf = value.Fixed.Rotation.DynamicEquation.Index.GetBytes();
				var mainBuf = Rotation_Fixed_Values.Create(value.Fixed).GetBytes();
				data.Add((mainBuf.Count() + refBuf.Count()).GetBytes());
				data.Add(refBuf);
				data.Add(mainBuf);
			}
			else if (value.Type.GetValue() == Data.RotationValues.ParamaterType.PVA)
			{
				var refBuf1_1 = value.PVA.Rotation.DynamicEquationMax.Index.GetBytes();
				var refBuf1_2 = value.PVA.Rotation.DynamicEquationMin.Index.GetBytes();
				var refBuf2_1 = value.PVA.Velocity.DynamicEquationMax.Index.GetBytes();
				var refBuf2_2 = value.PVA.Velocity.DynamicEquationMin.Index.GetBytes();
				var refBuf3_1 = value.PVA.Acceleration.DynamicEquationMax.Index.GetBytes();
				var refBuf3_2 = value.PVA.Acceleration.DynamicEquationMin.Index.GetBytes();

				List<byte[]> _data = new List<byte[]>();
				_data.Add(value.PVA.Rotation.GetBytes((float)Math.PI / 180.0f));
				_data.Add(value.PVA.Velocity.GetBytes((float)Math.PI / 180.0f));
				_data.Add(value.PVA.Acceleration.GetBytes((float)Math.PI / 180.0f));
				var mainBuf = _data.ToArray().ToArray();
				data.Add((mainBuf.Count() + refBuf1_1.Count() * 6).GetBytes());
				data.Add(refBuf1_1);
				data.Add(refBuf1_2);
				data.Add(refBuf2_1);
				data.Add(refBuf2_2);
				data.Add(refBuf3_1);
				data.Add(refBuf3_2);
				data.Add(mainBuf);
			}
			else if (value.Type.GetValue() == Data.RotationValues.ParamaterType.Easing)
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
				_data.Add(value.Easing.Start.GetBytes((float)Math.PI / 180.0f));
				_data.Add(value.Easing.End.GetBytes((float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
				var __data = _data.ToArray().ToArray();
				data.Add(__data.Count().GetBytes());
				data.Add(__data);
			}
			else if (value.Type.GetValue() == Data.RotationValues.ParamaterType.AxisPVA)
			{
				List<byte[]> _data = new List<byte[]>();
				_data.Add(value.AxisPVA.Axis.GetBytes());
				_data.Add(BitConverter.GetBytes(value.AxisPVA.Rotation.Max * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisPVA.Rotation.Min * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisPVA.Velocity.Max * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisPVA.Velocity.Min * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisPVA.Acceleration.Max * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisPVA.Acceleration.Min * (float)Math.PI / 180.0f));
				var __data = _data.ToArray().ToArray();
				data.Add(__data.Count().GetBytes());
				data.Add(__data);
			}
			else if (value.Type.GetValue() == Data.RotationValues.ParamaterType.AxisEasing)
			{
				var easing = Utl.MathUtl.Easing((float)value.AxisEasing.Easing.StartSpeed.Value, (float)value.AxisEasing.Easing.EndSpeed.Value);

				List<byte[]> _data = new List<byte[]>();
				_data.Add(value.AxisEasing.Axis.GetBytes());
				_data.Add(BitConverter.GetBytes(value.AxisEasing.Easing.Start.Max * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisEasing.Easing.Start.Min * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisEasing.Easing.End.Max * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(value.AxisEasing.Easing.End.Min * (float)Math.PI / 180.0f));
				_data.Add(BitConverter.GetBytes(easing[0]));
				_data.Add(BitConverter.GetBytes(easing[1]));
				_data.Add(BitConverter.GetBytes(easing[2]));
				var __data = _data.ToArray().ToArray();
				data.Add(__data.Count().GetBytes());
				data.Add(__data);
			}
			else if (value.Type.GetValue() == Data.RotationValues.ParamaterType.RotationFCurve)
			{
				var bytes = value.RotationFCurve.FCurve.GetBytes(
					(float)Math.PI / 180.0f);

				List<byte[]> _data = new List<byte[]>();
				data.Add(bytes.Count().GetBytes());
				data.Add(bytes);
			}

			return data.ToArray().ToArray();
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	struct Rotation_Fixed_Values
	{
		public Vector3D Position;

		static public Rotation_Fixed_Values Create(Data.RotationValues.FixedParamater value)
		{
			var s_value = new Rotation_Fixed_Values();

			s_value.Position = new Vector3D(
				value.Rotation.X * (float)Math.PI / 180.0f,
				value.Rotation.Y * (float)Math.PI / 180.0f,
				value.Rotation.Z * (float)Math.PI / 180.0f);

			return s_value;
		}
	}
}
