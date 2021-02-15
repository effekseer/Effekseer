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
			GUIManager.AddControl(this);
		}

		public void Update()
		{
			if (isFirstUpdate)
			{
				GUIManager.NativeManager.OpenPopup(Resources.GetString("RenameNode") + "###RenameNodeDialog");
			}

			if (GUIManager.NativeManager.BeginPopupModal(Resources.GetString("RenameNode") + "###RenameNodeDialog", ref opened, swig.WindowFlags.AlwaysAutoResize))
			{
				if (isFirstUpdate)
				{
					GUIManager.NativeManager.SetKeyboardFocusHere();
				}

				if (GUIManager.NativeManager.InputText("###RenameNodeInput", this.name, 
					swig.InputTextFlags.AutoSelectAll | swig.InputTextFlags.EnterReturnsTrue))
				{
					this.selectedNode.Name.Value = this.name;
					ShouldBeRemoved = true;
				}
				this.name = GUIManager.NativeManager.GetInputTextResult();
				
				if (GUIManager.NativeManager.IsKeyPressed(GUIManager.NativeManager.GetKeyIndex(swig.Key.Escape)))
				{
					ShouldBeRemoved = true;
				}

				GUIManager.NativeManager.Separator();

				float dpiScale = GUIManager.DpiScale;
				float buttonWidth = 80 * dpiScale;
				float areaWidth = GUIManager.NativeManager.GetContentRegionAvail().X;

				GUIManager.NativeManager.SetCursorPosX((areaWidth - buttonWidth * 2 + 8 * dpiScale) / 2);

				if (GUIManager.NativeManager.Button("OK", buttonWidth))
				{
					this.selectedNode.Name.Value = this.name;
					ShouldBeRemoved = true;
				}

				GUIManager.NativeManager.SameLine();

				if (GUIManager.NativeManager.Button(Resources.GetString("Cancel"), buttonWidth))
				{
					ShouldBeRemoved = true;
				}

				GUIManager.NativeManager.EndPopup();
			}
			else
			{
				ShouldBeRemoved = true;
			}

			isFirstUpdate = false;
		}
	}
}
