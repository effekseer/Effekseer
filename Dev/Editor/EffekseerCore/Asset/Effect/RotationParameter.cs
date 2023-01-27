namespace Effekseer.Asset.Effect
{
	public partial class RotationParameter
	{
		public enum ParamaterType
		{
			[Key(key = "Rotation_ParamaterType_Fixed")]
			Fixed = 0,
			[Key(key = "Rotation_ParamaterType_PVA")]
			PVA = 1,
			[Key(key = "Rotation_ParamaterType_Easing")]
			Easing = 2,
			[Key(key = "Rotation_ParamaterType_AxisPVA")]
			AxisPVA = 3,
			[Key(key = "Rotation_ParamaterType_AxisEasing")]
			AxisEasing = 4,
			[Key(key = "Rotation_ParamaterType_RotationFCurve")]
			RotationFCurve = 5,
			[Key(key = "Rotation_ParamaterType_RotateToViewpoint")]
			RotateToViewpoint = 6,
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class RotationParameter
	{
		public partial class FixedParamater
		{
			[Key(key = "Rotation_FixedParamater_Rotation")]
			public Effekseer.Asset.Vector3 Rotation = new Effekseer.Asset.Vector3();
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class RotationParameter
	{
		public partial class PVAParamater
		{
			[Key(key = "Rotation_PVAParamater_Rotation")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Rotation = new Effekseer.Asset.Vector3WithRange();

			[Key(key = "Rotation_PVAParamater_Velocity")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Velocity = new Effekseer.Asset.Vector3WithRange();

			[Key(key = "Rotation_PVAParamater_Acceleration")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Acceleration = new Effekseer.Asset.Vector3WithRange();
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class RotationParameter
	{
		public partial class AxisPVAParamater
		{
			[Key(key = "Rotation_AxisPVAParamater_Axis")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Axis = new Effekseer.Asset.Vector3WithRange();

			[Key(key = "Rotation_AxisPVAParamater_Rotation")]
			public Effekseer.Asset.FloatWithRange Rotation = new Effekseer.Asset.FloatWithRange();

			[Key(key = "Rotation_AxisPVAParamater_Velocity")]
			public Effekseer.Asset.FloatWithRange Velocity = new Effekseer.Asset.FloatWithRange();

			[Key(key = "Rotation_AxisPVAParamater_Acceleration")]
			public Effekseer.Asset.FloatWithRange Acceleration = new Effekseer.Asset.FloatWithRange();
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class RotationParameter
	{
		public partial class AxisEasingParamater
		{
			[Key(key = "Rotation_AxisEasingParamater_Axis")]
			[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.Asset.Vector3WithRange Axis = new Effekseer.Asset.Vector3WithRange();

			public Effekseer.Asset.FloatEasingParamater Easing = new FloatEasingParamater();

		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class RotationParameter
	{
		public partial class RotationFCurveParamater
		{
			[Key(key = "FCurve")]
			public Effekseer.Asset.FCurveVector3D FCurve = new Effekseer.Asset.FCurveVector3D();
		}
	}
}

namespace Effekseer.Asset.Effect
{
	public partial class RotationParameter
	{
		[VisiblityController(ID = 0)]
		public Effekseer.Asset.Effect.RotationParameter.ParamaterType Type = Effekseer.Asset.Effect.RotationParameter.ParamaterType.Fixed;

		[VisiblityControlled(ID = 0, Value = 0)]
		public Effekseer.Asset.Effect.RotationParameter.FixedParamater Fixed = new FixedParamater();


		[VisiblityControlled(ID = 0, Value = 1)]
		public Effekseer.Asset.Effect.RotationParameter.PVAParamater PVA = new PVAParamater();


		[VisiblityControlled(ID = 0, Value = 2)]
		public Effekseer.Asset.Vector3DEasingParamater Easing = new Vector3DEasingParamater();


		[VisiblityControlled(ID = 0, Value = 3)]
		public Effekseer.Asset.Effect.RotationParameter.AxisPVAParamater AxisPVA = new AxisPVAParamater();


		[VisiblityControlled(ID = 0, Value = 4)]
		public Effekseer.Asset.Effect.RotationParameter.AxisEasingParamater AxisEasing = new AxisEasingParamater();


		[VisiblityControlled(ID = 0, Value = 5)]
		public Effekseer.Asset.Effect.RotationParameter.RotationFCurveParamater RotationFCurve = new RotationFCurveParamater();

	}
}

