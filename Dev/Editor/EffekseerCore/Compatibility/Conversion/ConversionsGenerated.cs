
namespace Effekseer.Compatibility.Conversion
{
	public partial class ConversionFormatFrom1To2
	{

		public Effekseer.Asset.Effect.PositionParameter.ParamaterType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.LocationValues.ParamaterType> src)
		{
			var dst = (Effekseer.Asset.Effect.PositionParameter.ParamaterType)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.PositionParameter.FixedParamater ConvertValue(Effekseer.Data.LocationValues.FixedParamater src)
		{
			var dst = new Effekseer.Asset.Effect.PositionParameter.FixedParamater();
			dst.Location = ConvertValue(src.Location);
			return dst;
		}
		public Effekseer.Asset.Effect.PositionParameter.PVAParamater ConvertValue(Effekseer.Data.LocationValues.PVAParamater src)
		{
			var dst = new Effekseer.Asset.Effect.PositionParameter.PVAParamater();
			dst.Location = ConvertValue(src.Location);
			dst.Velocity = ConvertValue(src.Velocity);
			dst.Acceleration = ConvertValue(src.Acceleration);
			return dst;
		}
		public Effekseer.Asset.EasingType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.EasingType> src)
		{
			var dst = (Effekseer.Asset.EasingType)src.Value;
			return dst;
		}
		public Effekseer.Asset.EasingStart ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.EasingStart> src)
		{
			var dst = (Effekseer.Asset.EasingStart)src.Value;
			return dst;
		}
		public Effekseer.Asset.EasingEnd ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.EasingEnd> src)
		{
			var dst = (Effekseer.Asset.EasingEnd)src.Value;
			return dst;
		}
		public Effekseer.Asset.Vector3DEasingParamater ConvertValue(Effekseer.Data.Vector3DEasingParamater src)
		{
			var dst = new Effekseer.Asset.Vector3DEasingParamater();
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
		public Effekseer.Asset.Vector3DFCurveParameter ConvertValue(Effekseer.Data.Vector3DFCurveParameter src)
		{
			var dst = new Effekseer.Asset.Vector3DFCurveParameter();
			dst.FCurve = ConvertValue(src.FCurve);
			return dst;
		}
		public Effekseer.Asset.Effect.PositionParameter.NurbsCurveParameter.NurbsLoopType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.LocationValues.NurbsCurveParameter.NurbsLoopType> src)
		{
			var dst = (Effekseer.Asset.Effect.PositionParameter.NurbsCurveParameter.NurbsLoopType)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.PositionParameter.NurbsCurveParameter ConvertValue(Effekseer.Data.LocationValues.NurbsCurveParameter src)
		{
			var dst = new Effekseer.Asset.Effect.PositionParameter.NurbsCurveParameter();
			dst.FilePath = ConvertValue(src.FilePath);
			dst.Scale = ConvertValue(src.Scale);
			dst.MoveSpeed = ConvertValue(src.MoveSpeed);
			dst.LoopType = ConvertValue(src.LoopType);
			return dst;
		}
		public Effekseer.Asset.Effect.PositionParameter.ViewOffsetParameter ConvertValue(Effekseer.Data.LocationValues.ViewOffsetParameter src)
		{
			var dst = new Effekseer.Asset.Effect.PositionParameter.ViewOffsetParameter();
			dst.Distance = ConvertValue(src.Distance);
			return dst;
		}
		public Effekseer.Asset.Effect.PositionParameter ConvertValue(Effekseer.Data.LocationValues src)
		{
			var dst = new Effekseer.Asset.Effect.PositionParameter();
			dst.Type = ConvertValue(src.Type);
			dst.Fixed = ConvertValue(src.Fixed);
			dst.PVA = ConvertValue(src.PVA);
			dst.Easing = ConvertValue(src.Easing);
			dst.LocationFCurve = ConvertValue(src.LocationFCurve);
			dst.NurbsCurve = ConvertValue(src.NurbsCurve);
			dst.ViewOffset = ConvertValue(src.ViewOffset);
			return dst;
		}
		public Effekseer.Asset.Effect.RotationParameter.ParamaterType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.RotationValues.ParamaterType> src)
		{
			var dst = (Effekseer.Asset.Effect.RotationParameter.ParamaterType)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.RotationParameter.FixedParamater ConvertValue(Effekseer.Data.RotationValues.FixedParamater src)
		{
			var dst = new Effekseer.Asset.Effect.RotationParameter.FixedParamater();
			dst.Rotation = ConvertValue(src.Rotation);
			return dst;
		}
		public Effekseer.Asset.Effect.RotationParameter.PVAParamater ConvertValue(Effekseer.Data.RotationValues.PVAParamater src)
		{
			var dst = new Effekseer.Asset.Effect.RotationParameter.PVAParamater();
			dst.Rotation = ConvertValue(src.Rotation);
			dst.Velocity = ConvertValue(src.Velocity);
			dst.Acceleration = ConvertValue(src.Acceleration);
			return dst;
		}
		public Effekseer.Asset.Effect.RotationParameter.AxisPVAParamater ConvertValue(Effekseer.Data.RotationValues.AxisPVAParamater src)
		{
			var dst = new Effekseer.Asset.Effect.RotationParameter.AxisPVAParamater();
			dst.Axis = ConvertValue(src.Axis);
			dst.Rotation = ConvertValue(src.Rotation);
			dst.Velocity = ConvertValue(src.Velocity);
			dst.Acceleration = ConvertValue(src.Acceleration);
			return dst;
		}
		public Effekseer.Asset.FloatEasingParamater ConvertValue(Effekseer.Data.FloatEasingParamater src)
		{
			var dst = new Effekseer.Asset.FloatEasingParamater();
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
		public Effekseer.Asset.Effect.RotationParameter.AxisEasingParamater ConvertValue(Effekseer.Data.RotationValues.AxisEasingParamater src)
		{
			var dst = new Effekseer.Asset.Effect.RotationParameter.AxisEasingParamater();
			dst.Axis = ConvertValue(src.Axis);
			dst.Easing = ConvertValue(src.Easing);
			return dst;
		}
		public Effekseer.Asset.Effect.RotationParameter.RotationFCurveParamater ConvertValue(Effekseer.Data.RotationValues.RotationFCurveParamater src)
		{
			var dst = new Effekseer.Asset.Effect.RotationParameter.RotationFCurveParamater();
			dst.FCurve = ConvertValue(src.FCurve);
			return dst;
		}
		public Effekseer.Asset.Effect.RotationParameter ConvertValue(Effekseer.Data.RotationValues src)
		{
			var dst = new Effekseer.Asset.Effect.RotationParameter();
			dst.Type = ConvertValue(src.Type);
			dst.Fixed = ConvertValue(src.Fixed);
			dst.PVA = ConvertValue(src.PVA);
			dst.Easing = ConvertValue(src.Easing);
			dst.AxisPVA = ConvertValue(src.AxisPVA);
			dst.AxisEasing = ConvertValue(src.AxisEasing);
			dst.RotationFCurve = ConvertValue(src.RotationFCurve);
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter.RenderMode ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.OptionValues.RenderMode> src)
		{
			var dst = (Effekseer.Asset.Effect.OptionParameter.RenderMode)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter.ViewMode ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.OptionValues.ViewMode> src)
		{
			var dst = (Effekseer.Asset.Effect.OptionParameter.ViewMode)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter.FPSType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.OptionValues.FPSType> src)
		{
			var dst = (Effekseer.Asset.Effect.OptionParameter.FPSType)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter.CoordinateType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.OptionValues.CoordinateType> src)
		{
			var dst = (Effekseer.Asset.Effect.OptionParameter.CoordinateType)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter.ColorSpaceType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.OptionValues.ColorSpaceType> src)
		{
			var dst = (Effekseer.Asset.Effect.OptionParameter.ColorSpaceType)src.Value;
			return dst;
		}
		public Effekseer.Asset.FontType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.FontType> src)
		{
			var dst = (Effekseer.Asset.FontType)src.Value;
			return dst;
		}
		public Effekseer.Asset.MouseMappingType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.MouseMappingType> src)
		{
			var dst = (Effekseer.Asset.MouseMappingType)src.Value;
			return dst;
		}
		public Effekseer.Asset.LanguageSelector ConvertValue(Effekseer.Data.LanguageSelector src)
		{
			var dst = new Effekseer.Asset.LanguageSelector();
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter.DistortionMethodType ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.OptionValues.DistortionMethodType> src)
		{
			var dst = (Effekseer.Asset.Effect.OptionParameter.DistortionMethodType)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter.FileViewMode ConvertValue(Effekseer.Data.Value.Enum<Effekseer.Data.OptionValues.FileViewMode> src)
		{
			var dst = (Effekseer.Asset.Effect.OptionParameter.FileViewMode)src.Value;
			return dst;
		}
		public Effekseer.Asset.Effect.OptionParameter ConvertValue(Effekseer.Data.OptionValues src)
		{
			var dst = new Effekseer.Asset.Effect.OptionParameter();
			dst.RenderingMode = ConvertValue(src.RenderingMode);
			dst.ViewerMode = ConvertValue(src.ViewerMode);
			dst.GridColor = ConvertValue(src.GridColor);
			dst.IsGridShown = ConvertValue(src.IsGridShown);
			dst.IsXYGridShown = ConvertValue(src.IsXYGridShown);
			dst.IsXZGridShown = ConvertValue(src.IsXZGridShown);
			dst.IsYZGridShown = ConvertValue(src.IsYZGridShown);
			dst.GridLength = ConvertValue(src.GridLength);
			dst.Magnification = ConvertValue(src.Magnification);
			dst.ExternalMagnification = ConvertValue(src.ExternalMagnification);
			dst.FPS = ConvertValue(src.FPS);
			dst.Coordinate = ConvertValue(src.Coordinate);
			dst.ColorSpace = ConvertValue(src.ColorSpace);
			dst.Font = ConvertValue(src.Font);
			dst.FontSize = ConvertValue(src.FontSize);
			dst.MouseMappingType = ConvertValue(src.MouseMappingType);
			dst.MouseRotInvX = ConvertValue(src.MouseRotInvX);
			dst.MouseRotInvY = ConvertValue(src.MouseRotInvY);
			dst.MouseSlideInvX = ConvertValue(src.MouseSlideInvX);
			dst.MouseSlideInvY = ConvertValue(src.MouseSlideInvY);
			dst.LanguageSelector = ConvertValue(src.LanguageSelector);
			dst.DistortionType = ConvertValue(src.DistortionType);
			dst.FileBrowserViewMode = ConvertValue(src.FileBrowserViewMode);
			dst.FileBrowserIconSize = ConvertValue(src.FileBrowserIconSize);
			dst.FloatFormatDigits = ConvertValue(src.FloatFormatDigits);
			dst.AutoSaveIntervalMin = ConvertValue(src.AutoSaveIntervalMin);
			return dst;
		}

	}
}
