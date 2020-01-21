using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
    public enum RecordingExporterType : int
    {
        Sprite,
        SpriteSheet,
        Gif,
        Avi,
    }

    public enum RecordingTransparentMethodType : int
    {
        None,
        Original,
        GenerateAlpha,
    }

	public enum RecordingStorageTargetTyoe : int
	{
		Global,
		Local,
	}
    public enum ParentEffectType : int
	{
		[Name(value = "なし", language = Language.Japanese)]
		[Name(value = "Never", language = Language.English)]
		NotBind = 0,
		[Name(value = "なし(Root依存)-非推奨", language = Language.Japanese)]
		[Name(value = "Root dependent (deprecated)", language = Language.English)]
		NotBind_Root = 3,
		[Name(value = "生成時のみ", language = Language.Japanese)]
		[Name(value = "Only on create", language = Language.English)]
		WhenCreating = 1,
		[Name(value = "常時", language = Language.Japanese)]
		[Name(value = "Always", language = Language.English)]
		Already = 2,
	}

	public enum AlphaBlendType : int
	{
		[Name(value = "不透明", language = Language.Japanese)]
		[Name(value = "Opacity", language = Language.English)]
		Opacity = 0,
		[Name(value = "通常", language = Language.Japanese)]
		[Name(value = "Blend", language = Language.English)]
		Blend = 1,
		[Name(value = "加算", language = Language.Japanese)]
		[Name(value = "Additive", language = Language.English)]
		Add = 2,
		[Name(value = "減算", language = Language.Japanese)]
		[Name(value = "Subtract", language = Language.English)]
		Sub = 3,
		[Name(value = "乗算", language = Language.Japanese)]
		[Name(value = "Multiply", language = Language.English)]
		Mul = 4,
	}

	public enum RenderingOrder : int
	{
		[Name(value = "生成順", language = Language.Japanese)]
		[Name(value = "Order of spawn", language = Language.English)]
		FirstCreatedInstanceIsFirst = 0,
		[Name(value = "生成順の逆", language = Language.Japanese)]
		[Name(value = "Reversed", language = Language.English)]
		FirstCreatedInstanceIsLast = 1,
	}

	public enum CullingValues : int
	{
		[Name(value = "表表示", language = Language.Japanese)]
		[Name(value = "Front view", language = Language.English)]
		Front = 0,
		[Name(value = "裏表示", language = Language.Japanese)]
		[Name(value = "Back view", language = Language.English)]
		Back = 1,
		[Name(value = "両面表示", language = Language.Japanese)]
		[Name(value = "Double-sided", language = Language.English)]
		Double = 2,
	}

	public enum EasingStart : int
	{
		[Name(value = "低速3", language = Language.Japanese)]
		[Name(value = "Slowest", language = Language.English)]
		StartSlowly3 = -30,
		[Name(value = "低速2", language = Language.Japanese)]
		[Name(value = "Slower", language = Language.English)]
		StartSlowly2 = -20,
		[Name(value = "低速1", language = Language.Japanese)]
		[Name(value = "Slow", language = Language.English)]
		StartSlowly1 = -10,
		[Name(value = "等速", language = Language.Japanese)]
		[Name(value = "Normal", language = Language.English)]
		Start = 0,
		[Name(value = "高速1", language = Language.Japanese)]
		[Name(value = "Fast", language = Language.English)]
		StartRapidly1 = 10,
		[Name(value = "高速2", language = Language.Japanese)]
		[Name(value = "Faster", language = Language.English)]
		StartRapidly2 = 20,
		[Name(value = "高速3", language = Language.Japanese)]
		[Name(value = "Fastest", language = Language.English)]
		StartRapidly3 = 30,
	}

	public enum EasingEnd : int
	{
		[Name(value = "低速3", language = Language.Japanese)]
		[Name(value = "Slowest", language = Language.English)]
		EndSlowly3 = -30,
		[Name(value = "低速2", language = Language.Japanese)]
		[Name(value = "Slower", language = Language.English)]
		EndSlowly2 = -20,
		[Name(value = "低速1", language = Language.Japanese)]
		[Name(value = "Slow", language = Language.English)]
		EndSlowly1 = -10,
		[Name(value = "等速", language = Language.Japanese)]
		[Name(value = "Normal", language = Language.English)]
		End = 0,
		[Name(value = "高速1", language = Language.Japanese)]
		[Name(value = "Fast", language = Language.English)]
		EndRapidly1 = 10,
		[Name(value = "高速2", language = Language.Japanese)]
		[Name(value = "Faster", language = Language.English)]
		EndRapidly2 = 20,
		[Name(value = "高速3", language = Language.Japanese)]
		[Name(value = "Fastest", language = Language.English)]
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
		[Name(value = "Fixed", language = Language.English)]
		Fixed = 0,
		[Name(value = "ランダム", language = Language.Japanese)]
		[Name(value = "Random", language = Language.English)]
		Random = 1,
		[Name(value = "イージング", language = Language.Japanese)]
		[Name(value = "Easing", language = Language.English)]
		Easing = 2,
		[Name(value = "Fカーブ(RGBA)", language = Language.Japanese)]
		[Name(value = "F-Curve (RGBA)", language = Language.English)]
		FCurve = 3,
	}

	public enum TrackSizeType : int
	{
		[Name(value = "固定", language = Language.Japanese)]
		[Name(value = "Fixed", language = Language.English)]
		Fixed = 0,
	}

	public class ColorEasingParamater
	{ 
		[Name(language = Language.Japanese, value = "始点")]
		[Description(language = Language.Japanese, value = "イージングの始点")]
		[Name(language = Language.English, value = "Start")]
		[Description(language = Language.English, value = "Starting point of easing")]
		public Value.ColorWithRandom Start
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点")]
		[Description(language = Language.Japanese, value = "イージングの終点")]
		[Name(language = Language.English, value = "End")]
		[Description(language = Language.English, value = "Value of easing at end")]
		public Value.ColorWithRandom End
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "始点速度")]
		[Description(language = Language.Japanese, value = "始点速度")]
		[Name(language = Language.English, value = "Ease In")]
		[Description(language = Language.English, value = "Initial rate of easing")]
		public Value.Enum<EasingStart> StartSpeed
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点速度")]
		[Description(language = Language.Japanese, value = "終点速度")]
		[Name(language = Language.English, value = "Ease Out")]
		[Description(language = Language.English, value = "Rate of easing at end")]
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
		[Name(language = Language.English, value = "F-Curve")]
		[Description(language = Language.English, value = "F-Curve")]
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
		[Name(language = Language.English, value = "Start")]
		[Description(language = Language.English, value = "Starting point of easing")]
        public Value.FloatWithRandom Start
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点")]
        [Description(language = Language.Japanese, value = "イージングの終点")]
		[Name(language = Language.English, value = "End")]
		[Description(language = Language.English, value = "Value of easing at end")]
        public Value.FloatWithRandom End
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "始点速度")]
        [Description(language = Language.Japanese, value = "始点速度")]
		[Name(language = Language.English, value = "Ease In")]
		[Description(language = Language.English, value = "Initial rate of easing")]
        public Value.Enum<EasingStart> StartSpeed
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点速度")]
        [Description(language = Language.Japanese, value = "終点速度")]
		[Name(language = Language.English, value = "Ease Out")]
		[Description(language = Language.English, value = "Rate of easing at end")]
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
		[Name(language = Language.English, value = "Start")]
		[Description(language = Language.English, value = "Starting point of easing")]
        public Value.Vector2DWithRandom Start
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点")]
        [Description(language = Language.Japanese, value = "イージングの終点")]
		[Name(language = Language.English, value = "End")]
		[Description(language = Language.English, value = "Value of easing at end")]
        public Value.Vector2DWithRandom End
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "始点速度")]
        [Description(language = Language.Japanese, value = "始点速度")]
		[Name(language = Language.English, value = "Ease In")]
		[Description(language = Language.English, value = "Initial rate of easing")]
        public Value.Enum<EasingStart> StartSpeed
        {
            get;
            private set;
        }

        [Name(language = Language.Japanese, value = "終点速度")]
        [Description(language = Language.Japanese, value = "終点速度")]
		[Name(language = Language.English, value = "Ease Out")]
		[Description(language = Language.English, value = "Rate of easing at end")]
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
		[Name(language = Language.English, value = "Start")]
		[Description(language = Language.English, value = "Starting point of easing")]
		public Value.Vector3DWithRandom Start
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点")]
		[Description(language = Language.Japanese, value = "イージングの終点")]
		[Name(language = Language.English, value = "End")]
		[Description(language = Language.English, value = "Value of easing at end")]
		public Value.Vector3DWithRandom End
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "始点速度")]
		[Description(language = Language.Japanese, value = "始点速度")]
		[Name(language = Language.English, value = "Ease In")]
		[Description(language = Language.English, value = "Initial rate of easing")]
		public Value.Enum<EasingStart> StartSpeed
		{
			get;
			private set;
		}

		[Name(language = Language.Japanese, value = "終点速度")]
		[Description(language = Language.Japanese, value = "終点速度")]
		[Name(language = Language.English, value = "Ease Out")]
		[Description(language = Language.English, value = "Rate of easing at end")]
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
		[Name(language = Language.English, value = "F-Curve")]
		[Description(language = Language.English, value = "F-Curve")]
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
	/// A value for a selector
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

	/// <summary>
	/// A values which is shown or hidden with a selector
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = true,
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

	/// <summary>
	/// A class to show editable value in parameter list
	/// </summary>
	public class EditableValue
	{
		public object Value;
		public string Title = string.Empty;
		public string Description = string.Empty;
		public bool IsUndoEnabled;
		public bool IsShown = true;
		public int SelfSelectorID = -1;

		/// <summary>
		/// If this value is larger than 0, target selector id is used to show it.
		/// </summary>
		public int TargetSelectorID = -1;

		/// <summary>
		/// If selector value is included in this, this value is shown.
		/// </summary>
		public int[] RequiredSelectorValues = null;

		public static EditableValue Create(object o, System.Reflection.PropertyInfo info)
		{
			var ret = new EditableValue();
			ret.Value = o;

			var p = info;
			var attributes = p.GetCustomAttributes(false);

			var undo = attributes.Where(_ => _.GetType() == typeof(UndoAttribute)).FirstOrDefault() as UndoAttribute;
			if (undo != null && !undo.Undo)
			{
				ret.IsUndoEnabled = false;
			}
			else
			{
				ret.IsUndoEnabled = true;
			}

			var shown = attributes.Where(_ => _.GetType() == typeof(ShownAttribute)).FirstOrDefault() as ShownAttribute;

			if (shown != null && !shown.Shown)
			{
				ret.IsShown = false;
			}

			var selector_attribute = (from a in attributes where a is Data.SelectorAttribute select a).FirstOrDefault() as Data.SelectorAttribute;
			if (selector_attribute != null)
			{
				ret.SelfSelectorID = selector_attribute.ID;
			}

			// collect selected values
			var selectedAttributes = attributes.OfType<Data.SelectedAttribute>();
			if (selectedAttributes.Count() > 0)
			{
				if(selectedAttributes.Select(_=>_.ID).Distinct().Count() > 1)
				{
					throw new Exception("Same IDs are required.");
				}

				ret.TargetSelectorID = selectedAttributes.First().ID;
				ret.RequiredSelectorValues = selectedAttributes.Select(_ => _.Value).ToArray();
			}

			ret.Title = NameAttribute.GetName(attributes);
			ret.Description = DescriptionAttribute.GetDescription(attributes);

			return ret;
		}
	}

	/// <summary>
	/// An interface to special editable parameters
	/// </summary>
	public interface IEditableValueCollection
	{
		EditableValue[] GetValues();

		event ChangedValueEventHandler OnChanged;
	}

	public interface IResettableValue
	{
		void ResetValue();
	}
}
