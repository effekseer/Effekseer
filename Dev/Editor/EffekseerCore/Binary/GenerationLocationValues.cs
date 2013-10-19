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
				var path = value.Model.Model.RelativePath;
				if (model_and_index.ContainsKey(path))
				{
					data.Add(model_and_index[path].GetBytes());
				}
				else
				{
					data.Add(((int)-1).GetBytes());
				}

				data.Add( ((int)value.Model.Type.Value).GetBytes());
			}

			return data.ToArray().ToArray();
		}
	}
}
