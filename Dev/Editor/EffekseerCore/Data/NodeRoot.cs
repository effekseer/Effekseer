using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data
{
	public class NodeRoot : NodeBase
	{
		private Value.Path path;

		internal NodeRoot()
			:base(null)
		{
			path = new Value.Path(null, Resources.GetString("EffekseerParticleFilter"), false);
			Name.SetValueDirectly("Root");
		}

		public Value.Path GetPath()
		{
			return path;
		}

		public string GetFullPath()
		{
			return path.GetAbsolutePath();
		}

		public void SetFullPath(string fullpath)
		{
			path.SetAbsolutePathDirectly(fullpath);
		}
	}
}
