using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;

namespace Effekseer.Compatibility.Conversion
{
	public partial class ConversionFormatFrom1To2
	{
		EffectAsset.DynamicEquation[] dynamicEquations = null;

		public ConversionFormatFrom1To2()
		{
			dynamicEquations = Effekseer.Core.Dynamic.Equations.Values.Select(_ => ConvertValue(_)).ToArray();
		}

		public EffectAsset.DynamicEquation ConvertValue(Effekseer.Data.DynamicEquation value)
		{
			return new EffectAsset.DynamicEquation { Name = value.Name, Code = value.Code };
		}

		public bool ConvertValue(Effekseer.Data.Value.Boolean value)
		{
			return value.Value;
		}

		public int ConvertValue(Effekseer.Data.Value.Int value)
		{
			return value.Value;
		}

		public float ConvertValue(Effekseer.Data.Value.Float value)
		{
			return value.Value;
		}

		public Effekseer.Vector3F ConvertValue(Effekseer.Data.Value.Vector3D value)
		{
			return new Vector3F { X = value.X, Y = value.Y, Z = value.Z };
		}

		public EffectAsset.DynamicEquation ConvertValue(Effekseer.Data.Value.DynamicEquationReference value)
		{
			return dynamicEquations[value.Index];
		}

		public Effekseer.EffectAsset.FloatWithRange ConvertValue(Effekseer.Data.Value.FloatWithRandom value)
		{
			var ret = new Effekseer.EffectAsset.FloatWithRange();

			ret.IsDynamicEquationEnabled = value.IsDynamicEquationEnabled;
			ret.DynamicEquationMin = ConvertValue(value.DynamicEquationMin);
			ret.DynamicEquationMax = ConvertValue(value.DynamicEquationMax);

			ret.Min = value.Min;
			ret.Max = value.Max;
			ret.DrawnAs = value.DrawnAs;

			return ret;
		}

		public Effekseer.EffectAsset.Vector3WithRange ConvertValue(Effekseer.Data.Value.Vector3DWithRandom value)
		{
			var ret = new Effekseer.EffectAsset.Vector3WithRange();

			ret.IsDynamicEquationEnabled = value.IsDynamicEquationEnabled;
			ret.DynamicEquationMin = ConvertValue(value.DynamicEquationMin);
			ret.DynamicEquationMax = ConvertValue(value.DynamicEquationMax);

			ret.X = ConvertValue(value.X);
			ret.Y = ConvertValue(value.Y);
			ret.Z = ConvertValue(value.Z);

			return ret;
		}

		public Effekseer.EffectAsset.CurveAsset ConvertValue(Effekseer.Data.Value.PathForCurve value)
		{
			return CoreContext.Environment.GetAsset(value.RelativePath) as Effekseer.EffectAsset.CurveAsset;
		}

		public Effekseer.EffectAsset.FCurve ConvertValue(Effekseer.Data.Value.FCurve<float> value)
		{
			var ret = new Effekseer.EffectAsset.FCurve();

			ret.StartType = (EffectAsset.FCurveEdge)value.StartType.Value;
			ret.EndType = (EffectAsset.FCurveEdge)value.EndType.Value;
			ret.Sampling = value.Sampling.Value;
			ret.OffsetMin = value.OffsetMin.Value;
			ret.OffsetMax = value.OffsetMax.Value;

			foreach (var key in value.Keys)
			{
				var newKey = new EffectAsset.FCurveKey();
				newKey.Key.X = key.Frame;
				newKey.Key.Y = key.ValueAsFloat;
				newKey.Left.X = key.LeftX;
				newKey.Left.Y = key.LeftY;
				newKey.Right.X = key.RightX;
				newKey.Right.Y = key.RightY;
				ret.Keys.Add(newKey);
			}

			return ret;
		}

		public Effekseer.EffectAsset.FCurveVector3D ConvertValue(Effekseer.Data.Value.FCurveVector3D value)
		{
			var ret = new Effekseer.EffectAsset.FCurveVector3D();

			ret.Timeline = (EffectAsset.FCurveTimelineMode)value.Timeline.Value;
			ret.X = ConvertValue(value.X);
			ret.Y = ConvertValue(value.Y);
			ret.Z = ConvertValue(value.Z);
			return ret;
		}
	}
}