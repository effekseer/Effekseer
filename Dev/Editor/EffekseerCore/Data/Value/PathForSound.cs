using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class PathForSound : Effekseer.Data.Value.Path
	{
		internal PathForSound(string filter, string abspath = "")
			: base(filter, abspath)
		{
		}
	}
}
