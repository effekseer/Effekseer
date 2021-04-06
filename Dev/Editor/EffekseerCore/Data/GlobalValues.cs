using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class GlobalValues
	{
		[Key(key = "Global_RandomSeed")]
		public Value.Int RandomSeed
		{
			get;
			private set;
		}

		public GlobalValues()
		{
			RandomSeed = new Value.Int(-1, int.MaxValue - 1);
		}
	}
}
