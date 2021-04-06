using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum ZSortType
	{
		[Key(key = "ZsortType_None")]
		None,
		[Key(key = "ZsortType_NormalOrder")]
		NormalOrder,
		[Key(key = "ZsortType_ReverseOrder")]
		ReverseOrder,
	}


	public class DepthValues
    {
		[Key(key = "Depth_DepthOffset")]
        public Value.Float DepthOffset
        {
            get;
            private set;
        }

		[Key(key = "Depth_IsScaleChangedDependingOnDepthOffset")]
		public Value.Boolean IsScaleChangedDependingOnDepthOffset
		{
			get;
			private set;
		}

		[Key(key = "Depth_IsDepthOffsetChangedDependingOnParticleScale")]
		public Value.Boolean IsDepthOffsetChangedDependingOnParticleScale
		{
			get;
			private set;
		}

		[Key(key = "Depth_ZSort")]
		public Value.Enum<ZSortType> ZSort
		{
			get;
			private set;
		}

		[Key(key = "Depth_DepthClipping")]
		public Value.IntWithInifinite DepthClipping
		{
			get;
			private set;
		}

		[Key(key = "Depth_SuppressionOfScalingByDepth")]
		public Value.Float SuppressionOfScalingByDepth
		{
			get;
			private set;
		}

		[Key(key = "Depth_DrawingPriority")]
		public Value.Int DrawingPriority
		{
			get;
			private set;
		}

        public DepthValues()
        {
            DepthOffset = new Value.Float();
			IsScaleChangedDependingOnDepthOffset = new Value.Boolean();
			IsDepthOffsetChangedDependingOnParticleScale = new Value.Boolean();
			ZSort = new Value.Enum<ZSortType>(ZSortType.None);
			DrawingPriority = new Value.Int(0, 255, -255);
			DepthClipping = new Value.IntWithInifinite(1024, true, int.MaxValue, 16);
			SuppressionOfScalingByDepth = new Value.Float(1.0f, 1.0f, 0.0f, 0.1f);
        }
    }
}
