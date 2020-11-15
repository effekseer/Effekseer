using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal sealed class BasicUvSerializer : UvSerializer
	{
		private readonly Data.RendererCommonValues.UVAnimationSupportedFrameBlendParameter _animationParameter;

		public BasicUvSerializer(Data.RendererCommonValues value)
			: base(value.UV, value.UVFixed, value.UVAnimation.AnimationParams, value.UVScroll, value.UVFCurve)
		{
			_animationParameter = value.UVAnimation;
		}

		protected override void GetSize(TextureInformation texInfo, out float width, out float height)
		{
			// specification change(1.5)
			width = 128.0f;
			height = 128.0f;

			if (texInfo.Width > 0 && texInfo.Height > 0)
			{
				width = texInfo.Width;
				height = texInfo.Height;
			}
		}

		protected override void SerializeAnimationUv(UvAggregator aggregator)
		{
			base.SerializeAnimationUv(aggregator);
			aggregator.Add(_animationParameter.FlipbookInterpolationType);
		}
	}
}