using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class CullingValues
	{
		[Selector(ID = 0)]
		public Value.Enum<ParamaterType> Type
		{
			get;
			private set;
		}

		public class SphereParamater
		{

		}

		public CullingValues()
		{
			Type = new Value.Enum<ParamaterType>(ParamaterType.None);
		}

		public enum ParamaterType : int
		{
			[Name(value = "なし", language = Language.Japanese)]
			None = 0,
			[Name(value = "球", language = Language.Japanese)]
			Sphere = 1,
		}
	}
}
