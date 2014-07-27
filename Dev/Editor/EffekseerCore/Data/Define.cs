using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum ParentEffectType : int
	{
		[Name(value = "なし", language = Language.Japanese)]
		NotBind = 0,
		[Name(value = "なし(Root依存)-非推奨", language = Language.Japanese)]
		NotBind_Root = 3,
		[Name(value = "生成時のみ", language = Language.Japanese)]
		WhenCreating = 1,
		[Name(value = "常時", language = Language.Japanese)]
		Already = 2,
	}

	public enum AlphaBlendType : int
	{
		[Name(value = "不透明", language = Language.Japanese)]
		Opacity = 0,
		[Name(value = "通常", language = Language.Japanese)]
		Blend = 1,
		[Name(value = "加算", language = Language.Japanese)]
		Add = 2,
		[Name(value = "減算", language = Language.Japanese)]
		Sub = 3,
		[Name(value = "乗算", language = Language.Japanese)]
		Mul = 4,
	}

	public enum RenderingOrder : int
	{
		[Name(value = "生成順", language = Language.Japanese)]
		FirstCreatedInstanceIsFirst = 0,
		[Name(value = "生成順の逆", language = Language.Japanese)]
		FirstCreatedInstanceIsLast = 1,
	}

	public enum Culling : int
	{
		[Name(value = "表表示", language = Language.Japanese)]
		Front = 0,
		[Name(value = "裏表示", language = Language.Japanese)]
		Back = 1,
		[Name(value = "両面表示", language = Language.Japanese)]
		Double = 2,
	}

	public enum EasingStart : int
	{
		[Name(value = "低速3", language = Language.Japanese)]
		StartSlowly3 = -30,
		[Name(value = "低速2", language = Language.Japanese)]
		StartSlowly2 = -20,
		[Name(value = "低速1", language = Language.Japanese)]
		StartSlowly1 = -10,
		[Name(value = "等速", language = Language.Japanese)]
		Start = 0,
		[Name(value = "高速1", language = Language.Japanese)]
		StartRapidly1 = 10,
		[Name(value = "高速2", language = Language.Japanese)]
		StartRapidly2 = 20,
		[Name(value = "高速3", language = Language.Japanese)]
		StartRapidly3 = 30,
	}

	public enum EasingEnd : int
	{
		[Name(value = "低速3", language = Language.Japanese)]
		EndSlowly3 = -30,
		[Name(value = "低速2", language = Language.Japanese)]
		EndSlowly2 = -20,
		[Name(value = "低速1", language = Language.Japanese)]
		EndSlowly1 = -10,
		[Name(value = "等速", language = Language.Japanese)]
		End = 0,
		[Name(value = "高速1", language = Language.Japanese)]
		EndRapidly1 = 10,
		[Name(value = "高速2", language = Language.Japanese)]
		EndRapidly2 = 20,
		[Name(value = "高速3", language = Language.Japanese)]
		EndRapidly3 = 30,
	}

	public enum DrawnAs : int
	{ 
		MaxAndMin,
		CenterAndAmplitude,
	}
	
	public enum ColorSpace : int
	{ 
		RGBA,
		HSVA,
	}

	public enum StandardColorType : int
	{
		[Name(value = "固定", language = Language.Japanese)]
		Fixed = 0,
		[Name(value = "ランダム", language = Language.Japanese)]
		Random = 1,
		[Name(value = "イージング", language = Language.Japanese)]
		Easing = 2,
		[Name(value = "Fカーブ(RGBA)", language = Language.Japanese)]
		FCurve = 3,
	}

	public enum TrackSizeType : int
	{
		[Name(value = "固定", language = Language.Japanese)]
		Fixed = 0,
	}

	public class ColorEasingParamater
	{ 
		[Name(language = Language.Japanese, value = "始点")]
		[Description(language = Language.Japanese, value = "イージングの始点")]
		public Value.ColorWithRandom Start
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点")]
		[Description(language = Language.Japanese, value = "イージングの終点")]
		public Value.ColorWithRandom End
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "始点速度")]
		[Description(language = Language.Japanese, value = "始点速度")]
		public Value.Enum<EasingStart> StartSpeed
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点速度")]
		[Description(language = Language.Japanese, value = "終点速度")]
		public Value.Enum<EasingEnd> EndSpeed
		{
			get;
			private set;
		}

		internal ColorEasingParamater()
		{
			Start = new Value.ColorWithRandom(255, 255, 255, 255);
			End = new Value.ColorWithRandom(255, 255, 255, 255);
			Start.Link = End;
			End.Link = Start;
			StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
			EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
		}
	}

	public class ColorFCurveParameter
	{
		[Name(language = Language.Japanese, value = "Fカーブ")]
		[Description(language = Language.Japanese, value = "Fカーブ")]
		[Shown(Shown = true)]
		public Value.FCurveColorRGBA FCurve
		{
			get;
			private set;
		}

		internal ColorFCurveParameter()
		{
			FCurve = new Value.FCurveColorRGBA();

			FCurve.R.DefaultValueRangeMax = 256.0f;
			FCurve.R.DefaultValueRangeMin = 0.0f;
			FCurve.G.DefaultValueRangeMax = 256.0f;
			FCurve.G.DefaultValueRangeMin = 0.0f;
			FCurve.B.DefaultValueRangeMax = 256.0f;
			FCurve.B.DefaultValueRangeMin = 0.0f;
			FCurve.A.DefaultValueRangeMax = 256.0f;
			FCurve.A.DefaultValueRangeMin = 0.0f;
		}
	}

    public class FloatEasingParamater
    {
        [Name(language = Language.Japanese, value = "始点")]
        [Description(language = Language.Japanese, value = "イージングの始点")]
        public Value.FloatWithRandom Start
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点")]
        [Description(language = Language.Japanese, value = "イージングの終点")]
        public Value.FloatWithRandom End
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "始点速度")]
        [Description(language = Language.Japanese, value = "始点速度")]
        public Value.Enum<EasingStart> StartSpeed
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点速度")]
        [Description(language = Language.Japanese, value = "終点速度")]
        public Value.Enum<EasingEnd> EndSpeed
        {
            get;
            private set;
        }

        internal FloatEasingParamater(float value = 0.0f, float max = float.MaxValue, float min = float.MinValue)
        {
            Start = new Value.FloatWithRandom(value, max, min);
            End = new Value.FloatWithRandom(value, max, min);
            StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
            EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
        }
    }

    public class Vector2DEasingParamater
    {
        [Name(language = Language.Japanese, value = "始点")]
        [Description(language = Language.Japanese, value = "イージングの始点")]
        public Value.Vector2DWithRandom Start
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点")]
        [Description(language = Language.Japanese, value = "イージングの終点")]
        public Value.Vector2DWithRandom End
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "始点速度")]
        [Description(language = Language.Japanese, value = "始点速度")]
        public Value.Enum<EasingStart> StartSpeed
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点速度")]
        [Description(language = Language.Japanese, value = "終点速度")]
        public Value.Enum<EasingEnd> EndSpeed
        {
            get;
            private set;
        }

        internal Vector2DEasingParamater()
        {
            Start = new Value.Vector2DWithRandom(0, 0);
            End = new Value.Vector2DWithRandom(0, 0);
            StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
            EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
        }
    }

	public class Vector3DEasingParamater
	{
		[Name(language = Language.Japanese, value = "始点")]
		[Description(language = Language.Japanese, value = "イージングの始点")]
		public Value.Vector3DWithRandom Start
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点")]
		[Description(language = Language.Japanese, value = "イージングの終点")]
		public Value.Vector3DWithRandom End
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "始点速度")]
		[Description(language = Language.Japanese, value = "始点速度")]
		public Value.Enum<EasingStart> StartSpeed
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点速度")]
		[Description(language = Language.Japanese, value = "終点速度")]
		public Value.Enum<EasingEnd> EndSpeed
		{
			get;
			private set;
		}

		internal Vector3DEasingParamater(float defaultX = 0.0f, float defaultY = 0.0f, float defaultZ = 0.0f)
		{
			Start = new Value.Vector3DWithRandom(defaultX, defaultY, defaultZ);
			End = new Value.Vector3DWithRandom(defaultX, defaultY, defaultZ);
			StartSpeed = new Value.Enum<EasingStart>(EasingStart.Start);
			EndSpeed = new Value.Enum<EasingEnd>(EasingEnd.End);
		}
	}

	public class Vector3DFCurveParameter
	{
		[Name(language = Language.Japanese, value = "Fカーブ")]
		[Description(language = Language.Japanese, value = "Fカーブ")]
		[Shown(Shown = true)]
		public Value.FCurveVector3D FCurve
		{
			get;
			private set;
		}

		public Vector3DFCurveParameter()
		{
			FCurve = new Value.FCurveVector3D();
			
			FCurve.X.DefaultValueRangeMax = 10.0f;
			FCurve.X.DefaultValueRangeMin = -10.0f;
			FCurve.Y.DefaultValueRangeMax = 10.0f;
			FCurve.Y.DefaultValueRangeMin = -10.0f;
			FCurve.Z.DefaultValueRangeMax = 10.0f;
			FCurve.Z.DefaultValueRangeMin = -10.0f;
		}
	}

	/// <summary>
	/// 入出力に関する属性
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = false,
	Inherited = false)]
	public class IOAttribute : Attribute
	{
		public IOAttribute()
		{
			Import = true;
			Export = true;
		}

		public bool Import
		{
			get;
			set;
		}

		public bool Export
		{
			get;
			set;
		}
	}

	/// <summary>
	/// 表示に関する属性
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = false,
	Inherited = false)]
	public class ShownAttribute : Attribute
	{
		public ShownAttribute()
		{
			Shown = true;
		}

		public bool Shown
		{
			get;
			set;
		}
	}

	/// <summary>
	/// Undoに関する属性
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = false,
	Inherited = false)]
	public class UndoAttribute : Attribute
	{
		public UndoAttribute()
		{
			Undo = true;
		}

		public bool Undo
		{
			get;
			set;
		}
	}

	/// <summary>
	/// 選択肢に関する属性
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = false,
	Inherited = false)]
	public class SelectorAttribute : Attribute
	{
		public int ID
		{
			get;
			set;
		}

		public SelectorAttribute()
		{
			ID = -1;
		}
	}

	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = false,
	Inherited = false)]
	public class SelectedAttribute : Attribute
	{
		public int ID
		{
			get;
			set;
		}

		public int Value
		{
			get;
			set;
		}

		public SelectedAttribute()
		{
			ID = -1;
			Value = -1;
		}
	}
}
