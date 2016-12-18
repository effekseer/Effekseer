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
	public partial class DockNodeGenerationLocationValues : DockContent
	{
		public DockNodeGenerationLocationValues()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(DockNodeGenerationLocationValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeGenerationLocationValues_HandleDestroyed);

			lp_GenerationLocation.SetType(typeof(Data.GenerationLocationValues));
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconGenerationLocation).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_GenerationLocation.SetValue(((Data.Node)Core.SelectedNode).GenerationLocationValues);
				}
				else
				{
					lp_GenerationLocation.SetValue(null);
				}
			}
			else
			{
				lp_GenerationLocation.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeGenerationLocationValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeGenerationLocationValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeGenerationLocationValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
