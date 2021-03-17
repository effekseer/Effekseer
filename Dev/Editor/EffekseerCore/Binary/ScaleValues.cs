using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utl;
using Effekseer.Data;

namespace Effekseer.Binary
{
	class ScaleValues
	{
		public static byte[] GetBytes(Data.ScaleValues value, Data.ParentEffectType parentEffectType, ExporterVersion version)
		{
			float magnification = 1.0f;

			var type = value.Type.Value;

			if(version < ExporterVersion.Ver16Alpha1)
			{
				if(type == Data.ScaleValues.ParamaterType.SingleFCurve)
				{
					type = Data.ScaleValues.ParamaterType.Fixed;
				}
			}

			List<byte[]> data = new List<byte[]>();
			data.Add(((int)type).GetBytes());

			if (type == Data.ScaleValues.ParamaterType.Fixed)
			{
				var refBuf = value.Fixed.Scale.DynamicEquation.Index.GetBytes();
				var mainBuf = Scaling_Fixed_Values.Create(value.Fixed,magnification).GetBytes();
				data.Add((mainBuf.Count() + refBuf.Count()).GetBytes());
				data.Add(refBuf);
				data.Add(mainBuf);
			}
			else if (type == Data.ScaleValues.ParamaterType.PVA)
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
			else if (type == Data.ScaleValues.ParamaterType.Easing)
			{
				Utils.ExportEasing(value.Easing, magnification, data, version);
			}
			else if (type == Data.ScaleValues.ParamaterType.SinglePVA)
			{
				var bytes = Scaling_SinglePVA_Values.Create(value.SinglePVA, magnification).GetBytes();
				data.Add(bytes.Count().GetBytes());
				data.Add(bytes);
			}
			else if (type == Data.ScaleValues.ParamaterType.SingleEasing)
			{
				Utils.ExportEasing(value.SingleEasing, 1.0f, data, version, true);
			}
			else if (type == Data.ScaleValues.ParamaterType.FCurve)
			{
				var bytes = value.FCurve.FCurve.GetBytes();

				data.Add(bytes.Count().GetBytes());
				data.Add(bytes);
			}
			else if (type == Data.ScaleValues.ParamaterType.SingleFCurve)
			{
				var bytes = value.SingleFCurve.GetBytes();

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
