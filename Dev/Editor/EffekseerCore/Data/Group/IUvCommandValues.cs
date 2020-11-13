using Effekseer.Data.Value;

namespace Effekseer.Data.Group
{
	internal interface IUvCommandValues
	{
		Enum<RendererCommonValues.UVType> UV { get; }
		RendererCommonValues.UVDefaultParamater UVDefault { get; }
		RendererCommonValues.UVFixedParamater UVFixed { get; }
		RendererCommonValues.UVAnimationParamater UVAnimation { get; }
		RendererCommonValues.UVScrollParamater UVScroll { get; }
		RendererCommonValues.UVFCurveParamater UVFCurve { get; }
	}
}
