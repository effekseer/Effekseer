using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class PathForCurve : Path
	{
		internal PathForCurve(Path basepath, string filter, bool isRelativeSaved, string abspath = "")
			: base(basepath, filter, isRelativeSaved, abspath)
		{
		}
	}
}
