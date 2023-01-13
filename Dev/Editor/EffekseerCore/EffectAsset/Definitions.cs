namespace Effekseer.EffectAsset
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

namespace Effekseer.EffectAsset
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

namespace Effekseer.EffectAsset
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

namespace Effekseer.EffectAsset
{
	public partial class Vector3DEasingParamater
	{
		[Key(key = "Easing_Start")]
		[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
		public Effekseer.EffectAsset.Vector3WithRange Start = new Effekseer.EffectAsset.Vector3WithRange();

		[Key(key = "Easing_End")]
		[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
		public Effekseer.EffectAsset.Vector3WithRange End = new Effekseer.EffectAsset.Vector3WithRange();

		[Key(key = "Easing_Type")]
		public Effekseer.EffectAsset.EasingType Type = Effekseer.EffectAsset.EasingType.LeftRightSpeed;

		[Key(key = "Easing_StartSpeed")]
		public Effekseer.EffectAsset.EasingStart StartSpeed = Effekseer.EffectAsset.EasingStart.Start;

		[Key(key = "Easing_EndSpeed")]
		public Effekseer.EffectAsset.EasingEnd EndSpeed = Effekseer.EffectAsset.EasingEnd.End;

		[Key(key = "Easing_IsMiddleEnabled")]
		public bool IsMiddleEnabled = false;

		[Key(key = "Easing_Middle")]
		[Effekseer.EffectAsset.FloatRange(Min = float.MinValue, Max = float.MaxValue)]
		public Effekseer.EffectAsset.Vector3WithRange Middle = new Effekseer.EffectAsset.Vector3WithRange();

		[Key(key = "Easing_IsRandomGroupEnabled")]
		public bool IsRandomGroupEnabled = false;

		[Key(key = "Easing_RandomGroup_X")]
		public int RandomGroupX = 0;

		[Key(key = "Easing_RandomGroup_Y")]
		public int RandomGroupY = 1;

		[Key(key = "Easing_RandomGroup_Z")]
		public int RandomGroupZ = 2;

		[Key(key = "Easing_IsIndividualTypeEnabled")]
		public bool IsIndividualTypeEnabled = false;

		[Key(key = "Easing_IndividualType_X")]
		public Effekseer.EffectAsset.EasingType TypeX = Effekseer.EffectAsset.EasingType.Linear;

		[Key(key = "Easing_IndividualType_Y")]
		public Effekseer.EffectAsset.EasingType TypeY = Effekseer.EffectAsset.EasingType.Linear;

		[Key(key = "Easing_IndividualType_Z")]
		public Effekseer.EffectAsset.EasingType TypeZ = Effekseer.EffectAsset.EasingType.Linear;
	}
}

namespace Effekseer.EffectAsset
{
	public partial class Vector3DFCurveParameter
	{
		[Key(key = "FCurve")]
		public Effekseer.EffectAsset.FCurveVector3D FCurve = new Effekseer.EffectAsset.FCurveVector3D();
	}
}

namespace Effekseer.EffectAsset
{
	public partial class FloatEasingParamater
	{
		[Key(key = "Easing_Start")]
		public Effekseer.EffectAsset.FloatWithRange Start = new Effekseer.EffectAsset.FloatWithRange();

		[Key(key = "Easing_End")]
		public Effekseer.EffectAsset.FloatWithRange End = new Effekseer.EffectAsset.FloatWithRange();

		[Key(key = "Easing_TYpe")]
		public Effekseer.EffectAsset.EasingType Type = Effekseer.EffectAsset.EasingType.LeftRightSpeed;

		[Key(key = "Easing_StartSpeed")]
		public Effekseer.EffectAsset.EasingStart StartSpeed = Effekseer.EffectAsset.EasingStart.Start;

		[Key(key = "Easing_EndSpeed")]
		public Effekseer.EffectAsset.EasingEnd EndSpeed = Effekseer.EffectAsset.EasingEnd.End;

		[Key(key = "Easing_IsMiddleEnabled")]
		public bool IsMiddleEnabled = false;

		[Key(key = "Easing_Middle")]
		public Effekseer.EffectAsset.FloatWithRange Middle = new Effekseer.EffectAsset.FloatWithRange();

		[Key(key = "Easing_IsRandomGroupEnabled")]
		public bool IsRandomGroupEnabled = false;

		[Key(key = "Easing_RandomGroup_A")]
		public int RandomGroupA = 0;

		[Key(key = "Easing_IsIndividualTypeEnabled")]
		public bool IsIndividualTypeEnabled = false;

		[Key(key = "Easing_IndividualType_A")]
		public Effekseer.EffectAsset.EasingType Type_A = Effekseer.EffectAsset.EasingType.Linear;
	}
}

