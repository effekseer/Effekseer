
namespace Effekseer.Compatibility.Conversion
{
	public partial class ConversionFormatFrom1To2
	{

		public Effekseer.EffectAsset.Positions.ParamaterType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.LocationValues.ParamaterType> src)
		{
			var dst = (Effekseer.EffectAsset.Positions.ParamaterType)src.Value;
			return dst;
		}
		public Effekseer.EffectAsset.Positions.FixedParamater ConvertValue(Effekseer.Data.LocationValues.FixedParamater src)
		{
			var dst = new Effekseer.EffectAsset.Positions.FixedParamater();
			dst.Location = ConvertValue(src.Location);
			return dst;
		}
		public Effekseer.EffectAsset.Positions.PVAParamater ConvertValue(Effekseer.Data.LocationValues.PVAParamater src)
		{
			var dst = new Effekseer.EffectAsset.Positions.PVAParamater();
			dst.Location = ConvertValue(src.Location);
			dst.Velocity = ConvertValue(src.Velocity);
			dst.Acceleration = ConvertValue(src.Acceleration);
			return dst;
		}
		public Effekseer.EffectAsset.EasingType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.EasingType> src)
		{
			var dst = (Effekseer.EffectAsset.EasingType)src.Value;
			return dst;
		}
		public Effekseer.EffectAsset.EasingStart ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.EasingStart> src)
		{
			var dst = (Effekseer.EffectAsset.EasingStart)src.Value;
			return dst;
		}
		public Effekseer.EffectAsset.EasingEnd ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.EasingEnd> src)
		{
			var dst = (Effekseer.EffectAsset.EasingEnd)src.Value;
			return dst;
		}
		public Effekseer.EffectAsset.Vector3DEasingParamater ConvertValue(Effekseer.Data.Vector3DEasingParamater src)
		{
			var dst = new Effekseer.EffectAsset.Vector3DEasingParamater();
			dst.Start = ConvertValue(src.Start);
			dst.End = ConvertValue(src.End);
			dst.Type = ConvertValue(src.Type);
			dst.StartSpeed = ConvertValue(src.StartSpeed);
			dst.EndSpeed = ConvertValue(src.EndSpeed);
			dst.IsMiddleEnabled = ConvertValue(src.IsMiddleEnabled);
			dst.Middle = ConvertValue(src.Middle);
			dst.IsRandomGroupEnabled = ConvertValue(src.IsRandomGroupEnabled);
			dst.RandomGroupX = ConvertValue(src.RandomGroupX);
			dst.RandomGroupY = ConvertValue(src.RandomGroupY);
			dst.RandomGroupZ = ConvertValue(src.RandomGroupZ);
			dst.IsIndividualTypeEnabled = ConvertValue(src.IsIndividualTypeEnabled);
			dst.TypeX = ConvertValue(src.TypeX);
			dst.TypeY = ConvertValue(src.TypeY);
			dst.TypeZ = ConvertValue(src.TypeZ);
			return dst;
		}
		public Effekseer.EffectAsset.Vector3DFCurveParameter ConvertValue(Effekseer.Data.Vector3DFCurveParameter src)
		{
			var dst = new Effekseer.EffectAsset.Vector3DFCurveParameter();
			dst.FCurve = ConvertValue(src.FCurve);
			return dst;
		}
		public Effekseer.EffectAsset.Positions.NurbsCurveParameter.NurbsLoopType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.LocationValues.NurbsCurveParameter.NurbsLoopType> src)
		{
			var dst = (Effekseer.EffectAsset.Positions.NurbsCurveParameter.NurbsLoopType)src.Value;
			return dst;
		}
		public Effekseer.EffectAsset.Positions.NurbsCurveParameter ConvertValue(Effekseer.Data.LocationValues.NurbsCurveParameter src)
		{
			var dst = new Effekseer.EffectAsset.Positions.NurbsCurveParameter();
			dst.FilePath = ConvertValue(src.FilePath);
			dst.Scale = ConvertValue(src.Scale);
			dst.MoveSpeed = ConvertValue(src.MoveSpeed);
			dst.LoopType = ConvertValue(src.LoopType);
			return dst;
		}
		public Effekseer.EffectAsset.Positions.ViewOffsetParameter ConvertValue(Effekseer.Data.LocationValues.ViewOffsetParameter src)
		{
			var dst = new Effekseer.EffectAsset.Positions.ViewOffsetParameter();
			dst.Distance = ConvertValue(src.Distance);
			return dst;
		}
		public Effekseer.EffectAsset.Positions ConvertValue(Effekseer.Data.LocationValues src)
		{
			var dst = new Effekseer.EffectAsset.Positions();
			dst.Type = ConvertValue(src.Type);
			dst.Fixed = ConvertValue(src.Fixed);
			dst.PVA = ConvertValue(src.PVA);
			dst.Easing = ConvertValue(src.Easing);
			dst.LocationFCurve = ConvertValue(src.LocationFCurve);
			dst.NurbsCurve = ConvertValue(src.NurbsCurve);
			dst.ViewOffset = ConvertValue(src.ViewOffset);
			return dst;
		}
		public Effekseer.EffectAsset.RotationValues.ParamaterType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.RotationValues.ParamaterType> src)
		{
			var dst = (Effekseer.EffectAsset.RotationValues.ParamaterType)src.Value;
			return dst;
		}
		public Effekseer.EffectAsset.RotationValues.FixedParamater ConvertValue(Effekseer.Data.RotationValues.FixedParamater src)
		{
			var dst = new Effekseer.EffectAsset.RotationValues.FixedParamater();
			dst.Rotation = ConvertValue(src.Rotation);
			return dst;
		}
		public Effekseer.EffectAsset.RotationValues.PVAParamater ConvertValue(Effekseer.Data.RotationValues.PVAParamater src)
		{
			var dst = new Effekseer.EffectAsset.RotationValues.PVAParamater();
			dst.Rotation = ConvertValue(src.Rotation);
			dst.Velocity = ConvertValue(src.Velocity);
			dst.Acceleration = ConvertValue(src.Acceleration);
			return dst;
		}
		public Effekseer.EffectAsset.RotationValues.AxisPVAParamater ConvertValue(Effekseer.Data.RotationValues.AxisPVAParamater src)
		{
			var dst = new Effekseer.EffectAsset.RotationValues.AxisPVAParamater();
			dst.Axis = ConvertValue(src.Axis);
			dst.Rotation = ConvertValue(src.Rotation);
			dst.Velocity = ConvertValue(src.Velocity);
			dst.Acceleration = ConvertValue(src.Acceleration);
			return dst;
		}
		public Effekseer.EffectAsset.FloatEasingParamater ConvertValue(Effekseer.Data.FloatEasingParamater src)
		{
			var dst = new Effekseer.EffectAsset.FloatEasingParamater();
			dst.Start = ConvertValue(src.Start);
			dst.End = ConvertValue(src.End);
			dst.Type = ConvertValue(src.Type);
			dst.StartSpeed = ConvertValue(src.StartSpeed);
			dst.EndSpeed = ConvertValue(src.EndSpeed);
			dst.IsMiddleEnabled = ConvertValue(src.IsMiddleEnabled);
			dst.Middle = ConvertValue(src.Middle);
			dst.IsRandomGroupEnabled = ConvertValue(src.IsRandomGroupEnabled);
			dst.RandomGroupA = ConvertValue(src.RandomGroupA);
			dst.IsIndividualTypeEnabled = ConvertValue(src.IsIndividualTypeEnabled);
			dst.Type_A = ConvertValue(src.Type_A);
			return dst;
		}
		public Effekseer.EffectAsset.RotationValues.AxisEasingParamater ConvertValue(Effekseer.Data.RotationValues.AxisEasingParamater src)
		{
			var dst = new Effekseer.EffectAsset.RotationValues.AxisEasingParamater();
			dst.Axis = ConvertValue(src.Axis);
			dst.Easing = ConvertValue(src.Easing);
			return dst;
		}
		public Effekseer.EffectAsset.RotationValues.RotationFCurveParamater ConvertValue(Effekseer.Data.RotationValues.RotationFCurveParamater src)
		{
			var dst = new Effekseer.EffectAsset.RotationValues.RotationFCurveParamater();
			dst.FCurve = ConvertValue(src.FCurve);
			return dst;
		}
		public Effekseer.EffectAsset.RotationValues ConvertValue(Effekseer.Data.RotationValues src)
		{
			var dst = new Effekseer.EffectAsset.RotationValues();
			dst.Type = ConvertValue(src.Type);
			dst.Fixed = ConvertValue(src.Fixed);
			dst.PVA = ConvertValue(src.PVA);
			dst.Easing = ConvertValue(src.Easing);
			dst.AxisPVA = ConvertValue(src.AxisPVA);
			dst.AxisEasing = ConvertValue(src.AxisEasing);
			dst.RotationFCurve = ConvertValue(src.RotationFCurve);
			return dst;
		}

	}
}
