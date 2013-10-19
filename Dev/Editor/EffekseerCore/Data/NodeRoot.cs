using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class NodeRoot : NodeBase
	{
		internal NodeRoot()
			:base(null)
		{
			Name.SetValueDirectly("Root");
		}
	}
}
