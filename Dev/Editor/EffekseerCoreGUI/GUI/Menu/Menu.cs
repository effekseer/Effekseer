using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Menu
{
	public class Menu : IControl
	{
		string id = "";

		public object Label { get; set; } = null;

		public string Icon { get; set; } = null;

		public bool ShouldBeRemoved { get; private set; } = false;

		public Utils.DelayedList<IControl> Controls = new Utils.DelayedList<IControl>();
	
		public Menu(MultiLanguageString label = null, string icon = null)
		{
			if (label != null)
			{
				Label = label;
			}
			Icon = icon;

			var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void Update()
		{
			string nativeLabel = "";
			if (Icon != null) nativeLabel += Icon + " ";
			if (Label != null) nativeLabel += Label;
			nativeLabel += id;

			if (Manager.NativeManager.BeginMenu(nativeLabel))
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
