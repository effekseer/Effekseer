using System;
namespace Effekseer.GUI.Dock
{
	class BehaviorValues : DockPanel
	{
		Component.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public BehaviorValues()
		{
			Label = Icons.PanelBehavior + Resources.GetString("Behavior") + "###Behavior";
			DocPage = "behavior.html";

			paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.EffectBehaviorValues));

			CopyAndPaste = new Component.CopyAndPaste("Behavior", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = Resources.GetString("Behavior");
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

			Manager.NativeManager.Separator();
			paramerterList.Update();
		}

		object GetTargetObject()
		{
			return Core.EffectBehavior;
		}

		void Read()
		{
			paramerterList.SetValue(Core.EffectBehavior);
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