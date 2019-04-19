using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace Effekseer.GUI
{
	public partial class AboutForm : Form
	{
		public AboutForm()
		{
			InitializeComponent();
			
			subject.Text = "Effekseer Version " + Core.Version;
		}

		private void link1_Click(object sender, EventArgs e)
		{
			Process.Start(link1.Text);
		}

		private void link2_Click(object sender, EventArgs e)
		{
			Process.Start(link2.Text);
		}

		private void ok_Click(object sender, EventArgs e)
		{
			Close();
		}
	}
}
