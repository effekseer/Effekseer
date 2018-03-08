using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
    class Manager
    {
        internal static swig.GUIManager NativeManager;

        internal static List<IRemovableControl> Controls = new List<IRemovableControl>();

        public static void Update()
        {
            foreach (var c in Controls)
            {
                c.Update();
            }

            Controls.RemoveAll(_ => _.ShouldBeRemoved);
        }
    }
}
