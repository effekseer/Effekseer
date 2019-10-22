using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.Data;

namespace Effekseer.GUI.Dialog
{
	class RenameNode : IRemovableControl
	{

		bool opened = true;

		bool isFirstUpdate = true;

		public bool ShouldBeRemoved { get; private set; } = false;

		protected NodeBase selectedNode;
		private string name;
		public void Show(NodeBase selectedNode)
		{
			this.selectedNode = selectedNode;
			this.name = selectedNode.Name;
			Manager.AddControl(this);
		}

		public void Update()
		{
			if (isFirstUpdate)
			{
				Manager.NativeManager.OpenPopup(Resources.GetString("RenameNode") + "###RenameNodeDialog");
				isFirstUpdate = false;
			}

			if (Manager.NativeManager.BeginPopupModal(Resources.GetString("RenameNode") + "###RenameNodeDialog", ref opened, swig.WindowFlags.AlwaysAutoResize))
			{

				if (Manager.NativeManager.InputText("", this.name))
				{
					this.name = Manager.NativeManager.GetInputTextResult();
				}
				Manager.NativeManager.Separator();

				if (Manager.NativeManager.Button("OK", 100))
				{
					this.selectedNode.Name.Value = this.name;
					ShouldBeRemoved = true;
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.Button(Resources.GetString("Cancel"), 100))
				{
					ShouldBeRemoved = true;
				}

				Manager.NativeManager.EndPopup();
			}
			else
			{
				ShouldBeRemoved = true;
			}
		}
	}
}
