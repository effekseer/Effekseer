using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Script
{
	public class ScriptBase
	{
		public ScriptPosition ScriptPosition
		{
			get;
			protected set;
		}

		public string UniqueName
		{
			get;
			protected set;
		}

		public string Author
		{
			get;
			protected set;
		}

		public string Title
		{
			get;
			protected set;
		}

		public string Description
		{
			get;
			protected set;
		}
	}
}
