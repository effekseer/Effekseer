using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
#if MATERIAL_ENABLED
	public class PathForMaterial : Path
	{
		internal PathForMaterial(string filter, bool isRelativeSaved, string abspath = "")
			: base(filter, isRelativeSaved, abspath)
		{
		}
	}
#endif
}
