using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Script
{
	public class CommandScript : ScriptBase
	{
		public Action Function
		{
			get;
			private set;
		}

		internal CommandScript(ScriptPosition scriptposition, string uniquename, string author, string title, string description, Action function)
		{
			ScriptPosition = scriptposition;
			UniqueName = uniquename;
			Author = author;
			Title = title;
			Description = description;
			Function = function;
		}
	}
}
