using System;

namespace Effekseer.GUI.Dock
{
	internal class RotationValues : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public RotationValues()
		{
			Label = Icons.PanelRotation + MultiLanguageTextProvider.GetText("Rotation") + "###Rotation";
			DocPage = "rotation.html";

			paramerterList = new BindableComponent.ParameterList();
			paramerterList.SetType(typeof(Data.RotationValues));

			CopyAndPaste = new BindableComponent.CopyAndPaste("Rotation", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("Rotation");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
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

			paramerterList.Update();
		}

		private object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).RotationValues;
				}
			}
			return null;
		}

		private void Read()
		{
			paramerterList.SetValue(GetTargetObject());
		}

		private void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		private void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}
	}
}