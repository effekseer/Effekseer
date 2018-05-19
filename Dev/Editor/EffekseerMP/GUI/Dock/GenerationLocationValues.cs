using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class GenerationLocationValues : DockPanel
	{
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public GenerationLocationValues()
		{
			Label = Resources.GetString("SpawningMethod") + "###SpawningMethod";

			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.GenerationLocationValues));

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;
			
			Read();

			Icon = Images.GetIcon("PanelGenerationLocation");
			IconSize = new swig.Vec2(24, 24);
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

			paramerterList.Update();
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					paramerterList.SetValue(((Data.Node)Core.SelectedNode).GenerationLocationValues);
				}
				else
				{
					paramerterList.SetValue(null);
				}
			}
			else
			{
				paramerterList.SetValue(null);
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
