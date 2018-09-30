using System;
namespace Effekseer.GUI.Dock
{   
	class BehaviorValues : DockPanel
    {
        Component.ParameterList paramerterList = null;

        bool isFiestUpdate = true;

		public BehaviorValues()
        {
			Label = Resources.GetString("Behavior") + "###Behavior";

            paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.EffectBehaviorValues));

            Core.OnAfterLoad += OnAfterLoad;
            Core.OnAfterNew += OnAfterLoad;
            Core.OnAfterSelectNode += OnAfterSelectNode;

            Read();

			Icon = Images.GetIcon("PanelBehavior");
			IconSize = new swig.Vec2(24, 24);
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

            paramerterList.Update();
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
