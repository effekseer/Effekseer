namespace Effekseer.Asset.Effect
{
	public partial class PositionParameter
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

namespace Effekseer.Asset.Effect
{
	public partial class PositionParameter
	{
		public partial class FixedParamater
		{
			[Key(key = "Position_FixedParamater_Location")]
			public Effekseer.Asset.Vector3 Location = new Effekseer.Asset.Vector3();
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class PositionParameter
	{
		public partial class PVAParamater
		{
			[Key(key = "Position_PVAParamater_Location")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Location = new Effekseer.Asset.Vector3WithRange();

			[Key(key = "Position_PVAParamater_Velocity")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Velocity = new Effekseer.Asset.Vector3WithRange();

			[Key(key = "Position_PVAParamater_Acceleration")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Acceleration = new Effekseer.Asset.Vector3WithRange();
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class PositionParameter
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

namespace Effekseer.Asset.Effect
{
	public partial class PositionParameter
	{
		public partial class NurbsCurveParameter
		{
			[Key(key = "Position_NurbsCurveParameter_FileParh")]
			public Effekseer.Asset.CurveAsset FilePath = null;

			[Key(key = "Position_NurbsCurveParameter_Scale")]
			public float Scale = 1;

			[Key(key = "Position_NurbsCurveParameter_MoveSpeed")]
			public float MoveSpeed = 1;

			[Key(key = "Position_NurbsCurveParameter_LoopType")]
			public Effekseer.Asset.Effect.PositionParameter.NurbsCurveParameter.NurbsLoopType LoopType = Effekseer.Asset.Effect.PositionParameter.NurbsCurveParameter.NurbsLoopType.Repeat;
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class PositionParameter
	{
		public partial class ViewOffsetParameter
		{
			[Key(key = "Position_ViewOffsetParameter_Distance")]
			public Effekseer.Asset.FloatWithRange Distance = new Effekseer.Asset.FloatWithRange();
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class PositionParameter
	{
		[VisiblityController(ID = 0)]
		public Effekseer.Asset.Effect.PositionParameter.ParamaterType Type = Effekseer.Asset.Effect.PositionParameter.ParamaterType.Fixed;

		[VisiblityControlled(ID = 0, Value = 0)]
		public Effekseer.Asset.Effect.PositionParameter.FixedParamater Fixed = new FixedParamater();


		[VisiblityControlled(ID = 0, Value = 1)]
		public Effekseer.Asset.Effect.PositionParameter.PVAParamater PVA = new PVAParamater();


		[VisiblityControlled(ID = 0, Value = 2)]
		public Effekseer.Asset.Vector3DEasingParamater Easing = new Vector3DEasingParamater();


		[VisiblityControlled(ID = 0, Value = 3)]
		public Effekseer.Asset.Vector3DFCurveParameter LocationFCurve = new Vector3DFCurveParameter();


		[VisiblityControlled(ID = 0, Value = 4)]
		public Effekseer.Asset.Effect.PositionParameter.NurbsCurveParameter NurbsCurve = new NurbsCurveParameter();


		[VisiblityControlled(ID = 0, Value = 5)]
		public Effekseer.Asset.Effect.PositionParameter.ViewOffsetParameter ViewOffset = new ViewOffsetParameter();

	}
}

