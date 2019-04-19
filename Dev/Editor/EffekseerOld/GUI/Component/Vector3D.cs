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
	public partial class Vector3D : UserControl
	{
		public Vector3D()
		{
			InitializeComponent();

			this.SuspendLayout();
			Anchor = AnchorStyles.Left | AnchorStyles.Right;
			this.ResumeLayout(false);

			HandleDestroyed += new EventHandler(Vector3D_HandleDestroyed);
		}

		Data.Value.Vector3D binding = null;

		bool enableUndo = true;
		public bool EnableUndo
		{
			get
			{
				return enableUndo;
			}
			set
			{
				enableUndo = value;
				tb_x.EnableUndo = enableUndo;
				tb_y.EnableUndo = enableUndo;
				tb_z.EnableUndo = enableUndo;
			}
		}

		public Data.Value.Vector3D Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				if (binding != null)
				{
					tb_x.Binding = null;
					tb_y.Binding = null;
					tb_z.Binding = null;
				}

				binding = value;

				if (binding != null)
				{
					tb_x.Binding = binding.X;
					tb_y.Binding = binding.Y;
					tb_z.Binding = binding.Z;
				}
			}
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Vector3D;
			Binding = o_;
		}

		void Vector3D_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}
	}
}
