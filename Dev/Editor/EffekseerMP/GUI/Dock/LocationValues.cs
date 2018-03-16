using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class LocationValues : DockPanel
	{
		Component.ParameterList paramerterList = null;
		
		bool isFiestUpdate = true;

		public LocationValues()
		{
			Label = Resources.GetString("Position");

			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.LocationValues));

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;

			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
			}

			paramerterList.Update();
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					paramerterList.SetValue(((Data.Node)Core.SelectedNode).LocationValues);
				}
				else
				{
					paramerterList.SetValue(null);
				}
			}
			else
			{
				paramerterList.SetValue(null);
			}
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}
	}
}
