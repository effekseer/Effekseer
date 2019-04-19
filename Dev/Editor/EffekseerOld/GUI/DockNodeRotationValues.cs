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
	public partial class DockNodeRotationValues : DockContent
	{
		public DockNodeRotationValues()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(DockNodeRotationValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeRotationValues_HandleDestroyed);

			lp_Rotation.SetType(typeof(Data.RotationValues));
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconRotation).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_Rotation.SetValue(((Data.Node)Core.SelectedNode).RotationValues);
				}
				else
				{
					lp_Rotation.SetValue(null);
				}
			}
			else
			{
				lp_Rotation.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeRotationValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeRotationValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeRotationValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
