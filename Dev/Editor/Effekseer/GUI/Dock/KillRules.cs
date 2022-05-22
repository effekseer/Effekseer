using System;
using Effekseer.GUI.Component;
using IronPython.Compiler.Ast;

namespace Effekseer.GUI.Dock
{
	public class KillRules: DockPanel
	{
		Component.CopyAndPaste candp = null;
		Component.ParameterList parameterList = null;
		
		public KillRules()
		{
			Label = Icons.PanelLOD + "KIll Rules" + "###KillRuls";

			parameterList = new ParameterList();
			parameterList.SetType(typeof(Data.KillRulesValues));

			candp = new CopyAndPaste("KillRules", GetTargetObject, Read);
			
			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = "Kill Rules";
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
			candp.Update();
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