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
	public partial class DockNodeLocationAbsValues : DockContent
	{
		public DockNodeLocationAbsValues()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(DockNodeTranslationAbsValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeTranslationAbsValues_HandleDestroyed);

			lp_TranslationAbs.SetType(typeof(Data.LocationAbsValues));
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconLocationAbs).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_TranslationAbs.SetValue(((Data.Node)Core.SelectedNode).LocationAbsValues);
				}
				else
				{
					lp_TranslationAbs.SetValue(null);
				}
			}
			else
			{
				lp_TranslationAbs.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeTranslationAbsValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeTranslationAbsValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeTranslationAbsValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
