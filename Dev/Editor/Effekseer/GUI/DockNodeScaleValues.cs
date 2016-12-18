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
	public partial class DockNodeScaleValues : DockContent
	{
		public DockNodeScaleValues()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(DockNodeScalingValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeScalingValues_HandleDestroyed);

			lp_Scale.SetType(typeof(Data.ScaleValues));
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconScale).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_Scale.SetValue(((Data.Node)Core.SelectedNode).ScalingValues);
				}
				else
				{
					lp_Scale.SetValue(null);
				}
			}
			else
			{
				lp_Scale.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeScalingValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeScalingValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeScalingValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
