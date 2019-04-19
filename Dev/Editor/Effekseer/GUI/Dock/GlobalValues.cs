using System;
namespace Effekseer.GUI.Dock
{
	class GlobalValues : DockPanel
    {
        Component.ParameterList paramerterList = null;

        bool isFiestUpdate = true;

		public GlobalValues()
        {
			Label = Resources.GetString("Global") + "###Global";

            paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.GlobalValues));

            Core.OnAfterLoad += OnAfterLoad;
            Core.OnAfterNew += OnAfterLoad;
            Core.OnAfterSelectNode += OnAfterSelectNode;

            Read();

			Icon = Images.GetIcon("PanelGlobal");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("Global");
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
			paramerterList.SetValue(Core.Global);
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
