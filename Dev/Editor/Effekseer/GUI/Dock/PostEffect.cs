/*
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class PostEffect : DockPanel
	{
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public PostEffect()
		{
			Label = Resources.GetString("PostEffect") + "###PostEffect";
			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.EnvironmentValues));

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			Icon = Images.GetIcon("PanelPostEffect");
			TabToolTip = Resources.GetString("Options");
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
			if(isFiestUpdate)
			{
				paramerterList.SetValue(Core.Environment);
				isFiestUpdate = false;
			}

			paramerterList.Update();
		}

		void OnAfter(object sender, EventArgs e)
		{
			paramerterList.SetValue(Core.Environment);
		}
	}
}
*/