using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class PathForModel : Path
	{
		internal PathForModel(Path basepath, string filter, bool isRelativeSaved, string abspath = "")
			: base(basepath, filter, isRelativeSaved, abspath)
		{
		}
	}
}
