using Effekseer.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class CommonValues : DockPanel
	{
		BindableComponent.ParameterList paramerterList_Common = null;
		BindableComponent.ParameterList paramerterList_Node = null;

		bool isFiestUpdate = true;

		public CommonValues()
		{
			Label = Icons.PanelCommon + MultiLanguageTextProvider.GetText("BasicSettings") + "###BasicSettings";
			DocPage = "common.html";

			paramerterList_Node = new BindableComponent.ParameterList();
			paramerterList_Node.SetType(typeof(Data.NodeBase));
			paramerterList_Common = new BindableComponent.ParameterList();
			paramerterList_Common.SetType(typeof(Data.CommonValues));

			CopyAndPaste = new BindableComponent.CopyAndPaste("BasicSettings", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("BasicSettings");
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

			Manager.NativeManager.Separator();

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