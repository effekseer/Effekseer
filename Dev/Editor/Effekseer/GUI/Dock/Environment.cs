using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Environement : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public Environement()
		{
			Label = Icons.PanelEnvironment + MultiLanguageTextProvider.GetText("Environment_Name") + "###Environment";
			DocPage = "environment.html";

			paramerterList = new BindableComponent.ParameterList();

			CopyAndPaste = new BindableComponent.CopyAndPaste("Environment", GetTargetObject, null);

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = MultiLanguageTextProvider.GetText("Environment_Name");
		}

		public void FixValues()
		{
			paramerterList.FixValues();
		}

		public override void OnDisposed()
		{
			FixValues();

			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		protected override void UpdateInternal()
		{
			if (isFiestUpdate)
			{
				paramerterList.SetValue(Core.Environment);
				isFiestUpdate = false;
			}

			paramerterList.Update();
		}

		object GetTargetObject()
		{
			return Core.Environment;
		}

		void OnAfter(object sender, EventArgs e)
		{
			paramerterList.SetValue(Core.Environment);
		}
	}
}