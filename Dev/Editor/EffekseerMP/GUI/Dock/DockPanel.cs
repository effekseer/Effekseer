using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class DockPanel : IRemovableControl
    {
        public bool ShouldBeRemoved { get; private set; } = false;

        public void Update()
        {
            bool[] opened = new[] { true };

            if(Manager.NativeManager.Begin("###Dock", opened))
            {
                UpdateInternal();
                Manager.NativeManager.End();
            }
        }

        protected virtual void UpdateInternal()
        {

        }
    }
}
