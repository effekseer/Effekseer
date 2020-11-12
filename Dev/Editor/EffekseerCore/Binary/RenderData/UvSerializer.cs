using System.Linq;
using Effekseer.Data.Value;
using Effekseer.Utl;

namespace Effekseer.Binary.RenderData
{
	internal abstract class UvSerializer
	{
		private Enum<Data.RendererCommonValues.UVType> UvType { get; }
		private Data.RendererCommonValues.UVFixedParamater Fixed { get; }
		private Data.RendererCommonValues.UVAnimationParamater Animation { get; }
		private Data.RendererCommonValues.UVScrollParamater Scroll { get; }
		private Data.RendererCommonValues.UVFCurveParamater FCurve { get; }

		protected UvSerializer(Enum<Data.RendererCommonValues.UVType> uvType,
			Data.RendererCommonValues.UVFixedParamater @fixed,
			Data.RendererCommonValues.UVAnimationParamater animation,
			Data.RendererCommonValues.UVScrollParamater scroll,
			Data.RendererCommonValues.UVFCurveParamater fCurve)
		{
			UvType = uvType;
			Fixed = @fixed;
			Animation = animation;
			Scroll = scroll;
			FCurve = fCurve;
		}

		public byte[] SerializeUv(TextureInformation texInfo)
		{
			GetSize(texInfo, out float width, out float height);

			var aggregator = new UvAggregator(width, height);
			aggregator.Add(UvType);

			switch (UvType.Value)
			{
				case Data.RendererCommonValues.UVType.Default:
					break;
				case Data.RendererCommonValues.UVType.Fixed:
					SerializeFixedUv(aggregator);
					break;
				case Data.RendererCommonValues.UVType.Animation:
					SerializeAnimationUv(aggregator);
					break;
				case Data.RendererCommonValues.UVType.Scroll:
					SerializeScrollUv(aggregator);
					break;
				case Data.RendererCommonValues.UVType.FCurve:
					SerializeFCurveUv(aggregator);
					break;
			}

			return aggregator.CurrentData.ToArray().ToArray();
		}

		protected abstract void GetSize(TextureInformation texInfo, out float width, out float height);

		protected virtual void SerializeFCurveUv(UvAggregator aggregator)
		{
			var width = aggregator.Width;
			var height = aggregator.Height;
			aggregator.Add(FCurve.Start.GetBytes(1.0f / width, 1.0f / height));
			aggregator.Add(FCurve.Size.GetBytes(1.0f / width, 1.0f / height));
		}

		protected virtual void SerializeFixedUv(UvAggregator aggregator)
		{
			aggregator.AddVector2D(Fixed.Start);
			aggregator.AddVector2D(Fixed.Size);
		}

		protected virtual void SerializeAnimationUv(UvAggregator aggregator)
		{
			aggregator.AddVector2D(Animation.Start);
			aggregator.AddVector2D(Animation.Size);

			var frameLength = Animation.FrameLength.Infinite
				? (int.MaxValue / 100)
				: Animation.FrameLength.Value.Value;
			aggregator.AddInt(frameLength);

			aggregator.AddInt(Animation.FrameCountX.Value);
			aggregator.AddInt(Animation.FrameCountY.Value);
			aggregator.Add(Animation.LoopType);

			aggregator.AddInt(Animation.StartSheet.Max);
			aggregator.AddInt(Animation.StartSheet.Min);
		}

		protected virtual void SerializeScrollUv(UvAggregator aggregator)
		{
			aggregator.AddRandomVector2D(Scroll.Start);
			aggregator.AddRandomVector2D(Scroll.Size);
			aggregator.AddRandomVector2D(Scroll.Speed);
		}
	}
}