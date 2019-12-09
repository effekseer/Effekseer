using System;
namespace Effekseer.GUI.Dock
{   
	class BehaviorValues : DockPanel
    {
		Component.CopyAndPaste candp = null;
		Component.ParameterList paramerterList = null;

        bool isFiestUpdate = true;

		public BehaviorValues()
        {
			Label = Resources.GetString("Behavior") + "###Behavior";

            paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.EffectBehaviorValues));

			candp = new Component.CopyAndPaste("Behavior", GetTargetObject, Read);

            Core.OnAfterLoad += OnAfterLoad;
            Core.OnAfterNew += OnAfterLoad;
            Core.OnAfterSelectNode += OnAfterSelectNode;

            Read();

			Icon = Images.GetIcon("PanelBehavior");
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

			candp.Update();

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
