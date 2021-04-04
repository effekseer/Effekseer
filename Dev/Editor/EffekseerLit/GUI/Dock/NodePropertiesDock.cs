using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class NodePropertiesDock : DockPanel
	{
		Component.ParameterList _paramerterList = null;

		public NodePropertiesDock()
		{
			// Common settings
			{
				Label = Icons.PanelOptions + Resources.GetString("Options") + "###NodePropertiesDock";

				TabToolTip = Resources.GetString("Options");

				NoPadding = true;
				NoScrollBar = true;
				NoCloseButton = true;
				AllowsShortTab = false;
			}

			_paramerterList = new Component.ParameterList();
			_paramerterList.SetType(typeof(Data.LocationValues));
			Controls.Add(_paramerterList);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();
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
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void Read()
		{
			_paramerterList.SetValue(GetTargetObject());
		}

		private object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).LocationValues;
				}
			}
			return null;
		}

		public void FixValues()
		{
			_paramerterList.FixValues();
		}
	}
}
