using System;
namespace Effekseer.GUI.Dock
{
	class GlobalValues : DockPanel
    {
        Component.ParameterList paramerterList = null;

        bool isFiestUpdate = true;

		public GlobalValues()
        {
			Label = Resources.GetString("Globa") + "###Global";

            paramerterList = new Component.ParameterList();
			paramerterList.SetType(typeof(Data.GlobalValues));

            Core.OnAfterLoad += OnAfterLoad;
            Core.OnAfterNew += OnAfterLoad;
            Core.OnAfterSelectNode += OnAfterSelectNode;

            Read();
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
            if (Core.SelectedNode != null)
            {
                if (Core.SelectedNode is Data.Node)
                {
					paramerterList.SetValue(Core.Global);
                }
                else
                {
                    paramerterList.SetValue(null);
                }
            }
            else
            {
                paramerterList.SetValue(null);
            }
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
