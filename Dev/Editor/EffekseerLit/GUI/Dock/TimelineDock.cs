using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class TimelineDock : DockPanel
	{
		public TimelineDock()
		{
			Label = Icons.PanelOptions + Resources.GetString("Options") + "###TimelineDock";

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = Resources.GetString("Options");

			NoPadding = true;
			NoScrollBar = false;
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
			if (Manager.NativeManager.BeginNodeFrameTimeline())
			{

				Manager.NativeManager.EndNodeFrameTimeline();
			}
		}

		void OnAfter(object sender, EventArgs e)
		{
		}
	}
}
