namespace Effekseer.EffectAsset
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

namespace Effekseer.EffectAsset
{
	public partial class RotationParameter
	{
		public partial class FixedParamater
		{
			[Key(key = "Rotation_FixedParamater_Rotation")]
			public Effekseer.Vector3F Rotation = new Effekseer.Vector3F();
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class RotationParameter
	{
		public partial class PVAParamater
		{
			[Key(key = "Rotation_PVAParamater_Rotation")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Rotation = new Effekseer.EffectAsset.Vector3WithRange();

			[Key(key = "Rotation_PVAParamater_Velocity")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Velocity = new Effekseer.EffectAsset.Vector3WithRange();

			[Key(key = "Rotation_PVAParamater_Acceleration")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Acceleration = new Effekseer.EffectAsset.Vector3WithRange();
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class RotationParameter
	{
		public partial class AxisPVAParamater
		{
			[Key(key = "Rotation_AxisPVAParamater_Axis")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Axis = new Effekseer.EffectAsset.Vector3WithRange();

			[Key(key = "Rotation_AxisPVAParamater_Rotation")]
			public Effekseer.EffectAsset.FloatWithRange Rotation = new Effekseer.EffectAsset.FloatWithRange();

			[Key(key = "Rotation_AxisPVAParamater_Velocity")]
			public Effekseer.EffectAsset.FloatWithRange Velocity = new Effekseer.EffectAsset.FloatWithRange();

			[Key(key = "Rotation_AxisPVAParamater_Acceleration")]
			public Effekseer.EffectAsset.FloatWithRange Acceleration = new Effekseer.EffectAsset.FloatWithRange();
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class RotationParameter
	{
		public partial class AxisEasingParamater
		{
			[Key(key = "Rotation_AxisEasingParamater_Axis")]
			[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
			public Effekseer.EffectAsset.Vector3WithRange Axis = new Effekseer.EffectAsset.Vector3WithRange();

			public Effekseer.EffectAsset.FloatEasingParamater Easing = new FloatEasingParamater();

		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class RotationParameter
	{
		public partial class RotationFCurveParamater
		{
			[Key(key = "FCurve")]
			public Effekseer.EffectAsset.FCurveVector3D FCurve = new Effekseer.EffectAsset.FCurveVector3D();
		}
	}
}

namespace Effekseer.EffectAsset
{
	public partial class RotationParameter
	{
		[VisiblityController(ID = 0)]
		public Effekseer.EffectAsset.RotationParameter.ParamaterType Type = Effekseer.EffectAsset.RotationParameter.ParamaterType.Fixed;

		[VisiblityControlled(ID = 0, Value = 0)]
		public Effekseer.EffectAsset.RotationParameter.FixedParamater Fixed = new FixedParamater();


		[VisiblityControlled(ID = 0, Value = 1)]
		public Effekseer.EffectAsset.RotationParameter.PVAParamater PVA = new PVAParamater();


		[VisiblityControlled(ID = 0, Value = 2)]
		public Effekseer.EffectAsset.Vector3DEasingParamater Easing = new Vector3DEasingParamater();


		[VisiblityControlled(ID = 0, Value = 3)]
		public Effekseer.EffectAsset.RotationParameter.AxisPVAParamater AxisPVA = new AxisPVAParamater();


		[VisiblityControlled(ID = 0, Value = 4)]
		public Effekseer.EffectAsset.RotationParameter.AxisEasingParamater AxisEasing = new AxisEasingParamater();


		[VisiblityControlled(ID = 0, Value = 5)]
		public Effekseer.EffectAsset.RotationParameter.RotationFCurveParamater RotationFCurve = new RotationFCurveParamater();

	}
}

