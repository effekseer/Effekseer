using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Menu
{
    class MainMenu : IRemovableControl
    {
        public bool ShouldBeRemoved { get; private set; } = false;

        internal List<IControl> Controls = new List<IControl>();

        public void Update()
        {
            Manager.NativeManager.BeginMainMenuBar();

            foreach (var ctrl in Controls)
            {
                ctrl.Update();
            }

            Manager.NativeManager.EndMainMenuBar();
        }
    }
}
