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
	public partial class DockFCurves : DockContent
	{
		public DockFCurves()
		{
			InitializeComponent();

			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconFCurve).GetHicon());
		}

		public void ScrollPosition(object o)
		{
			fCurves.ScrollPosition(o);
		}
	}
}
