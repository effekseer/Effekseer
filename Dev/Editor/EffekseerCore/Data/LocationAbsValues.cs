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

		internal LocationAbsValues()
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.None);
			None = new NoneParamater();
			Gravity = new GravityParamater();
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
	
		public enum ParamaterType : int
		{
			[Name(value = "無し", language = Language.Japanese)]
			None = 0,
			[Name(value = "重力", language = Language.Japanese)]
			Gravity = 1,
		}
	}
}
