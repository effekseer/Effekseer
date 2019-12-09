using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class LocationAbsValues : DockPanel
	{
		Component.CopyAndPaste candp = null;
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public LocationAbsValues()
		{
			Label = Resources.GetString("AttractionForces") + "###AttractionForces";

			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.LocationAbsValues));
			candp = new Component.CopyAndPaste("AttractionForces", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;
			
			Read();

			Icon = Images.GetIcon("PanelLocationAbs");
			TabToolTip = Resources.GetString("AttractionForces");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
			}

			candp.Update();

			paramerterList.Update();
		}

		void Read()
		{
			paramerterList.SetValue(GetTargetObject());
		}

		object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).LocationAbsValues;
				}
			}
			return null;
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
