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
			paramerterList.SetType(typeof(Data.CullingValues));

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
					paramerterList.SetValue(Core.Culling);
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
