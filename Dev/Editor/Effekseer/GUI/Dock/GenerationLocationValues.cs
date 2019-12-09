using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class GenerationLocationValues : DockPanel
	{
		Component.CopyAndPaste candp = null;
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public GenerationLocationValues()
		{
			Label = Resources.GetString("SpawningMethod") + "###SpawningMethod";

			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.GenerationLocationValues));

			candp = new Component.CopyAndPaste("SpawningMethod", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;
			
			Read();

			Icon = Images.GetIcon("PanelGenerationLocation");
			TabToolTip = Resources.GetString("SpawningMethod");
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

		object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).GenerationLocationValues;
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
