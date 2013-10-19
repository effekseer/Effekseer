using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Script
{
	public class SelectedScript : ScriptBase
	{
		public Action<Data.NodeBase,Object> Function
		{
			get;
			private set;
		}

		internal SelectedScript(ScriptPosition scriptposition, string uniquename, string author, string title, string description, Action<Data.NodeBase, Object> function)
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
