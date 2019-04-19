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
	public partial class DockContent : WeifenLuo.WinFormsUI.Docking.DockContent
	{
		public DockContent()
		{
			InitializeComponent();
		}

		protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
		{
			bool handle = false;
			Shortcuts.ProcessCmdKey(ref msg, keyData, ref handle);
			if (handle) return true;

			return base.ProcessCmdKey(ref msg, keyData);
		}
	}
}
