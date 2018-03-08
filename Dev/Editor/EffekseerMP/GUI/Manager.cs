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

		static List<IRemovableControl> addingControls = new List<IRemovableControl>();

		public static void AddControl(IRemovableControl control)
		{
			addingControls.Add(control);
		}

        public static void Update()
        {
			foreach (var c in Controls)
			{
				c.Update();
			}

			foreach (var c in addingControls)
			{
				Controls.Add(c);
			}

			addingControls.Clear();

			Controls.RemoveAll(_ => _.ShouldBeRemoved);
		}
	}
}
