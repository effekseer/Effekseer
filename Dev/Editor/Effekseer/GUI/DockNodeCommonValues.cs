using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	public partial class DockNodeCommonValues : DockContent
	{
		public DockNodeCommonValues()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(DockNodeCommonValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeCommonValues_HandleDestroyed);

			lp_Common.SetType(typeof(Data.CommonValues));
			lp_Node.SetType(typeof(Data.NodeBase));
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconCommon).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				lp_Node.SetValue(Core.SelectedNode);
				if (Core.SelectedNode is Data.Node)
				{
					lp_Common.SetValue(((Data.Node)Core.SelectedNode).CommonValues);
				}
				else
				{
					lp_Common.SetValue(null);
				}
			}
			else
			{
				lp_Node.SetValue(null);
				lp_Common.SetValue(null);
			}
		}

		private void DockNodeCommonValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeCommonValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeCommonValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
