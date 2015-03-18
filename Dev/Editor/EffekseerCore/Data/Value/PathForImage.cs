using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class PathForImage : Effekseer.Data.Value.Path
	{
		internal PathForImage(string filter, bool isRelativeSaved, string abspath = "")
			: base(filter, isRelativeSaved, abspath)
		{
		}
	}
}
