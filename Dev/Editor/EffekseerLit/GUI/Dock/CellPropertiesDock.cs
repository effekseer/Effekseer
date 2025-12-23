using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class CellPropertiesDock : DockPanel
	{
		public CellPropertiesDock()
		{
			Label = Icons.PanelOptions + MultiLanguageTextProvider.GetText("Options") + "###CellPropertiesDock";

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = MultiLanguageTextProvider.GetText("Options");

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
		}

		void OnAfter(object sender, EventArgs e)
		{
		}
	}
}