using Effekseer.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class NodePropertiesDock : DockPanel
	{
		Component.ParameterList paramerterList_Common = null;
		Component.ParameterList paramerterList_Node = null;

		public NodePropertiesDock()
		{
			Label = Icons.PanelOptions + Resources.GetString("Options") + "###NodePropertiesDock";

			paramerterList_Node = new Component.ParameterList();
			paramerterList_Node.SetType(typeof(Data.NodeBase));
			paramerterList_Common = new Component.ParameterList();
			paramerterList_Common.SetType(typeof(Data.CommonValues));

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterNew;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = Resources.GetString("Options");

			NoPadding = true;
			NoScrollBar = true;
			NoCloseButton = true;
			AllowsShortTab = false;
		}

		public override void OnDisposed()
		{
			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterNew;
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}

		protected override void UpdateInternal()
		{
			paramerterList_Node.Update();
			paramerterList_Common.Update();
		}

		private object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).CommonValues;
				}
			}
			return null;
		}

		private void Read()
		{
			if (Core.SelectedNode != null)
			{
				paramerterList_Node.SetValue(new NodeBaseValues(Core.SelectedNode));
				paramerterList_Common.SetValue(GetTargetObject());
			}
			else
			{
				paramerterList_Node.SetValue(null);
				paramerterList_Common.SetValue(null);
			}
		}

		private void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		private void OnAfterNew(object sender, EventArgs e)
		{
			Read();
		}

		private void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}
	}
}
