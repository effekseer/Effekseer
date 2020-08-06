using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class AdvancedRenderCommonValues2 : DockPanel
    {
        Component.CopyAndPaste candp = null;
        Component.ParameterList parameterList = null;

        bool isFirstUpdate = true;

        public AdvancedRenderCommonValues2()
        {
            Label = Resources.GetString("AdvancedRenderSettings2") + "###AdvancedRenderSettings2";

            parameterList = new Component.ParameterList();
            parameterList.SetType(typeof(Data.AdvancedRenderCommonValues2));

            candp = new Component.CopyAndPaste("AdvancedRenderSettings2", GetTargetObject, Read);

            Core.OnAfterLoad += OnAfterLoad;
            Core.OnAfterNew += OnAfterLoad;
            Core.OnAfterSelectNode += OnAfterSelectNode;

            Read();

            TabToolTip = Resources.GetString("AdvancedRenderSettings2");
        }

        public void FixValues()
        {
            parameterList.FixValues();
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
            if (isFirstUpdate)
            {
            }

            candp.Update();

            parameterList.Update();
        }

        object GetTargetObject()
        {
#if __EFFEKSEER_BUILD_VERSION16__
			if(Core.SelectedNode != null)
            {
                if (Core.SelectedNode is Data.Node)
                {
                    return ((Data.Node)Core.SelectedNode).AdvancedRendererCommonValues2Values;
                }
            }
            return null;
#else
			return null;
#endif
		}

        void Read()
        {
            parameterList.SetValue(GetTargetObject());
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
