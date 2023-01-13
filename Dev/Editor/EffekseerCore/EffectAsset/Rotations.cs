namespace Effekseer.EffectAsset
{
	public partial class RotationValues
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
	public partial class RotationValues
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
	public partial class RotationValues
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
	public partial class RotationValues
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
	public partial class RotationValues
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
	public partial class RotationValues
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
	public partial class RotationValues
	{
		public Effekseer.EffectAsset.RotationValues.ParamaterType Type = Effekseer.EffectAsset.RotationValues.ParamaterType.Fixed;

		public Effekseer.EffectAsset.RotationValues.FixedParamater Fixed = new FixedParamater();


		public Effekseer.EffectAsset.RotationValues.PVAParamater PVA = new PVAParamater();


		public Effekseer.EffectAsset.Vector3DEasingParamater Easing = new Vector3DEasingParamater();


		public Effekseer.EffectAsset.RotationValues.AxisPVAParamater AxisPVA = new AxisPVAParamater();


		public Effekseer.EffectAsset.RotationValues.AxisEasingParamater AxisEasing = new AxisEasingParamater();


		public Effekseer.EffectAsset.RotationValues.RotationFCurveParamater RotationFCurve = new RotationFCurveParamater();

	}
}

