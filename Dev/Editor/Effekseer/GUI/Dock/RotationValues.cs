using System;

namespace Effekseer.GUI.Dock
{
	internal class RotationValues : DockPanel
	{
		Component.CopyAndPaste candp = null;
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public RotationValues()
		{
			Label = Icons.PanelRotation + Resources.GetString("Rotation") + "###Rotation";

			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.RotationValues));

			candp = new Component.CopyAndPaste("Rotation", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;
			
			Read();

			TabToolTip = Resources.GetString("Rotation");
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

			candp.Update();

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
