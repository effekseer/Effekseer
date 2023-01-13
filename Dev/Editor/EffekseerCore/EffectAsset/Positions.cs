namespace Effekseer.EffectAsset
{
	public partial class Positions
	{
		public enum ParamaterType
		{
			[Key(key = "Position_ParamaterType_Fixed")]
			Fixed = 0,
			[Key(key = "Position_ParamaterType_PVA")]
			PVA = 1,
			[Key(key = "Position_ParamaterType_Easing")]
			Easing = 2,
			[Key(key = "Position_ParamaterType_LocationFCurve")]
			LocationFCurve = 3,
			[Key(key = "Position_ParameterType_NurbsCurve")]
			NurbsCurve = 4,
			[Key(key = "Position_ParameterType_ViewOffset")]
			ViewOffset = 5,
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class Positions
	{
		public partial class FixedParamater
		{
			[Key(key = "Position_FixedParamater_Location")]
			public Effekseer.Vector3F Location = new Effekseer.Vector3F();
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class Positions
	{
		public partial class PVAParamater
		{
			[Key(key = "Position_PVAParamater_Location")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Location = new Effekseer.EffectAsset.Vector3WithRange();

			[Key(key = "Position_PVAParamater_Velocity")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Velocity = new Effekseer.EffectAsset.Vector3WithRange();

			[Key(key = "Position_PVAParamater_Acceleration")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Acceleration = new Effekseer.EffectAsset.Vector3WithRange();
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class Positions
	{
		public partial class NurbsCurveParameter
		{
			public enum NurbsLoopType
			{
				[Key(key = "Position_NurbsCurveParameter_NurbsLoopType_Repeat")]
				Repeat = 0,
				[Key(key = "Position_NurbsCurveParameter_NurbsLoopType_Stop")]
				Stop = 1,
			}
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class Positions
	{
		public partial class NurbsCurveParameter
		{
			[Key(key = "Position_NurbsCurveParameter_FileParh")]
			public Effekseer.EffectAsset.CurveAsset FilePath = null;

			[Key(key = "Position_NurbsCurveParameter_Scale")]
			public float Scale = 1;

			[Key(key = "Position_NurbsCurveParameter_MoveSpeed")]
			public float MoveSpeed = 1;

			[Key(key = "Position_NurbsCurveParameter_LoopType")]
			public Effekseer.EffectAsset.Positions.NurbsCurveParameter.NurbsLoopType LoopType = Effekseer.EffectAsset.Positions.NurbsCurveParameter.NurbsLoopType.Repeat;
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class Positions
	{
		public partial class ViewOffsetParameter
		{
			[Key(key = "Position_ViewOffsetParameter_Distance")]
			public Effekseer.EffectAsset.FloatWithRange Distance = new Effekseer.EffectAsset.FloatWithRange();
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class Positions
	{
		public Effekseer.EffectAsset.Positions.ParamaterType Type = Effekseer.EffectAsset.Positions.ParamaterType.Fixed;

		public Effekseer.EffectAsset.Positions.FixedParamater Fixed = new FixedParamater();


		public Effekseer.EffectAsset.Positions.PVAParamater PVA = new PVAParamater();


		public Effekseer.EffectAsset.Vector3DEasingParamater Easing = new Vector3DEasingParamater();


		public Effekseer.EffectAsset.Vector3DFCurveParameter LocationFCurve = new Vector3DFCurveParameter();


		public Effekseer.EffectAsset.Positions.NurbsCurveParameter NurbsCurve = new NurbsCurveParameter();


		public Effekseer.EffectAsset.Positions.ViewOffsetParameter ViewOffset = new ViewOffsetParameter();

	}
}

