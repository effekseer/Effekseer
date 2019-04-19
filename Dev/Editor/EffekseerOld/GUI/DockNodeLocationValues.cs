using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	public partial class DockNodeLocationValues : DockContent
	{
		public DockNodeLocationValues()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(DockNodeTranslationValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeTranslationValues_HandleDestroyed);

			lp_Translation.SetType(typeof(Data.LocationValues));

			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconLocation).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_Translation.SetValue(((Data.Node)Core.SelectedNode).LocationValues);
				}
				else
				{
					lp_Translation.SetValue(null);
				}
			}
			else
			{
				lp_Translation.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeTranslationValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeTranslationValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeTranslationValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
