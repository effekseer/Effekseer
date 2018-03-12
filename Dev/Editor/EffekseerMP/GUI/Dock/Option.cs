using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Option : DockPanel
	{
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public Option()
		{
			Label = Resources.GetString("Options");
			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.OptionValues));

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;
		}

		protected override void UpdateInternal()
		{
			if(isFiestUpdate)
			{
				paramerterList.SetValue(Core.Option);
				isFiestUpdate = false;
			}

			paramerterList.Update();
		}

		void OnAfter(object sender, EventArgs e)
		{
			paramerterList.SetValue(Core.Option);
		}
	}
}
