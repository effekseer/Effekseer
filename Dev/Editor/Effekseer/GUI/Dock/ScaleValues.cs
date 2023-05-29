using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class ScaleValues : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public ScaleValues()
		{
			Label = Icons.PanelScale + MultiLanguageTextProvider.GetText("Scale") + "###Scale";
			DocPage = "scale.html";

			paramerterList = new BindableComponent.ParameterList();
			paramerterList.SetType(typeof(Data.ScaleValues));

			CopyAndPaste = new BindableComponent.CopyAndPaste("Scale", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("Scale");
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

		object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).ScalingValues;
				}
			}
			return null;
		}

		void Read()
		{
			paramerterList.SetValue(GetTargetObject());
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