using System;
namespace Effekseer.GUI.Dock
{
	class Culling : DockPanel
    {
        Component.ParameterList paramerterList = null;

        bool isFiestUpdate = true;

		public Culling()
        {
			Label = Resources.GetString("Culling") + "###Culling";

            paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.EffectCullingValues));

            Core.OnAfterLoad += OnAfterLoad;
            Core.OnAfterNew += OnAfterLoad;
            Core.OnAfterSelectNode += OnAfterSelectNode;

            Read();

			Icon = Images.GetIcon("PanelCulling");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("Culling");
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
			paramerterList.SetValue(Core.Culling);
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
