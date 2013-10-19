using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class LocationValues
	{
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 0)]
		[IO(Export = true)]
		public FixedParamater Fixed
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 1)]
		[IO(Export = true)]
		public PVAParamater PVA
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 2)]
		[IO(Export = true)]
		public Vector3DEasingParamater Easing
		{
			get;
			private set;
		}

		[Selected(ID = 0, Value = 3)]
		[IO(Export = true)]
		public Vector3DFCurveParameter LocationFCurve
		{
			get;
			private set;
		}

		internal LocationValues()
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.Fixed);
			Fixed = new FixedParamater();
			PVA = new PVAParamater();
			Easing = new Vector3DEasingParamater();
			LocationFCurve = new Vector3DFCurveParameter();
		}

		public class FixedParamater
		{
			[Name(language = Language.Japanese, value = "位置")]
			[Description(language = Language.Japanese, value = "インスタンスの位置")]
			public Value.Vector3D Location
			{
				get;
				private set;
			}

			internal FixedParamater()
			{
				Location = new Value.Vector3D(0, 0, 0);
			}
		}

		public class PVAParamater
		{
			[Name(language = Language.Japanese, value = "位置")]
			[Description(language = Language.Japanese, value = "インスタンスの初期位置")]
			public Value.Vector3DWithRandom Location
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "速度")]
			[Description(language = Language.Japanese, value = "インスタンスの初期速度")]
			public Value.Vector3DWithRandom Velocity
			{
				get;
				private set;
			}

			[Name(language = Language.Japanese, value = "加速度")]
			[Description(language = Language.Japanese, value = "インスタンスの初期加速度")]
			public Value.Vector3DWithRandom Acceleration
			{
				get;
				private set;
			}

			internal PVAParamater()
			{
				Location = new Value.Vector3DWithRandom(0, 0, 0);
				Velocity = new Value.Vector3DWithRandom(0, 0, 0);
				Acceleration = new Value.Vector3DWithRandom(0, 0, 0);
			}
		}

		public enum ParamaterType : int
		{
			[Name(value = "位置", language = Language.Japanese)]
			Fixed = 0,
			[Name(value = "位置・速度・加速度", language = Language.Japanese)]
			PVA = 1,
			[Name(value = "イージング", language = Language.Japanese)]
			Easing = 2,
			[Name(value = "位置(Fカーブ)", language = Language.Japanese)]
			LocationFCurve = 3,
		}
	}
}
