using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Data.Value
{
	public class DynamicEquationReference
	{
		public DynamicEquation Value
		{
			get;
			private set;
		}

		public void SetValue(DynamicEquation param)
		{
			if (param == Value) return;

			var old_value = Value;
			var new_value = param;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					Value = new_value;
				},
				() =>
				{
					Value = old_value;
				});

			Command.CommandManager.Execute(cmd);
		}

		public int Index
		{
			get
			{
				return Core.Dynamic.Equations.GetIndex(Value);
			}
		}
	}
}
