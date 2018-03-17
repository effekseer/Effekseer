using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class CommonValues : DockPanel
	{
		Component.ParameterList paramerterList_Common = null;
		Component.ParameterList paramerterList_Node = null;

		bool isFiestUpdate = true;

		public CommonValues()
		{
			Label = Resources.GetString("BasicSettings");

			paramerterList_Node = new Component.ParameterList();
			paramerterList_Node.SetType(typeof(Data.NodeBase));
			paramerterList_Common = new Component.ParameterList();
			paramerterList_Common.SetType(typeof(Data.CommonValues));

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		public void FixValues()
		{
			paramerterList_Common.FixValues();
			paramerterList_Node.FixValues();
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

			paramerterList_Node.Update();
			paramerterList_Common.Update();
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				paramerterList_Node.SetValue(Core.SelectedNode);
				if (Core.SelectedNode is Data.Node)
				{
					paramerterList_Common.SetValue(((Data.Node)Core.SelectedNode).CommonValues);
				}
				else
				{
					paramerterList_Common.SetValue(null);
				}
			}
			else
			{
				paramerterList_Node.SetValue(null);
				paramerterList_Common.SetValue(null);
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
