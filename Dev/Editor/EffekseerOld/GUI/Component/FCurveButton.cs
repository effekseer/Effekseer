using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI.Component
{
	public partial class FCurveButton : Button
	{
		public FCurveButton()
		{
			InitializeComponent();

            Text = Properties.Resources.FCurves;

			Click += new EventHandler(FCurveButton_Click);
		}

		object boundObject = null;

		public bool EnableUndo { get; set; }

		public void SetBinding(object o)
		{
			boundObject = o;
		}

		void FCurveButton_Click(object sender, EventArgs e)
		{
			if (boundObject == null) return;

			var opened = GUIManager.DockFCurves != null && GUIManager.DockFCurves.Created;
			
			GUIManager.SelectOrShowWindow(typeof(DockFCurves));

			if (!opened)
			{
				// うまく行かない
				GUIManager.DockFCurves.Size = new System.Drawing.Size(600, 400);
			}

			GUIManager.DockFCurves.ScrollPosition(boundObject);
		}
	}
}
