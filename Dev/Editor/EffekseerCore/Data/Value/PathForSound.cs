using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class PathForSound : Effekseer.Data.Value.Path
	{
		internal PathForSound(string filter, bool isRelativeSaved, string abspath = "")
			: base(filter, isRelativeSaved, abspath)
		{
		}
	}
}
