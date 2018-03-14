using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class CommonValues : DockPanel
	{
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public CommonValues()
		{
			Label = Resources.GetString("BasicSettings");
			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.CommonValues));

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
			}

			paramerterList.Update();
		}

		void OnAfter(object sender, EventArgs e)
		{
			
		}
	}
}
