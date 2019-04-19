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
	public partial class DockEffectBehavior : DockContent
	{
		public DockEffectBehavior()
		{
			InitializeComponent();
			HandleCreated += new EventHandler(DockEffectBehavior_HandleCreated);
			HandleDestroyed += new EventHandler(DockEffectBehavior_HandleDestroyed);

			lp_behavior.SetType(typeof(Data.EffectBehaviorValues));

            Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconBehavior).GetHicon());
        }

		void Read()
		{
			lp_behavior.SetValue(Core.EffectBehavior);
		}

		void OnAfter(object sender, EventArgs e)
		{
			Read();
		}

		void DockEffectBehavior_HandleCreated(object sender, EventArgs e)
		{
			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;
		}

		void DockEffectBehavior_HandleDestroyed(object sender, EventArgs e)
		{
			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		private void DockEffectBehavior_Load(object sender, EventArgs e)
		{
			if (Core.EffectBehavior != null)
			{
				Read();
			}
		}
	}
}
