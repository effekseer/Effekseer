using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class NodeBaseValues : Data.IEditableValueCollection
	{
		Data.NodeBase node;

		public NodeBaseValues(Data.NodeBase node)
		{
			this.node = node;
		}
		public Data.EditableValue[] GetValues()
		{
			List<Data.EditableValue> values = new List<Data.EditableValue>();

			var valueIsRendered = Data.EditableValue.Create(node.IsRendered, node.GetType().GetProperty("IsRendered"));
			var valueName = Data.EditableValue.Create(node.Name, node.GetType().GetProperty("Name"));

			return new[]
			{
				valueIsRendered,
				valueName,
			};
		}

		public event ChangedValueEventHandler OnChanged;
	}

	class CommonValues : DockPanel
	{
		Component.ParameterList paramerterList_Common = null;
		Component.ParameterList paramerterList_Node = null;

		bool isFiestUpdate = true;

		public CommonValues()
		{
			Label = Resources.GetString("BasicSettings") + "###BasicSettings";

			paramerterList_Node = new Component.ParameterList();
			paramerterList_Node.SetType(typeof(Data.NodeBase));
			paramerterList_Common = new Component.ParameterList();
			paramerterList_Common.SetType(typeof(Data.CommonValues));

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			Icon = Images.GetIcon("PanelCommon");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("BasicSettings");
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
				paramerterList_Node.SetValue(new NodeBaseValues(Core.SelectedNode));

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
