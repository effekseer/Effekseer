using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class PathForImage : Effekseer.Data.Value.Path
	{
		internal PathForImage(string filter, string abspath = "")
			: base(filter, abspath)
		{
		}
	}
}
