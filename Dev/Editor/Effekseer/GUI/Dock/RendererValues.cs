using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class RendererValues : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;
		Data.RendererValues currentValues = null;

		public RendererValues()
		{
			Label = Icons.PanelRender + MultiLanguageTextProvider.GetText("RenderSettings") + "###RenderSettings";
			DocPage = string.Empty;

			paramerterList = new BindableComponent.ParameterList();
			paramerterList.SetType(typeof(Data.RendererValues));

			CopyAndPaste = new BindableComponent.CopyAndPaste("RenderSettings", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Controls.Add(paramerterList);

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("RenderSettings");
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
					return ((Data.Node)Core.SelectedNode).DrawingValues;
				}
			}
			return null;
		}

		void Read()
		{
			paramerterList.SetValue(GetTargetObject());
			UpdateDocPage();
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			if (currentValues != null)
			{
				currentValues.Type.OnChanged -= OnTypeChanged;
			}
			currentValues = (Data.RendererValues)GetTargetObject();
			if (currentValues != null)
			{
				currentValues.Type.OnChanged += OnTypeChanged;
			}

			Read();
		}

		void OnTypeChanged(object sender, ChangedValueEventArgs e)
		{
			UpdateDocPage();
		}

		void UpdateDocPage()
		{
			if (currentValues != null)
			{
				switch (currentValues.Type.Value)
				{
					case Data.RendererValues.ParamaterType.Sprite:
						DocPage = "rendererSprite.html";
						break;
					case Data.RendererValues.ParamaterType.Ribbon:
						DocPage = "rendererRibbon.html";
						break;
					case Data.RendererValues.ParamaterType.Ring:
						DocPage = "rendererRing.html";
						break;
					case Data.RendererValues.ParamaterType.Model:
						DocPage = "rendererModel.html";
						break;
					case Data.RendererValues.ParamaterType.Track:
						DocPage = "rendererTrack.html";
						break;
					default:
						DocPage = string.Empty;
						break;
				}
			}
			else
			{
				DocPage = string.Empty;
			}
		}
	}
}