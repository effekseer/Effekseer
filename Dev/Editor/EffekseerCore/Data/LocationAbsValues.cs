using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public enum LocalForceFieldType
	{
		[Name(value = "無し", language = Language.Japanese)]
		[Name(value = "None", language = Language.English)]
		None = 0,

		[Name(value = "乱流", language = Language.Japanese)]
		[Name(value = "Turbulence", language = Language.English)]
		Turbulence = 1,
	}

	public class LocalForceFieldTurbulence
	{
		[Name(value = "ランダムシード", language = Language.Japanese)]
		[Name(value = "Random Seed", language = Language.English)]
		[Description(language = Language.Japanese, value = "数値に応じて乱流の流れ方が変わります。")]
		[Description(language = Language.English, value = "The flow of turbulence changes according to the value.")]
		public Value.Int Seed { get; private set; }

		[Name(value = "領域の大きさ", language = Language.Japanese)]
		[Name(value = "Field scale", language = Language.English)]
		[Description(language = Language.Japanese, value = "値を大きくすると乱流の幅が大きくなります。")]
		[Description(language = Language.English, value = "Larger value ​​increases the turbulence width.")]
		public Value.Float FieldScale { get; private set; }

		[Name(value = "強さ", language = Language.Japanese)]
		[Name(value = "Strength", language = Language.English)]
		[Description(language = Language.Japanese, value = "乱流がパーティクルに与える力の強さです。")]
		[Description(language = Language.English, value = "The strength of turbulence on particles.")]
		public Value.Float Strength { get; private set; }

		[Name(value = "複雑さ", language = Language.Japanese)]
		[Name(value = "Complexity", language = Language.English)]
		[Description(language = Language.Japanese, value = "値を大きくすると乱流が複雑になります。ただし、処理が重くなります。")]
		[Description(language = Language.English, value = "Larger value ​​complicates turbulence. However, processing becomes heavy.")]
		public Value.Int Octave { get; private set; }

		public LocalForceFieldTurbulence()
		{
			Seed = new Value.Int(1, int.MaxValue, 0, 1);
			FieldScale = new Value.Float(4.0f, float.MaxValue, 0, 0.1f);
			Strength = new Value.Float(0.1f, float.MaxValue, 0, 0.1f);
			Octave = new Value.Int(1, 10, 1, 1);
		}
	}
	public class LocalForceField : IEditableValueCollection
	{
		[Selector(ID = 10)]
		public Value.Enum<LocalForceFieldType> Type
		{
			get;
			private set;
		}

		[Selected(ID = 10, Value = (int)LocalForceFieldType.Turbulence)]
		public LocalForceFieldTurbulence Turbulence
		{
			get;
			private set;
		}

		int number = 1;
		public LocalForceField(int number)
		{
			this.number = number;
			Type = new Value.Enum<LocalForceFieldType>();
			Turbulence = new LocalForceFieldTurbulence();
		}

		public EditableValue[] GetValues()
		{
			var ret = new List<EditableValue>();

			EditableValue ev = new EditableValue();
			ev.Value = Type;

			{
				if (Core.Language == Language.English)
				{
					ev.Title = "ForceField" + number.ToString();
				}

				if (Core.Language == Language.Japanese)
				{
					ev.Title = "力場" + number.ToString();
				}

				ev.Description = "";
			}

			ev.IsShown = true;
			ev.IsUndoEnabled = true;
			ev.SelfSelectorID = 10;
			ret.Add(ev);

			ret.Add(EditableValue.Create(Turbulence, this.GetType().GetProperty("Turbulence")));
			return ret.ToArray();
		}

		public event ChangedValueEventHandler OnChanged;

		public void Changed()
		{
			if (OnChanged != null)
			{
				OnChanged(this, null);
			}
		}
	}

	public class LocationAbsValues
	{
		[IO(Export = true)]
		public LocalForceField LocalForceField1 { get; private set; }

		[IO(Export = true)]
		public LocalForceField LocalForceField2 { get; private set; }

		[IO(Export = true)]
		public LocalForceField LocalForceField3 { get; private set; }


		[Name(value = "力場(1.4)", language = Language.Japanese)]
		[Name(value = "ForceField(1.4)", language = Language.English)]
		[Description(language = Language.Japanese, value = "バージョン1.4以前の力場の機能です。")]
		[Description(language = Language.English, value = "ForceField before 1.4")]
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 0)]
		[IO(Export = true)]
		public NoneParamater None
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public GravityParamater Gravity
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		public AttractiveForceParamater AttractiveForce
		{
			get;
			private set;
		}

		internal LocationAbsValues()
		{
			LocalForceField1 = new LocalForceField(1);
			LocalForceField2 = new LocalForceField(2);
			LocalForceField3 = new LocalForceField(3);

			Type = new Value.Enum<ParamaterType>(ParamaterType.None);
			None = new NoneParamater();
			Gravity = new GravityParamater();
			AttractiveForce = new AttractiveForceParamater();
		}

		public class NoneParamater
		{
			internal NoneParamater()
			{
			}
		}

		public class GravityParamater
		{
			[Name(language = Language.Japanese, value = "重力")]
			[Description(language = Language.Japanese, value = "インスタンスにかかる重力")]
			[Name(language = Language.English, value = "Gravity")]
			[Description(language = Language.English, value = "Gravity's effect on the instance")]
			public Value.Vector3D Gravity
			{
				get;
				private set;
			}

			internal GravityParamater()
			{
				Gravity = new Value.Vector3D(0, 0, 0);
			}
		}
		
		public class AttractiveForceParamater
		{
			[Name(language = Language.Japanese, value = "引力")]
			[Description(language = Language.Japanese, value = "ターゲットの引力")]
			[Name(language = Language.English, value = "Attraction")]
			[Description(language = Language.English, value = "Strength of the point of attraction")]
			public Value.Float Force
			{
				get;
				private set;
			}
			
			[Name(language = Language.Japanese, value = "制御")]
			[Description(language = Language.Japanese, value = "移動方向をターゲット方向へ補正量")]
			[Name(language = Language.English, value = "Resistance")]
			[Description(language = Language.English, value = "Resistance to the directional pull of the attractor")]
			public Value.Float Control
			{
				get;
				private set;
			}
			
			[Name(language = Language.Japanese, value = "最小範囲")]
			[Description(language = Language.Japanese, value = "この範囲以内では引力がフルに掛かります")]
			[Name(language = Language.English, value = "Minimum Range")]
			[Description(language = Language.English, value = "Within this range, it will be affected by the attractor")]
			public Value.Float MinRange
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "最大範囲")]
			[Description(language = Language.Japanese, value = "この範囲以外では引力が掛かりません")]
			[Name(language = Language.English, value = "Maximum Range")]
			[Description(language = Language.English, value = "Outside this range, the attractor will have no effect on the instance")]
			public Value.Float MaxRange
			{
				get;
				private set;
			}

			internal AttractiveForceParamater()
			{
				Force = new Value.Float(0.0f, float.MaxValue, float.MinValue, 0.01f);
				Control = new Value.Float(1.0f, float.MaxValue, float.MinValue, 0.1f);
				MinRange = new Value.Float(0.0f, 1000.0f, 0.0f, 1.0f);
				MaxRange = new Value.Float(0.0f, 1000.0f, 0.0f, 1.0f);
			}
		}
	
		public enum ParamaterType : int
		{
			[Name(value = "無し", language = Language.Japanese)]
			[Name(value = "None", language = Language.English)]
			None = 0,
			[Name(value = "重力", language = Language.Japanese)]
			[Name(value = "Gravity", language = Language.English)]
			Gravity = 1,
			[Name(value = "引力(ターゲット有り)", language = Language.Japanese)]
			[Name(value = "Attraction (if point is set)", language = Language.English)]
			AttractiveForce = 2,
		}
	}
}
