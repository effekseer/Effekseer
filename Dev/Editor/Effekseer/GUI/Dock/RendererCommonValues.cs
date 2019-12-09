using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class RendererCommonValues : DockPanel
	{
		Component.CopyAndPaste candp = null;
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public RendererCommonValues()
		{
			Label = Resources.GetString("BasicRenderSettings") + "###BasicRenderSettings";

			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.RendererCommonValues));

			candp = new Component.CopyAndPaste("BasicRenderSettings", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Controls.Add(candp);
			Controls.Add(paramerterList);
			
			Read();

			Icon = Images.GetIcon("PanelRendererCommon");
			TabToolTip = Resources.GetString("BasicRenderSettings");
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
		}

		object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).RendererCommonValues;
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
