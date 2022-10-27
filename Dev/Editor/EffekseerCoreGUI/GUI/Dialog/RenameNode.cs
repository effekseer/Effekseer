using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.Data;

namespace Effekseer.GUI.Dialog
{
	public class RenameNode : IRemovableControl
	{

		bool opened = true;

		bool isFirstUpdate = true;

		public bool ShouldBeRemoved { get; private set; } = false;

		NodeBase selectedNode;

		/// <summary>
		/// TODO : Refactor
		/// </summary>
		EffectAsset.Node nodeAsset;

		string name;

		public void Show(NodeBase node)
		{
			this.selectedNode = node;
			this.name = node.Name;
			Manager.AddControl(this);
		}

		public void Show(EffectAsset.Node nodeAsset)
		{
			this.nodeAsset = nodeAsset;
			this.name = nodeAsset.Name;
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
				void changeNameToExit(string value)
				{
					if (selectedNode != null)
					{
						this.selectedNode.Name.Value = value;
					}
					else if (nodeAsset != null)
					{
						if (this.nodeAsset.Name != value)
						{
							CoreContext.SelectedEffect.Context.StartEditFields(CoreContext.SelectedEffectNode);

							this.nodeAsset.Name = value;

							CoreContext.SelectedEffect.Context.NotifyEditFields(CoreContext.SelectedEffectNode);

							CoreContext.SelectedEffect.Context.EndEditFields(CoreContext.SelectedEffectNode);
						}
					}

					ShouldBeRemoved = true;

				}

				if (isFirstUpdate)
				{
					Manager.NativeManager.SetKeyboardFocusHere();
				}

				if (Manager.NativeManager.InputText("###RenameNodeInput", this.name,
					swig.InputTextFlags.AutoSelectAll | swig.InputTextFlags.EnterReturnsTrue))
				{
					changeNameToExit(this.name);
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
					changeNameToExit(this.name);
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