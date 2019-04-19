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
	public partial class DockGlobal : DockContent
	{
		public DockGlobal()
		{
			InitializeComponent();
			HandleCreated += new EventHandler(DockOption_HandleCreated);
			HandleDestroyed += new EventHandler(DockOption_HandleDestroyed);

			lp_option.SetType(typeof(Data.GlobalValues));

            Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconOption).GetHicon());
        }

		void Read()
		{
			lp_option.SetValue(Core.Global);
		}

		void OnAfter(object sender, EventArgs e)
		{
			Read();
		}

		void DockOption_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;
		}

		void DockOption_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		private void DockOption_Load(object sender, EventArgs e)
		{
			if (Core.Option != null)
			{
				Read();
			}
		}
	}
}
