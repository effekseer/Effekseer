using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class PrefabListDock : DockPanel
	{
		public PrefabListDock()
		{
			Label = Icons.PanelOptions + Resources.GetString("Options") + "###PrefabListDock";

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = Resources.GetString("Options");

			NoPadding = true;
			NoScrollBar = true;
			NoCloseButton = true;
			AllowsShortTab = false;
		}

		public override void OnDisposed()
		{
			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		protected override void UpdateInternal()
		{
			if (Manager.NativeManager.TreeNodeEx("test", swig.TreeNodeFlags.Bullet))
			{
				// D&D Source
				if (Manager.NativeManager.BeginDragDropSource())
				{
					byte[] idBuf = new byte[5];
					if (Manager.NativeManager.SetDragDropPayload("PayloadName", idBuf, idBuf.Length))
					{
					}
					Manager.NativeManager.Text("test2");

					Manager.NativeManager.EndDragDropSource();
				}


				Manager.NativeManager.TreePop();
			}
		}

		void OnAfter(object sender, EventArgs e)
		{
		}
	}
}
