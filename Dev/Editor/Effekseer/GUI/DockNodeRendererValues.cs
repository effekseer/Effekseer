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
	public partial class DockNodeRendererValues : DockContent
	{
		public DockNodeRendererValues()
		{
			InitializeComponent();
			HandleCreated += new EventHandler(DockNodeDrawingValues_HandleCreated);
			HandleDestroyed += new EventHandler(DockNodeDrawingValues_HandleDestroyed);

			lp_Renderer.SetType(typeof(Data.RendererValues));
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconRenderer).GetHicon());
		}

		void Read()
		{
			if (Core.SelectedNode != null)
			{
				if (Core.SelectedNode is Data.Node)
				{
					lp_Renderer.SetValue(((Data.Node)Core.SelectedNode).DrawingValues);
				}
				else
				{
					lp_Renderer.SetValue(null);
				}
			}
			else
			{
				lp_Renderer.SetValue(null);
			}
		}

		void OnAfterSelectNode(object sender, EventArgs e)
		{
			Read();
		}

		private void DockNodeDrawingValues_Load(object sender, EventArgs e)
		{
			Read();
		}

		void DockNodeDrawingValues_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode += OnAfterSelectNode;
		}

		void DockNodeDrawingValues_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterSelectNode -= OnAfterSelectNode;
		}
	}
}
