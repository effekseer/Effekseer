namespace Effekseer.Asset
{
	public enum EasingType
	{
		[Key(key = "Easing_LeftRightSpeed")]
		LeftRightSpeed = 0,
		[Key(key = "Easing_Linear")]
		Linear = 1,
		[Key(key = "Easing_EaseInQuadratic")]
		EaseInQuadratic = 10,
		[Key(key = "Easing_EaseOutQuadratic")]
		EaseOutQuadratic = 11,
		[Key(key = "Easing_EaseInOutQuadratic")]
		EaseInOutQuadratic = 12,
		[Key(key = "Easing_EaseInCubic")]
		EaseInCubic = 20,
		[Key(key = "Easing_EaseOutCubic")]
		EaseOutCubic = 21,
		[Key(key = "Easing_EaseInOutCubic")]
		EaseInOutCubic = 22,
		[Key(key = "Easing_EaseInQuartic")]
		EaseInQuartic = 30,
		[Key(key = "Easing_EaseOutQuartic")]
		EaseOutQuartic = 31,
		[Key(key = "Easing_EaseInOutQuartic")]
		EaseInOutQuartic = 32,
		[Key(key = "Easing_EaseInQuintic")]
		EaseInQuintic = 40,
		[Key(key = "Easing_EaseOutQuintic")]
		EaseOutQuintic = 41,
		[Key(key = "Easing_EaseInOutQuintic")]
		EaseInOutQuintic = 42,
		[Key(key = "Easing_EaseInBack")]
		EaseInBack = 50,
		[Key(key = "Easing_EaseOutBack")]
		EaseOutBack = 51,
		[Key(key = "Easing_EaseInOutBack")]
		EaseInOutBack = 52,
		[Key(key = "Easing_EaseInBounce")]
		EaseInBounce = 60,
		[Key(key = "Easing_EaseOutBounce")]
		EaseOutBounce = 61,
		[Key(key = "Easing_EaseInOutBounce")]
		EaseInOutBounce = 62,
	}
}

namespace Effekseer.Asset
{
	public enum EasingStart
	{
		[Key(key = "Easing_StartSlowly3")]
		StartSlowly3 = -30,
		[Key(key = "Easing_StartSlowly2")]
		StartSlowly2 = -20,
		[Key(key = "Easing_StartSlowly1")]
		StartSlowly1 = -10,
		[Key(key = "Easing_StartNormal")]
		Start = 0,
		[Key(key = "Easing_StartRapidly1")]
		StartRapidly1 = 10,
		[Key(key = "Easing_StartRapidly2")]
		StartRapidly2 = 20,
		[Key(key = "Easing_StartRapidly3")]
		StartRapidly3 = 30,
	}
}

namespace Effekseer.Asset
{
	public enum EasingEnd
	{
		[Key(key = "Easing_EndSlowly3")]
		EndSlowly3 = -30,
		[Key(key = "Easing_EndSlowly2")]
		EndSlowly2 = -20,
		[Key(key = "Easing_EndSlowly1")]
		EndSlowly1 = -10,
		[Key(key = "Easing_EndNormal")]
		End = 0,
		[Key(key = "Easing_EndRapidly1")]
		EndRapidly1 = 10,
		[Key(key = "Easing_EndRapidly2")]
		EndRapidly2 = 20,
		[Key(key = "Easing_EndRapidly3")]
		EndRapidly3 = 30,
	}
}

