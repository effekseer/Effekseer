﻿using System;
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
		Component.CopyAndPaste candp = null;

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

			candp = new Component.CopyAndPaste("BasicSettings", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			Icon = Images.GetIcon("PanelCommon");
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

			candp.Update();

			paramerterList_Node.Update();
			paramerterList_Common.Update();
		}

		object GetTargetObject()
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


		void Read()
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
