using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Environement : DockPanel
	{
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public Environement()
		{
			Label = Icons.PanelEnvironment + Resources.GetString("Environment_Name") + "###Environment";
			DocPage = "environment.html";

			paramerterList = new Component.ParameterList();

			CopyAndPaste = new Component.CopyAndPaste("Environment", GetTargetObject, null);

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = Resources.GetString("Environment_Name");
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

			Manager.NativeManager.Separator();
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