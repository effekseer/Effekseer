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
		[Key(key = "BasicSettings_ParentEffectType_NotBind")]
		NotBind = 0,
		[Key(key = "BasicSettings_ParentEffectType_NotBind_Root")]
		NotBind_Root = 3,
		[Key(key = "BasicSettings_ParentEffectType_WhenCreating")]
		WhenCreating = 1,
		[Key(key = "BasicSettings_ParentEffectType_Already")]
		Already = 2,
	}

#if __EFFEKSEER_BUILD_VERSION16__
	public enum TranslationParentEffectType : int
	{
		[Key(key = "BasicSettings_ParentEffectType_NotBind")]
		NotBind = 0,
		[Key(key = "BasicSettings_ParentEffectType_NotBind_Root")]
		NotBind_Root = 3,
		[Key(key = "BasicSettings_ParentEffectType_WhenCreating")]
		WhenCreating = 1,
		[Key(key = "BasicSettings_ParentEffectType_Already")]
		Already = 2,
		[Name(language = Language.Japanese, value = "なし-親を追従")]
		[Name(language = Language.English, value = "NotBind-FollowParant")]
		NotBind_FollowParent = 4,
		[Name(language = Language.Japanese, value = "生成時のみ-親を追従")]
		[Name(language = Language.English, value = "WhenCreating-FollowParant")]
		WhenCreating_FollowParent = 5,
	}
#endif

	public enum AlphaBlendType : int
	{
		[Key(key = "AlphaBlendType_Opacity")]
		Opacity = 0,
		[Key(key = "AlphaBlendType_Blend")]
		Blend = 1,
		[Key(key = "AlphaBlendType_Add")]
		Add = 2,
		[Key(key = "AlphaBlendType_Sub")]
		Sub = 3,
		[Key(key = "AlphaBlendType_Mul")]
		Mul = 4,
	}

	public enum RenderingOrder : int
	{
		[Key(key = "RS_RenderingOrder_FirstCreatedInstanceIsFirst")]
		FirstCreatedInstanceIsFirst = 0,
		[Key(key = "RS_RenderingOrder_FirstCreatedInstanceIsLast")]
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

	public enum EasingEnd : int
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
		[Key(key = "StandardColorType_Fixed")]
		Fixed = 0,
		[Key(key = "StandardColorType_Random")]
		Random = 1,
		[Key(key = "StandardColorType_Easing")]
		Easing = 2,
		[Key(key = "StandardColorType_FCurve")]
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
		[Key(key = "Easing_Start")]
		public Value.ColorWithRandom Start
		{
			get;
			private set;
		}

		[Key(key = "Easing_End")]
		public Value.ColorWithRandom End
		{
			get;
			private set;
		}

		[Key(key = "Easing_StartSpeed")]
		public Value.Enum<EasingStart> StartSpeed
		{
			get;
			private set;
		}

		[Key(key = "Easing_EndSpeed")]
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
		[Key(key = "FCurve")]
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
		[Key(key = "Easing_Start")]
		public Value.FloatWithRandom Start
        {
            get;
            private set;
        }

		[Key(key = "Easing_End")]
		public Value.FloatWithRandom End
        {
            get;
            private set;
        }

		[Key(key = "Easing_StartSpeed")]
		public Value.Enum<EasingStart> StartSpeed
        {
            get;
            private set;
        }

		[Key(key = "Easing_EndSpeed")]
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
		[Key(key="Easing_Start")]
        public Value.Vector2DWithRandom Start
        {
            get;
            private set;
        }

		[Key(key="Easing_End")]
		public Value.Vector2DWithRandom End
        {
            get;
            private set;
        }

		[Key(key="Easing_StartSpeed")]
		public Value.Enum<EasingStart> StartSpeed
        {
            get;
            private set;
        }

		[Key(key="Easing_EndSpeed")]
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
		[Key(key = "Easing_Start")]
		public Value.Vector3DWithRandom Start
		{
			get;
			private set;
		}

		[Key(key = "Easing_End")]
		public Value.Vector3DWithRandom End
		{
			get;
			private set;
		}

		[Key(key = "Easing_StartSpeed")]
		public Value.Enum<EasingStart> StartSpeed
		{
			get;
			private set;
		}

		[Key(key = "Easing_EndSpeed")]
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
		[Key(key = "FCurve")]
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
	/// For collection to create a treenode in GUI
	/// </summary>
	[AttributeUsage(
		AttributeTargets.Property | AttributeTargets.Field,
	AllowMultiple = true,
	Inherited = false)]
	public class TreeNodeAttribute : Attribute
	{
		public string key
		{
			get;
			set;
		}

		public string id
		{
			get;
			set;
		}
	}

	/// <summary>
	/// A class to show editable value in parameter list
	/// </summary>
	public class EditableValue
	{
		public object Value;
		public object Title = new MultiLanguageString(string.Empty);
		public object Description = new MultiLanguageString(string.Empty);
		public bool IsUndoEnabled;
		public bool IsShown = true;
		public int SelfSelectorID = -1;
		public string TreeNodeID = null;

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

			var key = KeyAttribute.GetKey(attributes);
			var nameKey = key + "_Name";
			if(string.IsNullOrEmpty(key))
			{
				nameKey = info.ReflectedType.Name + "_" + info.Name + "_Name";
			}

			if(MultiLanguageTextProvider.HasKey(nameKey))
			{
				ret.Title = new MultiLanguageString(nameKey);
			}
			else
			{
				ret.Title = NameAttribute.GetName(attributes);
				//if (!string.IsNullOrEmpty(ret.Title.ToString()))
				//{
				//	System.IO.File.AppendAllText("kv.csv", nameKey + ","  + "\"" + ret.Title.ToString() + "\"" + "\r\n");
				//}
			}

			var descKey = key + "_Desc";
			if (string.IsNullOrEmpty(key))
			{
				descKey = info.ReflectedType.Name + "_" + info.Name + "_Desc";
			}

			if (MultiLanguageTextProvider.HasKey(descKey))
			{
				ret.Description = new MultiLanguageString(descKey);
			}
			else
			{
				ret.Description = DescriptionAttribute.GetDescription(attributes);

				//if(!string.IsNullOrEmpty(ret.Description.ToString()))
				//{
				//	System.IO.File.AppendAllText("kv.csv", descKey + "," + "\"" + ret.Description.ToString() + "\"" + "\r\n");
				//}
			}

			var treeNode = attributes.OfType<TreeNodeAttribute>().FirstOrDefault();

			if (treeNode != null)
			{
				ret.TreeNodeID = treeNode.id;

				if (MultiLanguageTextProvider.HasKey(treeNode.key))
				{
					ret.Title = new MultiLanguageString(treeNode.key);
				}

			}

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