namespace Effekseer.Asset
{
	public partial class Vector3DEasingParamater
	{
		[Key(key = "Easing_Start")]
		[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
		public Effekseer.Asset.Vector3WithRange Start = new Effekseer.Asset.Vector3WithRange();

		[Key(key = "Easing_End")]
		[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
		public Effekseer.Asset.Vector3WithRange End = new Effekseer.Asset.Vector3WithRange();

		[Key(key = "Easing_Type")]
		[VisiblityController(ID = 200)]
		public Effekseer.Asset.EasingType Type = Effekseer.Asset.EasingType.LeftRightSpeed;

		[Key(key = "Easing_StartSpeed")]
		[VisiblityControlled(ID = 200, Value = 0)]
		public Effekseer.Asset.EasingStart StartSpeed = Effekseer.Asset.EasingStart.Start;

		[Key(key = "Easing_EndSpeed")]
		[VisiblityControlled(ID = 200, Value = 0)]
		public Effekseer.Asset.EasingEnd EndSpeed = Effekseer.Asset.EasingEnd.End;

		[Key(key = "Easing_IsMiddleEnabled")]
		[VisiblityController(ID = 300)]
		public bool IsMiddleEnabled = false;

		[Key(key = "Easing_Middle")]
		[VisiblityControlled(ID = 300, Value = 0)]
		[Asset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
		public Effekseer.Asset.Vector3WithRange Middle = new Effekseer.Asset.Vector3WithRange();

		[Key(key = "Easing_IsRandomGroupEnabled")]
		[VisiblityController(ID = 400)]
		public bool IsRandomGroupEnabled = false;

		[Key(key = "Easing_RandomGroup_X")]
		[VisiblityControlled(ID = 400, Value = 0)]
		public int RandomGroupX = 0;

		[Key(key = "Easing_RandomGroup_Y")]
		[VisiblityControlled(ID = 400, Value = 0)]
		public int RandomGroupY = 1;

		[Key(key = "Easing_RandomGroup_Z")]
		[VisiblityControlled(ID = 400, Value = 0)]
		public int RandomGroupZ = 2;

		[Key(key = "Easing_IsIndividualTypeEnabled")]
		[VisiblityController(ID = 500)]
		public bool IsIndividualTypeEnabled = false;

		[Key(key = "Easing_IndividualType_X")]
		[VisiblityControlled(ID = 500, Value = 0)]
		public Effekseer.Asset.EasingType TypeX = Effekseer.Asset.EasingType.Linear;

		[Key(key = "Easing_IndividualType_Y")]
		[VisiblityControlled(ID = 500, Value = 0)]
		public Effekseer.Asset.EasingType TypeY = Effekseer.Asset.EasingType.Linear;

		[Key(key = "Easing_IndividualType_Z")]
		[VisiblityControlled(ID = 500, Value = 0)]
		public Effekseer.Asset.EasingType TypeZ = Effekseer.Asset.EasingType.Linear;
	}
}

namespace Effekseer.Asset
{
	public partial class Vector3DFCurveParameter
	{
		[Key(key = "FCurve")]
		public Effekseer.Asset.FCurveVector3D FCurve = new Effekseer.Asset.FCurveVector3D();
	}
}

namespace Effekseer.Asset
{
	public partial class FloatEasingParamater
	{
		[Key(key = "Easing_Start")]
		public Effekseer.Asset.FloatWithRange Start = new Effekseer.Asset.FloatWithRange();

		[Key(key = "Easing_End")]
		public Effekseer.Asset.FloatWithRange End = new Effekseer.Asset.FloatWithRange();

		[Key(key = "Easing_TYpe")]
		[VisiblityController(ID = 200)]
		public Effekseer.Asset.EasingType Type = Effekseer.Asset.EasingType.LeftRightSpeed;

		[Key(key = "Easing_StartSpeed")]
		[VisiblityControlled(ID = 200, Value = 0)]
		public Effekseer.Asset.EasingStart StartSpeed = Effekseer.Asset.EasingStart.Start;

		[Key(key = "Easing_EndSpeed")]
		[VisiblityControlled(ID = 200, Value = 0)]
		public Effekseer.Asset.EasingEnd EndSpeed = Effekseer.Asset.EasingEnd.End;

		[Key(key = "Easing_IsMiddleEnabled")]
		[VisiblityController(ID = 300)]
		public bool IsMiddleEnabled = false;

		[Key(key = "Easing_Middle")]
		[VisiblityControlled(ID = 300, Value = 0)]
		public Effekseer.Asset.FloatWithRange Middle = new Effekseer.Asset.FloatWithRange();

		[Key(key = "Easing_IsRandomGroupEnabled")]
		[VisiblityController(ID = 400)]
		public bool IsRandomGroupEnabled = false;

		[Key(key = "Easing_RandomGroup_A")]
		[VisiblityControlled(ID = 400, Value = 0)]
		public int RandomGroupA = 0;

		[Key(key = "Easing_IsIndividualTypeEnabled")]
		[VisiblityController(ID = 500)]
		public bool IsIndividualTypeEnabled = false;

		[Key(key = "Easing_IndividualType_A")]
		[VisiblityControlled(ID = 500, Value = 0)]
		public Effekseer.Asset.EasingType Type_A = Effekseer.Asset.EasingType.Linear;
	}
}

