using System;
namespace Effekseer.GUI.Dock
{
	class BehaviorValues : DockPanel
	{
		BindableComponent.ParameterList paramerterList = null;

		bool isFiestUpdate = true;

		public BehaviorValues()
		{
			Label = Icons.PanelBehavior + MultiLanguageTextProvider.GetText("Behavior") + "###Behavior";
			DocPage = "behavior.html";

			paramerterList = new BindableComponent.ParameterList();
			paramerterList.SetType(typeof(Data.EffectBehaviorValues));

			CopyAndPaste = new BindableComponent.CopyAndPaste("Behavior", GetTargetObject, Read);

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSelectNode += OnAfterSelectNode;

			Read();

			TabToolTip = MultiLanguageTextProvider.GetText("Behavior");
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