using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class GlobalValues
	{
		[Name(language = Language.Japanese, value = "ランダムシード")]
		[Description(language = Language.Japanese, value = "ランダムのシード。-1でシード指定なし")]
		[Name(language = Language.English, value = "Random seed")]
		[Description(language = Language.English, value = "Random's seed. When this value is -1, seed is not specified.")]
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
