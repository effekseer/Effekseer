using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Menu
{
	class Menu : IControl
	{
		string id = "";

		public object Label { get; set; } = null;

		public bool ShouldBeRemoved { get; private set; } = false;

		internal Utils.DelayedList<IControl> Controls = new Utils.DelayedList<IControl>();

		public Menu(MultiLanguageString label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void Update()
		{
			if (Manager.NativeManager.BeginMenu(Label + id))
			{
				Controls.Lock();
				foreach (var ctrl in Controls.Internal)
				{
					ctrl.Update();
				}
				Controls.Unlock();

				Manager.NativeManager.EndMenu();
			}
		}
	}
}
