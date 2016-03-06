using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class LocationAbsValues
	{
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
