using Effekseer.Data.Value;
using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal sealed class AdvancedUvSerializer : UvSerializer
	{
		public AdvancedUvSerializer(
			Enum<Data.RendererCommonValues.UVType> uvType,
			Data.RendererCommonValues.UVFixedParamater @fixed,
			Data.RendererCommonValues.UVAnimationParamater animation,
			Data.RendererCommonValues.UVScrollParamater scroll,
			Data.RendererCommonValues.UVFCurveParamater fCurve)
			: base(uvType, @fixed, animation, scroll, fCurve)
		{
		}

		protected override void GetSize(TextureInformation texInfo, out float width, out float height)
		{
			// specification change(1.5)
			width = 128.0f;
			height = 128.0f;

			if (texInfo.Width > width && texInfo.Height > height)
			{
				width = texInfo.Width;
				height = texInfo.Height;
			}
		}
	}
}