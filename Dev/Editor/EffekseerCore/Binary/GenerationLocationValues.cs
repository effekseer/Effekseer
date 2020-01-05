using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;

namespace Effekseer.Binary
{
	class GenerationLocationValues
	{
		public static byte[] GetBytes(Data.GenerationLocationValues value, Data.ParentEffectType parentEffectType, Dictionary<string, int> model_and_index)
		{
			List<byte[]> data = new List<byte[]>();
			if (value.EffectsRotation)
			{
				data.Add((1).GetBytes());
			}
			else
			{
				data.Add((0).GetBytes());
			}

			data.Add(value.Type.GetValueAsInt().GetBytes());

			if (value.Type.GetValue() == Data.GenerationLocationValues.ParameterType.Point)
			{
				data.Add(value.Point.Location.GetBytes(1.0f));
			}
			else if (value.Type.GetValue() == Data.GenerationLocationValues.ParameterType.Sphere)
			{
				data.Add((value.Sphere.Radius.Max).GetBytes());
				data.Add((value.Sphere.Radius.Min).GetBytes());
				data.Add((value.Sphere.RotationX.Max / 180.0f * (float)Math.PI).GetBytes());
				data.Add((value.Sphere.RotationX.Min / 180.0f * (float)Math.PI).GetBytes());
				data.Add((value.Sphere.RotationY.Max / 180.0f * (float)Math.PI).GetBytes());
				data.Add((value.Sphere.RotationY.Min / 180.0f * (float)Math.PI).GetBytes());
			}
			if (value.Type.GetValue() == Data.GenerationLocationValues.ParameterType.Model)
			{
				var relative_path = value.Model.Model.RelativePath;

				if(!string.IsNullOrEmpty(relative_path))
				{
					if (string.IsNullOrEmpty(System.IO.Path.GetDirectoryName(relative_path)))
					{
						relative_path = System.IO.Path.GetFileNameWithoutExtension(relative_path) + ".efkmodel";
					}
					else
					{
						relative_path = System.IO.Path.ChangeExtension(relative_path, ".efkmodel");
					}

					if (model_and_index.ContainsKey(relative_path))
					{
						data.Add(model_and_index[relative_path].GetBytes());
					}
					else
					{
						data.Add(((int)-1).GetBytes());
					}
				}
				else
				{
					data.Add(((int)-1).GetBytes());
				}

				data.Add( ((int)value.Model.Type.Value).GetBytes());
			}
			else if (value.Type.GetValue() == Data.GenerationLocationValues.ParameterType.Circle)
			{
				data.Add((value.Circle.Division.Value).GetBytes());
				data.Add((value.Circle.Radius.Max).GetBytes());
				data.Add((value.Circle.Radius.Min).GetBytes());
				data.Add((value.Circle.AngleStart.Max / 180.0f * (float)Math.PI).GetBytes());
				data.Add((value.Circle.AngleStart.Min / 180.0f * (float)Math.PI).GetBytes());
				data.Add((value.Circle.AngleEnd.Max / 180.0f * (float)Math.PI).GetBytes());
				data.Add((value.Circle.AngleEnd.Min / 180.0f * (float)Math.PI).GetBytes());
				data.Add(((int)value.Circle.Type.Value).GetBytes());

				// Version 1.30(10)
				data.Add(((int)value.Circle.AxisDirection.Value).GetBytes());

				data.Add((value.Circle.AngleNoize.Max / 180.0f * (float)Math.PI).GetBytes());
				data.Add((value.Circle.AngleNoize.Min / 180.0f * (float)Math.PI).GetBytes());
			}

			else if (value.Type.GetValue() == Data.GenerationLocationValues.ParameterType.Line)
			{
				data.Add((value.Line.Division.Value).GetBytes());
				data.Add(value.Line.PositionStart.GetBytes(1.0f));
				data.Add(value.Line.PositionEnd.GetBytes(1.0f));
				data.Add((value.Line.PositionNoize.Max).GetBytes());
				data.Add((value.Line.PositionNoize.Min).GetBytes());
				data.Add(((int)value.Line.Type.Value).GetBytes());
			}

			return data.ToArray().ToArray();
		}
	}
}
