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
	public partial class DockNodeRendererCommonValues : DockContent
	{
		public DockNodeRendererCommonValues()
		{
			InitializeComponent();

			HandleCreated += new EventHandler(DockNodeRendererCommonValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeRendererCommonValues_HandleDestroyed);

			lp_RendererCommon.SetType(typeof(Data.RendererCommonValues));
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_RendererCommon.SetValue(((Data.Node)Core.SelectedNode).RendererCommonValues);
				}
				else
				{
					lp_RendererCommon.SetValue(null);
				}
			}
			else
			{
				lp_RendererCommon.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeSoundValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeRendererCommonValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}

		void DockNodeRendererCommonValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}
	}
}
