using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class AdvancedRenderCommonValues : DockPanel
	{
		BindableComponent.ParameterList parameterList = null;

		bool isFirstUpdate = true;

		public AdvancedRenderCommonValues()
		{
			Label = Icons.PanelAdvancedRenderCommon + MultiLanguageTextProvider.GetText("AdvancedRenderSettings") + "###AdvancedRenderSettings";
			DocPage = "rendererCommonAd.html";

			parameterList = new BindableComponent.ParameterList();
			parameterList.SetType(typeof(Data.AdvancedRenderCommonValues));

			CopyAndPaste = new BindableComponent.CopyAndPaste("AdvancedRenderSettings", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Controls.Add(parameterList);

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("AdvancedRenderSettings");
		}

		public void FixValues()
		{
			parameterList.FixValues();
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
			if (isFirstUpdate)
			{
			}
		}

		object GetTargetObject()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					return ((Data.Node)Core.SelectedNode).AdvancedRendererCommonValuesValues;
				}
			}
			return null;
		}

		void Read()
		{
			parameterList.SetValue(GetTargetObject());
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