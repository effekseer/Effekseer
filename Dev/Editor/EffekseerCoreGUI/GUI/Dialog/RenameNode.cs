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
			}

			if (Manager.NativeManager.BeginPopupModal(Resources.GetString("RenameNode") + "###RenameNodeDialog", ref opened, swig.WindowFlags.AlwaysAutoResize))
			{
				if (isFirstUpdate)
				{
					Manager.NativeManager.SetKeyboardFocusHere();
				}

				if (Manager.NativeManager.InputText("###RenameNodeInput", this.name, 
					swig.InputTextFlags.AutoSelectAll | swig.InputTextFlags.EnterReturnsTrue))
				{
					this.selectedNode.Name.Value = this.name;
					ShouldBeRemoved = true;
				}
				this.name = Manager.NativeManager.GetInputTextResult();
				
				if (Manager.NativeManager.IsKeyPressed(Manager.NativeManager.GetKeyIndex(swig.Key.Escape)))
				{
					ShouldBeRemoved = true;
				}

				Manager.NativeManager.Separator();

				float dpiScale = Manager.DpiScale;
				float buttonWidth = 80 * dpiScale;
				float areaWidth = Manager.NativeManager.GetContentRegionAvail().X;

				Manager.NativeManager.SetCursorPosX((areaWidth - buttonWidth * 2 + 8 * dpiScale) / 2);

				if (Manager.NativeManager.Button("OK", buttonWidth))
				{
					this.selectedNode.Name.Value = this.name;
					ShouldBeRemoved = true;
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.Button(Resources.GetString("Cancel"), buttonWidth))
				{
					ShouldBeRemoved = true;
				}

				Manager.NativeManager.EndPopup();
			}
			else
			{
				ShouldBeRemoved = true;
			}

			isFirstUpdate = false;
		}
	}
}
