using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Effekseer.Utils;
using Effekseer.Data;

namespace Effekseer.Binary
{
	class LocationValues
	{
		public static byte[] GetBytes(Data.LocationValues value, Data.ParentEffectType parentEffectType, SortedDictionary<string, int> curveAndIndex)
		{
			var type = value.Type.Value;

			List<byte[]> data = new List<byte[]>();
			data.Add(((int)type).GetBytes());

			if (type == Data.LocationValues.ParamaterType.Fixed)
			{
				var refBuf = value.Fixed.Location.DynamicEquation.Index.GetBytes();
				var mainBuf = Translation_Fixed_Values.Create(value.Fixed, 1.0f).GetBytes();
				data.Add((mainBuf.Count() + refBuf.Count()).GetBytes());
				data.Add(refBuf);
				data.Add(mainBuf);
			}
			else if (type == Data.LocationValues.ParamaterType.PVA)
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
			else if (type == Data.LocationValues.ParamaterType.Easing)
			{
				Utils.ExportEasing(value.Easing, 1.0f, data);
			}
			else if (type == Data.LocationValues.ParamaterType.LocationFCurve)
			{
				var bytes1 = value.LocationFCurve.FCurve.GetBytes();

				List<byte[]> _data = new List<byte[]>();
				data.Add(bytes1.Count().GetBytes());
				data.Add(bytes1);
			}
			else if (type == Data.LocationValues.ParamaterType.NurbsCurve)
			{
				if (value.NurbsCurve.FilePath.RelativePath != string.Empty)
				{
					// export index
					var relative_path = value.NurbsCurve.FilePath.RelativePath;

					if (string.IsNullOrEmpty(System.IO.Path.GetDirectoryName(relative_path)))
					{
						relative_path = System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkcurve";
					}
					else
					{
						relative_path = System.IO.Path.ChangeExtension(relative_path, ".efkcurve");
					}

					data.Add(curveAndIndex[relative_path].GetBytes());
				}
				else
				{
					data.Add((-1).GetBytes());
				}

				data.Add(BitConverter.GetBytes(value.NurbsCurve.Scale));

				data.Add(BitConverter.GetBytes(value.NurbsCurve.MoveSpeed));

				data.Add(value.NurbsCurve.LoopType.GetValueAsInt().GetBytes());
			}
			else if (type == Data.LocationValues.ParamaterType.ViewOffset)
			{
				data.Add(value.ViewOffset.Distance.Max.GetBytes());
				data.Add(value.ViewOffset.Distance.Min.GetBytes());
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