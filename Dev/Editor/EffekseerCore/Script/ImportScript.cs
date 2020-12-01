using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Script
{
	public class ImportScript : ScriptBase
	{
		public string Filter
		{
			get;
			private set;
		}

		public Action<string> Function
		{
			get;
			private set;
		}

		public ImportScript(ScriptPosition scriptposition, string uniquename, string author, string title, string description, string filter, Action<string> function)
		{
			ScriptPosition = scriptposition;
			UniqueName = uniquename;
			Author = author;
			Title = title;
			Description = description;
			Filter = filter;
			Function = function;
		}
	}
}
