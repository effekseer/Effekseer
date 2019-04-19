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
			paramerterList.SetType(typeof(Data.PostEffectValues));

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			Icon = Images.GetIcon("PanelPostEffect");
			IconSize = new swig.Vec2(24, 24);
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
				paramerterList.SetValue(Core.PostEffect);
				isFiestUpdate = false;
			}

			paramerterList.Update();
		}

		void OnAfter(object sender, EventArgs e)
		{
			paramerterList.SetValue(Core.PostEffect);
		}
	}
}
