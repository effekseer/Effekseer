using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class Environement : DockPanel
	{
		Component.CopyAndPaste candp = null;
		Component.ParameterList paramerterListBackground = null;
		Component.ParameterList paramerterListLighting = null;
		Component.ParameterList paramerterListPostEffect = null;

		bool isFiestUpdate = true;

		public Environement()
		{
			Label = Resources.GetString("Environment_Name") + "###Environment";
			
			paramerterListBackground = new Component.ParameterList();
			paramerterListBackground.SetType(typeof(Data.EnvironmentBackgroundValues));

			paramerterListLighting = new Component.ParameterList();
			paramerterListLighting.SetType(typeof(Data.EnvironmentLightingValues));

			paramerterListPostEffect = new Component.ParameterList();
			paramerterListPostEffect.SetType(typeof(Data.EnvironmentPostEffectValues));

			candp = new Component.CopyAndPaste("Environment", GetTargetObject, null);

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			Icon = Images.GetIcon("PanelEnvironment");
			TabToolTip = Resources.GetString("Environment_Name");
		}

		public void FixValues()
		{
			paramerterListBackground.FixValues();
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
				paramerterListBackground.SetValue(Core.Environment.Background);
				paramerterListLighting.SetValue(Core.Environment.Lighting);
				paramerterListPostEffect.SetValue(Core.Environment.PostEffect);
				isFiestUpdate = false;
			}

			candp.Update();

			if (Manager.NativeManager.TreeNode(Resources.GetString("Environment_Background")))
			{
				paramerterListBackground.Update();
				Manager.NativeManager.TreePop();
			}

			if (Manager.NativeManager.TreeNode(Resources.GetString("Environment_Lighting")))
			{
				paramerterListLighting.Update();
				Manager.NativeManager.TreePop();
			}

			if (Manager.NativeManager.TreeNode(Resources.GetString("Environment_PostEffect")))
			{
				paramerterListPostEffect.Update();
				Manager.NativeManager.TreePop();
			}
		}

		object GetTargetObject()
		{
			return Core.Environment;
		}

		void OnAfter(object sender, EventArgs e)
		{
			paramerterListBackground.SetValue(Core.Environment);
		}
	}
}
