using System;
using Effekseer.GUI.BindableComponent;

namespace Effekseer.GUI.Dock
{
	public class KillRules: DockPanel
	{
		BindableComponent.ParameterList parameterList = null;
		
		public KillRules()
		{
			Label = Icons.PanelKillRules + MultiLanguageTextProvider.GetText("KillRules") + "###KillRules";
			DocPage = "killRules.html";

			parameterList = new ParameterList();
			parameterList.SetType(typeof(Data.KillRulesValues));

			CopyAndPaste = new CopyAndPaste("KillRules", GetTargetObject, Read);
			
			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("KillRules");
		}

		public override void OnDisposed()
		{
			parameterList.FixValues();
			
			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}

		protected override void UpdateInternal()
		{
			parameterList.Update();
		}

		object GetTargetObject()
		{
			if (Core.SelectedNode != null && Core.SelectedNode is Data.Node)
			{
				return ((Data.Node)Core.SelectedNode).KillRulesValues;
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