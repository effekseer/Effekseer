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
	public partial class DockCulling : DockContent
	{
		public DockCulling()
		{
			InitializeComponent();
			HandleCreated += new EventHandler(DockCulling_HandleCreated);
			HandleDestroyed += new EventHandler(DockCulling_HandleDestroyed);

			lp_culling.SetType(typeof(Data.EffectCullingValues));

            Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconCulling).GetHicon());
        }

		void Read()
		{
			lp_culling.SetValue(Core.Culling);
		}

		void OnAfter(object sender, EventArgs e)
		{
			Read();
		}

		void DockCulling_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;
		}

		void DockCulling_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		private void DockCulling_Load(object sender, EventArgs e)
		{
			if (Core.Culling != null)
			{
				Read();
			}
		}
	}
}
